//this file is autogenerated using stringify.bat (premake --stringify) in the build folder of this project
static const char* parallelLinearBvhCL= \
"/*\n"
"This software is provided 'as-is', without any express or implied warranty.\n"
"In no event will the authors be held liable for any damages arising from the use of this software.\n"
"Permission is granted to anyone to use this software for any purpose,\n"
"including commercial applications, and to alter it and redistribute it freely,\n"
"subject to the following restrictions:\n"
"1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.\n"
"2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.\n"
"3. This notice may not be removed or altered from any source distribution.\n"
"*/\n"
"//Initial Author Jackson Lee, 2014\n"
"typedef float b3Scalar;\n"
"typedef float4 b3Vector3;\n"
"#define b3Max max\n"
"#define b3Min min\n"
"typedef struct\n"
"{\n"
"	unsigned int m_key;\n"
"	unsigned int m_value;\n"
"} SortDataCL;\n"
"typedef struct \n"
"{\n"
"	union\n"
"	{\n"
"		float4	m_min;\n"
"		float   m_minElems[4];\n"
"		int			m_minIndices[4];\n"
"	};\n"
"	union\n"
"	{\n"
"		float4	m_max;\n"
"		float   m_maxElems[4];\n"
"		int			m_maxIndices[4];\n"
"	};\n"
"} b3AabbCL;\n"
"unsigned int interleaveBits(unsigned int x)\n"
"{\n"
"	//........ ........ ......12 3456789A	//x\n"
"	//....1..2 ..3..4.. 5..6..7. .8..9..A	//x after interleaving bits\n"
"	\n"
"	//........ ....1234 56789A12 3456789A	//x |= (x << 10)\n"
"	//........ ....1111 1....... ...11111	//0x 00 0F 80 1F\n"
"	//........ ....1234 5....... ...6789A	//x = ( x | (x << 10) ) & 0x000F801F; \n"
"	\n"
"	//.......1 23451234 5.....67 89A6789A	//x |= (x <<  5)\n"
"	//.......1 1.....11 1.....11 .....111	//0x 01 83 83 07\n"
"	//.......1 2.....34 5.....67 .....89A	//x = ( x | (x <<  5) ) & 0x01838307;\n"
"	\n"
"	//....12.1 2..34534 5..67.67 ..89A89A	//x |= (x <<  3)\n"
"	//....1... 1..1...1 1..1...1 ..1...11	//0x 08 91 91 23\n"
"	//....1... 2..3...4 5..6...7 ..8...9A	//x = ( x | (x <<  3) ) & 0x08919123;\n"
"	\n"
"	//...11..2 2.33..4N 5.66..77 .88..9NA	//x |= (x <<  1)	( N indicates overlapping bits, first overlap is bit {4,5} second is {9,A} )\n"
"	//....1..1 ..1...1. 1..1..1. .1...1.1	//0x 09 22 92 45\n"
"	//....1..2 ..3...4. 5..6..7. .8...9.A	//x = ( x | (x <<  1) ) & 0x09229245;\n"
"	\n"
"	//...11.22 .33..445 5.66.77. 88..99AA	//x |= (x <<  1)\n"
"	//....1..1 ..1..1.. 1..1..1. .1..1..1	//0x 09 34 92 29\n"
"	//....1..2 ..3..4.. 5..6..7. .8..9..A	//x = ( x | (x <<  1) ) & 0x09349229;\n"
"	\n"
"	//........ ........ ......11 11111111	//0x000003FF\n"
"	x &= 0x000003FF;		//Clear all bits above bit 10\n"
"	\n"
"	x = ( x | (x << 10) ) & 0x000F801F;\n"
"	x = ( x | (x <<  5) ) & 0x01838307;\n"
"	x = ( x | (x <<  3) ) & 0x08919123;\n"
"	x = ( x | (x <<  1) ) & 0x09229245;\n"
"	x = ( x | (x <<  1) ) & 0x09349229;\n"
"	\n"
"	return x;\n"
"}\n"
"unsigned int getMortonCode(unsigned int x, unsigned int y, unsigned int z)\n"
"{\n"
"	return interleaveBits(x) << 0 | interleaveBits(y) << 1 | interleaveBits(z) << 2;\n"
"}\n"
"//Should replace with an optimized parallel reduction\n"
"__kernel void findAllNodesMergedAabb(__global b3AabbCL* out_mergedAabb, int numAabbsNeedingMerge)\n"
"{\n"
"	//Each time this kernel is added to the command queue, \n"
"	//the number of AABBs needing to be merged is halved\n"
"	//\n"
"	//Example with 159 AABBs:\n"
"	//	numRemainingAabbs == 159 / 2 + 159 % 2 == 80\n"
"	//	numMergedAabbs == 159 - 80 == 79\n"
"	//So, indices [0, 78] are merged with [0 + 80, 78 + 80]\n"
"	\n"
"	int numRemainingAabbs = numAabbsNeedingMerge / 2 + numAabbsNeedingMerge % 2;\n"
"	int numMergedAabbs = numAabbsNeedingMerge - numRemainingAabbs;\n"
"	\n"
"	int aabbIndex = get_global_id(0);\n"
"	if(aabbIndex >= numMergedAabbs) return;\n"
"	\n"
"	int otherAabbIndex = aabbIndex + numRemainingAabbs;\n"
"	\n"
"	b3AabbCL aabb = out_mergedAabb[aabbIndex];\n"
"	b3AabbCL otherAabb = out_mergedAabb[otherAabbIndex];\n"
"		\n"
"	b3AabbCL mergedAabb;\n"
"	mergedAabb.m_min = b3Min(aabb.m_min, otherAabb.m_min);\n"
"	mergedAabb.m_max = b3Max(aabb.m_max, otherAabb.m_max);\n"
"	out_mergedAabb[aabbIndex] = mergedAabb;\n"
"}\n"
"__kernel void assignMortonCodesAndAabbIndicies(__global b3AabbCL* worldSpaceAabbs, __global b3AabbCL* mergedAabbOfAllNodes, \n"
"												__global SortDataCL* out_mortonCodesAndAabbIndices, int numAabbs)\n"
"{\n"
"	int leafNodeIndex = get_global_id(0);	//Leaf node index == AABB index\n"
"	if(leafNodeIndex >= numAabbs) return;\n"
"	\n"
"	b3AabbCL mergedAabb = mergedAabbOfAllNodes[0];\n"
"	b3Vector3 gridCenter = (mergedAabb.m_min + mergedAabb.m_max) * 0.5f;\n"
"	b3Vector3 gridCellSize = (mergedAabb.m_max - mergedAabb.m_min) / (float)1024;\n"
"	\n"
"	b3AabbCL aabb = worldSpaceAabbs[leafNodeIndex];\n"
"	b3Vector3 aabbCenter = (aabb.m_min + aabb.m_max) * 0.5f;\n"
"	b3Vector3 aabbCenterRelativeToGrid = aabbCenter - gridCenter;\n"
"	\n"
"	//Quantize into integer coordinates\n"
"	//floor() is needed to prevent the center cell, at (0,0,0) from being twice the size\n"
"	b3Vector3 gridPosition = aabbCenterRelativeToGrid / gridCellSize;\n"
"	\n"
"	int4 discretePosition;\n"
"	discretePosition.x = (int)( (gridPosition.x >= 0.0f) ? gridPosition.x : floor(gridPosition.x) );\n"
"	discretePosition.y = (int)( (gridPosition.y >= 0.0f) ? gridPosition.y : floor(gridPosition.y) );\n"
"	discretePosition.z = (int)( (gridPosition.z >= 0.0f) ? gridPosition.z : floor(gridPosition.z) );\n"
"	\n"
"	//Clamp coordinates into [-512, 511], then convert range from [-512, 511] to [0, 1023]\n"
"	discretePosition = b3Max( -512, b3Min(discretePosition, 511) );\n"
"	discretePosition += 512;\n"
"	\n"
"	//Interleave bits(assign a morton code, also known as a z-curve)\n"
"	unsigned int mortonCode = getMortonCode(discretePosition.x, discretePosition.y, discretePosition.z);\n"
"	\n"
"	//\n"
"	SortDataCL mortonCodeIndexPair;\n"
"	mortonCodeIndexPair.m_key = mortonCode;\n"
"	mortonCodeIndexPair.m_value = leafNodeIndex;\n"
"	\n"
"	out_mortonCodesAndAabbIndices[leafNodeIndex] = mortonCodeIndexPair;\n"
"}\n"
"#define B3_PLVBH_TRAVERSE_MAX_STACK_SIZE 128\n"
"#define B3_PLBVH_ROOT_NODE_MARKER -1	//Used to indicate that the (root) node has no parent \n"
"#define B3_PLBVH_ROOT_NODE_INDEX 0\n"
"//For elements of internalNodeChildIndices(int2), the negative bit determines whether it is a leaf or internal node.\n"
"//Positive index == leaf node, while negative index == internal node (remove negative sign to get index).\n"
"//\n"
"//Since the root internal node is at index 0, no internal nodes should reference it as a child,\n"
"//and so index 0 is always used to indicate a leaf node.\n"
"int isLeafNode(int index) { return (index >= 0); }\n"
"int getIndexWithInternalNodeMarkerRemoved(int index) { return (index >= 0) ? index : -index; }\n"
"int getIndexWithInternalNodeMarkerSet(int isLeaf, int index) { return (isLeaf) ? index : -index; }\n"
"__kernel void constructBinaryTree(__global int* firstIndexOffsetPerLevel,\n"
"									__global int* numNodesPerLevel,\n"
"									__global int2* out_internalNodeChildIndices, \n"
"									__global int* out_internalNodeParentNodes, \n"
"									__global int* out_leafNodeParentNodes, \n"
"									int numLevels, int numInternalNodes)\n"
"{\n"
"	int internalNodeIndex = get_global_id(0);\n"
"	if(internalNodeIndex >= numInternalNodes) return;\n"
"	\n"
"	//Find the level that this node is in, using linear search(could replace with binary search)\n"
"	int level = 0;\n"
"	int numInternalLevels = numLevels - 1;		//All levels except the last are internal nodes\n"
"	for(; level < numInternalLevels; ++level)\n"
"	{\n"
"		if( firstIndexOffsetPerLevel[level] <= internalNodeIndex && internalNodeIndex < firstIndexOffsetPerLevel[level + 1]) break;\n"
"	}\n"
"	\n"
"	//Check lower levels to find child nodes\n"
"	//Left child is always in the next level, but the same does not apply to the right child\n"
"	int indexInLevel = internalNodeIndex - firstIndexOffsetPerLevel[level];\n"
"	int firstIndexInNextLevel = firstIndexOffsetPerLevel[level + 1];	//Should never be out of bounds(see for loop above)\n"
"	\n"
"	int leftChildLevel = level + 1;\n"
"	int leftChildIndex = firstIndexInNextLevel + indexInLevel * 2;\n"
"	\n"
"	int rightChildLevel = level + 1;\n"
"	int rightChildIndex = leftChildIndex + 1;\n"
"	\n"
"	//Under certain conditions, the right child index as calculated above is invalid; need to find the correct index\n"
"	//\n"
"	//First condition: must be at least 2 levels apart from the leaf node level;\n"
"	//if the current level is right next to the leaf node level, then the right child\n"
"	//will never be invalid due to the way the nodes are allocated (also avoid a out-of-bounds memory access)\n"
"	//\n"
"	//Second condition: not enough nodes in the next level for each parent to have 2 children, so the right child is invalid\n"
"	//\n"
"	//Third condition: must be the last node in its level\n"
"	if( level < numLevels - 2 \n"
"		&& numNodesPerLevel[level] * 2 > numNodesPerLevel[level + 1] \n"
"		&& indexInLevel == numNodesPerLevel[level] - 1 )\n"
"	{\n"
"		//Check lower levels until we find a node without a parent\n"
"		for(; rightChildLevel < numLevels - 1; ++rightChildLevel)\n"
"		{\n"
"			int rightChildNextLevel = rightChildLevel + 1;\n"
"		\n"
"			//If this branch is taken, it means that the last node in rightChildNextLevel has no parent\n"
"			if( numNodesPerLevel[rightChildLevel] * 2 < numNodesPerLevel[rightChildNextLevel] )\n"
"			{\n"
"				//Set the node to the last node in rightChildNextLevel\n"
"				rightChildLevel = rightChildNextLevel;\n"
"				rightChildIndex = firstIndexOffsetPerLevel[rightChildNextLevel] + numNodesPerLevel[rightChildNextLevel] - 1;\n"
"				break;\n"
"			}\n"
"		}\n"
"	}\n"
"	\n"
"	int isLeftChildLeaf = (leftChildLevel >= numLevels - 1);\n"
"	int isRightChildLeaf = (rightChildLevel >= numLevels - 1);\n"
"	\n"
"	//If left/right child is a leaf node, the index needs to be corrected\n"
"	//the way the index is calculated assumes that the leaf and internal nodes are in a contiguous array,\n"
"	//with leaf nodes at the end of the array; in actuality, the leaf and internal nodes are in separate arrays\n"
"	{\n"
"		int leafNodeLevel = numLevels - 1;\n"
"		leftChildIndex = (isLeftChildLeaf) ? leftChildIndex - firstIndexOffsetPerLevel[leafNodeLevel] : leftChildIndex;\n"
"		rightChildIndex = (isRightChildLeaf) ? rightChildIndex - firstIndexOffsetPerLevel[leafNodeLevel] : rightChildIndex;\n"
"	}\n"
"	\n"
"	//Set the negative sign bit if the node is internal\n"
"	int2 childIndices;\n"
"	childIndices.x = getIndexWithInternalNodeMarkerSet(isLeftChildLeaf, leftChildIndex);\n"
"	childIndices.y = getIndexWithInternalNodeMarkerSet(isRightChildLeaf, rightChildIndex);\n"
"	out_internalNodeChildIndices[internalNodeIndex] = childIndices;\n"
"	\n"
"	//Assign parent node index to children\n"
"	__global int* out_leftChildParentNodeIndices = (isLeftChildLeaf) ? out_leafNodeParentNodes : out_internalNodeParentNodes;\n"
"	out_leftChildParentNodeIndices[leftChildIndex] = internalNodeIndex;\n"
"	\n"
"	__global int* out_rightChildParentNodeIndices = (isRightChildLeaf) ? out_leafNodeParentNodes : out_internalNodeParentNodes;\n"
"	out_rightChildParentNodeIndices[rightChildIndex] = internalNodeIndex;\n"
"}\n"
"__kernel void determineInternalNodeAabbs(__global int* firstIndexOffsetPerLevel,\n"
"										__global int* numNodesPerLevel, \n"
"										__global int2* internalNodeChildIndices,\n"
"										__global SortDataCL* mortonCodesAndAabbIndices,\n"
"										__global b3AabbCL* leafNodeAabbs, \n"
"										__global int2* out_internalNodeLeafIndexRanges,\n"
"										__global b3AabbCL* out_internalNodeAabbs, \n"
"										int numLevels, int numInternalNodes, int level)\n"
"{\n"
"	int i = get_global_id(0);\n"
"	if(i >= numInternalNodes) return;\n"
"	\n"
"	//For each node in a level, check its child nodes to determine its AABB\n"
"	{\n"
"		int indexInLevel = i;	//Index relative to firstIndexOffsetPerLevel[level]\n"
"		\n"
"		int numNodesInLevel = numNodesPerLevel[level];\n"
"		if(indexInLevel < numNodesInLevel)\n"
"		{\n"
"			int internalNodeIndexGlobal = indexInLevel + firstIndexOffsetPerLevel[level];\n"
"			int2 childIndicies = internalNodeChildIndices[internalNodeIndexGlobal];\n"
"			\n"
"			int leftChildIndex = getIndexWithInternalNodeMarkerRemoved(childIndicies.x);\n"
"			int rightChildIndex = getIndexWithInternalNodeMarkerRemoved(childIndicies.y);\n"
"		\n"
"			int isLeftChildLeaf = isLeafNode(childIndicies.x);\n"
"			int isRightChildLeaf = isLeafNode(childIndicies.y);\n"
"			\n"
"			//left/RightChildLeafIndex == Rigid body indicies\n"
"			int leftChildLeafIndex = (isLeftChildLeaf) ? mortonCodesAndAabbIndices[leftChildIndex].m_value : -1;\n"
"			int rightChildLeafIndex = (isRightChildLeaf) ? mortonCodesAndAabbIndices[rightChildIndex].m_value : -1;\n"
"			\n"
"			b3AabbCL leftChildAabb = (isLeftChildLeaf) ? leafNodeAabbs[leftChildLeafIndex] : out_internalNodeAabbs[leftChildIndex];\n"
"			b3AabbCL rightChildAabb = (isRightChildLeaf) ? leafNodeAabbs[rightChildLeafIndex] : out_internalNodeAabbs[rightChildIndex];\n"
"			\n"
"			//\n"
"			b3AabbCL internalNodeAabb;\n"
"			internalNodeAabb.m_min = b3Min(leftChildAabb.m_min, rightChildAabb.m_min);\n"
"			internalNodeAabb.m_max = b3Max(leftChildAabb.m_max, rightChildAabb.m_max);\n"
"			out_internalNodeAabbs[internalNodeIndexGlobal] = internalNodeAabb;\n"
"			\n"
"			//For index range, x == min and y == max; left child always has lower index\n"
"			int2 leafIndexRange;\n"
"			leafIndexRange.x = (isLeftChildLeaf) ? leftChildIndex : out_internalNodeLeafIndexRanges[leftChildIndex].x;\n"
"			leafIndexRange.y = (isRightChildLeaf) ? rightChildIndex : out_internalNodeLeafIndexRanges[rightChildIndex].y;\n"
"			\n"
"			out_internalNodeLeafIndexRanges[internalNodeIndexGlobal] = leafIndexRange;\n"
"		}\n"
"	}\n"
"}\n"
"//From sap.cl\n"
"#define NEW_PAIR_MARKER -1\n"
"bool TestAabbAgainstAabb2(const b3AabbCL* aabb1, const b3AabbCL* aabb2)\n"
"{\n"
"	bool overlap = true;\n"
"	overlap = (aabb1->m_min.x > aabb2->m_max.x || aabb1->m_max.x < aabb2->m_min.x) ? false : overlap;\n"
"	overlap = (aabb1->m_min.z > aabb2->m_max.z || aabb1->m_max.z < aabb2->m_min.z) ? false : overlap;\n"
"	overlap = (aabb1->m_min.y > aabb2->m_max.y || aabb1->m_max.y < aabb2->m_min.y) ? false : overlap;\n"
"	return overlap;\n"
"}\n"
"//From sap.cl\n"
"__kernel void plbvhCalculateOverlappingPairs(__global b3AabbCL* rigidAabbs, \n"
"											__global int2* internalNodeChildIndices, \n"
"											__global b3AabbCL* internalNodeAabbs,\n"
"											__global int2* internalNodeLeafIndexRanges,\n"
"											__global SortDataCL* mortonCodesAndAabbIndices,\n"
"											__global int* out_numPairs, __global int4* out_overlappingPairs, \n"
"											int maxPairs, int numQueryAabbs)\n"
"{\n"
"#define USE_SPATIALLY_COHERENT_INDICIES		//mortonCodesAndAabbIndices[] contains rigid body indices sorted along the z-curve\n"
"#ifdef USE_SPATIALLY_COHERENT_INDICIES\n"
"	int queryRigidIndex = get_group_id(0) * get_local_size(0) + get_local_id(0);\n"
"	if(queryRigidIndex >= numQueryAabbs) return;\n"
"	\n"
"	int queryBvhNodeIndex = queryRigidIndex;\n"
"	queryRigidIndex = mortonCodesAndAabbIndices[queryRigidIndex].m_value;		//	fix queryRigidIndex naming for this branch\n"
"#else\n"
"	int queryRigidIndex = get_global_id(0);\n"
"	if(queryRigidIndex >= numQueryAabbs) return;\n"
"#endif\n"
"	b3AabbCL queryAabb = rigidAabbs[queryRigidIndex];\n"
"	\n"
"	int stack[B3_PLVBH_TRAVERSE_MAX_STACK_SIZE];\n"
"	\n"
"	//Starting by placing only the root node index, 0, in the stack causes it to be detected as a leaf node(see isLeafNode() in loop)\n"
"	int stackSize = 2;\n"
"	stack[0] = internalNodeChildIndices[B3_PLBVH_ROOT_NODE_INDEX].x;\n"
"	stack[1] = internalNodeChildIndices[B3_PLBVH_ROOT_NODE_INDEX].y;\n"
"	\n"
"	while(stackSize)\n"
"	{\n"
"		int internalOrLeafNodeIndex = stack[ stackSize - 1 ];\n"
"		--stackSize;\n"
"		\n"
"		int isLeaf = isLeafNode(internalOrLeafNodeIndex);	//Internal node if false\n"
"		int bvhNodeIndex = getIndexWithInternalNodeMarkerRemoved(internalOrLeafNodeIndex);\n"
"		\n"
"		//Optimization - if the node is not a leaf, check whether the highest leaf index of that node\n"
"		//is less than the queried node's index to avoid testing each pair twice.\n"
"		{\n"
"			//	fix: produces duplicate pairs\n"
"		//	int highestLeafIndex = (isLeaf) ? numQueryAabbs : internalNodeLeafIndexRanges[bvhNodeIndex].y;\n"
"		//	if(highestLeafIndex < queryBvhNodeIndex) continue;\n"
"		}\n"
"		\n"
"		//bvhRigidIndex is not used if internal node\n"
"		int bvhRigidIndex = (isLeaf) ? mortonCodesAndAabbIndices[bvhNodeIndex].m_value : -1;\n"
"	\n"
"		b3AabbCL bvhNodeAabb = (isLeaf) ? rigidAabbs[bvhRigidIndex] : internalNodeAabbs[bvhNodeIndex];\n"
"		if( queryRigidIndex != bvhRigidIndex && TestAabbAgainstAabb2(&queryAabb, &bvhNodeAabb) )\n"
"		{\n"
"			if(isLeaf && rigidAabbs[queryRigidIndex].m_minIndices[3] < rigidAabbs[bvhRigidIndex].m_minIndices[3])\n"
"			{\n"
"				int4 pair;\n"
"				pair.x = rigidAabbs[queryRigidIndex].m_minIndices[3];\n"
"				pair.y = rigidAabbs[bvhRigidIndex].m_minIndices[3];\n"
"				pair.z = NEW_PAIR_MARKER;\n"
"				pair.w = NEW_PAIR_MARKER;\n"
"				\n"
"				int pairIndex = atomic_inc(out_numPairs);\n"
"				if(pairIndex < maxPairs) out_overlappingPairs[pairIndex] = pair;\n"
"			}\n"
"			\n"
"			if(!isLeaf)	//Internal node\n"
"			{\n"
"				if(stackSize + 2 > B3_PLVBH_TRAVERSE_MAX_STACK_SIZE)\n"
"				{\n"
"					//Error\n"
"				}\n"
"				else\n"
"				{\n"
"					stack[ stackSize++ ] = internalNodeChildIndices[bvhNodeIndex].x;\n"
"					stack[ stackSize++ ] = internalNodeChildIndices[bvhNodeIndex].y;\n"
"				}\n"
"			}\n"
"		}\n"
"		\n"
"	}\n"
"}\n"
;
