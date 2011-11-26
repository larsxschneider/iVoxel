/*
 *  Copyright (c) 2011, Lars Schneider
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  Redistributions of source code must retain the above copyright notice, this
 *  list of conditions and the following disclaimer.
 *  Redistributions in binary form must reproduce the above copyright notice,
 *  this list of conditions and the following disclaimer in the documentation
 *  and/or other materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "CrossPlatformHelper.h"

#include "Octree.h"
#include "DebugConfig.h"
#include "MiniGL.h"
#include <cmath>
#include <limits>
#include "MemoryPool.h"
#include "APIFactory.h"
#include "BackingStore.h"
#include "StaticBackingStore.h"

				
#if USE_MEMORY_POOL
#define OCTREE_MALLOC( pointer, type )	while (!(pointer = (type *)_memoryPool->binAlloc(sizeof(type))))\
											swapLeastRecentlyUsedNodesToBackingStore()
#define OCTREE_FREE( pointer)			_memoryPool->binFree( pointer )
#else
#define OCTREE_MALLOC( pointer, type )	pointer = (type *)malloc(sizeof(type))
// Cast to (void*) is necessary to remove any "const".
// see http://stackoverflow.com/questions/2819535/unable-to-free-const-pointers-in-c
#define OCTREE_FREE( pointer)			free( (void*)pointer )
#endif


namespace WVSClientCommon
{

static const uint8_t		OCTREE_NODE_EDGE_SEGEMENTATION = 8;

// Edge lenght of the world. One voxel has a size of 1^3. Consequently a node has a size of 8^3
// This is done to cope with floating point rounding errors.
// OCTREE_SCALE defines real size of a voxel
static const int32_t		OCTREE_WORLD_EDGE_LENGTH = 8 * (1 << (OCTREE_LEAF_LEVEL + 1));
static const int32_t		OCTREE_WORLD_HALF_EDGE_LENGTH = 4 * (1 << (OCTREE_LEAF_LEVEL + 1));


Octree::Octree(	const uint32_t maxPointsInBuffer,
				const char* const backingStoreFilename,
				const bool isStaticFile,
				const MiniGL::ViewFrustum* const viewFrustum,
				const AppCore* const callbackObject,
				const RenderCallbackMethodT callbackMethod) :
				_maxPointsInBuffer(maxPointsInBuffer),
				_renderViewFrustum(viewFrustum),
				_renderCallbackObject(callbackObject),
				_renderCallbackMethod(callbackMethod)
{
	assert(_maxPointsInBuffer > 0);
	
	_regionOrginSet = new bool[OCTREE_LEAF_LEVEL+1];
	_regionOrigin = new FIXPVECTOR3[OCTREE_LEAF_LEVEL+1];
	_regionOriginFloat = new VECTOR3[OCTREE_LEAF_LEVEL+1];
	_nextFrameRegionMin = new FIXPVECTOR3[OCTREE_LEAF_LEVEL+1];
	_nextFrameRegionMax = new FIXPVECTOR3[OCTREE_LEAF_LEVEL+1];
	
	_insertionNodeCache = new CachedNode[OCTREE_LEAF_LEVEL+1];
	
	// Pre-calculate all radii
	_voxelIncircleRadius = new int32_t[OCTREE_LEAF_LEVEL+1];	// = half of voxel edge length
	_nodeIncircleRadius = new int32_t[OCTREE_LEAF_LEVEL+1];		// = half of node edge length
	_voxelIncircleDiameter = new float_t[OCTREE_LEAF_LEVEL+1];
	_nodeIncircleDiameter = new float_t[OCTREE_LEAF_LEVEL+1];
	_voxelCircumcircleRadius = new float_t[OCTREE_LEAF_LEVEL+1];
	_nodeCircumcircleRadius = new float_t[OCTREE_LEAF_LEVEL+1];
	_distanceLevelThreshold = new double_t[OCTREE_LEAF_LEVEL+1];
#if USE_VOXEL_ACCU
	// Init voxel accu with only one slot
	// Reason: screenHeight/screenWidth is not yet known
	_voxelAccu = new uint16_t[1];
#endif

	// Precalc octree radii
	for (uint8_t i = 0; i <= OCTREE_LEAF_LEVEL; ++i)
	{	
		_voxelIncircleRadius[i] = OCTREE_WORLD_EDGE_LENGTH / (1 << (i + 4)); // TODO: shifting here!
		_nodeIncircleRadius[i] = OCTREE_WORLD_EDGE_LENGTH / (1 << (i + 1));
		
		_voxelIncircleDiameter[i] = _voxelIncircleRadius[i] << 1;
		_nodeIncircleDiameter[i] = _nodeIncircleRadius[i] << 1;
		
		// see http://en.wikipedia.org/wiki/Cube
		_voxelCircumcircleRadius[i] = float_t(double_t(_voxelIncircleRadius[i]) * sqrt(3.0));
		_nodeCircumcircleRadius[i] = float_t(double_t(_nodeIncircleRadius[i]) * sqrt(3.0));
		
		// Initial region is in bottom-back-left of the octree
		_regionOrginSet[i] = false;
		_regionOrigin[i].x = -_nodeIncircleRadius[0];
		_regionOrigin[i].y = -_nodeIncircleRadius[0];
		_regionOrigin[i].z = -_nodeIncircleRadius[0];
		_regionOriginFloat[i].x = -_nodeIncircleRadius[0];
		_regionOriginFloat[i].y = -_nodeIncircleRadius[0];
		_regionOriginFloat[i].z = -_nodeIncircleRadius[0];
		
		// Init distance (depends on screen size therefore is initilized later)
		_distanceLevelThreshold[i] = 0;
	}

#if USE_MEMORY_POOL
	// Initilize memory pool
	_memoryPool = new MemoryPool(2);
	_memoryPool->setupElementBin(0, sizeof(Node), OCTREE_NODE_MEMORY_MB * 1024 * 1024);
	_memoryPool->setupElementBin(1, sizeof(QuantPointBlock), OCTREE_QUANT_POINT_BLOCK_MEMORY_MB * 1024 * 1024);
#endif
	
	// Allocate rootnode. Rootnode must never be in the RLU list.
	// Means prevUsedNode/nextUsedNode is always NULL
	OCTREE_MALLOC(_rootNode, Node);
	_rootNode->reset();
	lockNode(_rootNode);
		
	_leastRecentlyUsedNode = NULL;
	_mostRecentlyUsedNode = NULL;
	
	// Root node is present
	_nodeCount = 1;
	
	// We have no points inserted, yet
	_pointCount = 0;
	
#if USE_BACKING_STORE
	if (isStaticFile)
	{
		_backingStore = new StaticBackingStore(	_memoryPool->getMaximalNumberOfElementsInBin(0),
												(Octree::Node*)_memoryPool->getPayloadPointer(0));
		((StaticBackingStore*)_backingStore)->open(backingStoreFilename, _rootNode);
	}
	else
	{
		_backingStore = new BackingStore();
		_backingStore->init(backingStoreFilename);
	}

	// Fork worker thread that restores nodes from backing store
	for (uint32_t i = 0; i < OCTREE_ASYNC_NODE_RESTORE_RING_BUFFER_LENGTH; ++i)
		_asyncRestoreRingBuffer[i] = NULL;
	_asyncRestoreRingBufferTail = 0;
#endif
}


Octree::~Octree()
{
	// Destroy tree recursive
	OCTREE_FREE(_rootNode);
	
#if USE_MEMORY_POOL
	delete _memoryPool;
#endif

#if USE_BACKING_STORE
	delete _backingStore;
#endif

#if USE_VOXEL_ACCU
	delete[] _voxelAccu;
#endif

	delete[] _insertionNodeCache;
	delete[] _voxelIncircleRadius;
	delete[] _voxelCircumcircleRadius;
	delete[] _voxelIncircleDiameter;
	delete[] _nodeIncircleDiameter;
	delete[] _nodeIncircleRadius;
	delete[] _nodeCircumcircleRadius;
	delete[] _nextFrameRegionMin;
	delete[] _nextFrameRegionMax;
	delete[] _regionOrigin;
	delete[] _regionOriginFloat;
}


void Octree::updateScreenSizeRelatedConstants()
{
#if USE_VOXEL_ACCU
	// Adjust voxel accu according to screen size
	// Voxel accu is 1/8 of screen size
	// Voxel accu is a three dimensional array flat stored in one dimension
	_voxelAccuHeight = _renderViewFrustum->getCamera()->getScreenHeight() >> 3;
	_voxelAccuWidth = _renderViewFrustum->getCamera()->getScreenWidth() >> 3;
	delete[] _voxelAccu;
	_voxelAccu = new uint16_t[_voxelAccuHeight * _voxelAccuWidth * (OCTREE_LEAF_LEVEL + 1) + 1];
#endif

	// Calculate LOD distances based on screen size
	double_t height = _renderViewFrustum->getCamera()->getScreenHeight();
	double_t fov = _renderViewFrustum->getCamera()->getFieldOfViewInDegree();
	for (uint8_t i = 0; i <= OCTREE_LEAF_LEVEL; ++i)
	{	
		_distanceLevelThreshold[i] = pow(double_t(_voxelIncircleDiameter[i]) / tan(DEGREES_TO_RADIANS(fov / (2.0 * height))), 2.0);
	}
}


#if USE_VOXEL_ACCU
void Octree::setVoxelAccuThreshold(const uint16_t threshold)
{
	_voxelAccuThreshold = threshold;
}
#endif

	
void Octree::writeNodeToDisk(FILE* const file, Node* const node, uint32_t* numberOfPointsWrittenToDisk)
{
	// Traverse childs first
	for (uint8_t i = 0; i < 8; ++i)
	{
		if (node->children[i] != NULL)
		{
			// Restore child from backing store if not present
			if (node->isChildInMemory(i) == false)
			{
				restoreNodeFromBackingStore(node, i);
			}
			
			// Traverse child
			writeNodeToDisk(file, node->children[i], numberOfPointsWrittenToDisk);
			
			// Points per node is limited to quantization grid
			assert(node->quantPointCount <= OCTREE_NODE_EDGE_SEGEMENTATION *
											OCTREE_NODE_EDGE_SEGEMENTATION *
											OCTREE_NODE_EDGE_SEGEMENTATION);

			Node* child = node->children[i];
			_backingStore->writeNodeToFile(file, child, &(node->childrenFilePosition[i]));
			node->unsetChildInMemory(i);
			
			*numberOfPointsWrittenToDisk += child->quantPointCount;
			
			if (node != _rootNode) touchNode(node);
			if (_mostRecentlyUsedNode != child) freeNodeFromMemory(child);
		}
	}
}
	

void Octree::saveToDisk(const char* const filename)
{
	for (uint8_t i = 0; i < 8; ++i)
		if ((_rootNode->children[i] != NULL) && (_rootNode->isChildInMemory(i))) touchNode(_rootNode->children[i]);

	// Free some memory
	while (_nodeCount > 32)
	{
		swapLeastRecentlyUsedNodesToBackingStore();
	}

	APIFactory::GetInstance().lock(OCTREE_LOCK);
	
	uint32_t numberOfPointsWrittenToDisk = 0;
	
	// Write point cloud to file
	FILE* pointFile = fopen(filename, "wb");
	
	// Reserve space for the root node at the beginning of the file
	fseek(pointFile, 0, SEEK_SET);
	fwrite(_rootNode->children, sizeof(Octree::Node*), 8, pointFile);
	
	// Write all child nodes
	writeNodeToDisk(pointFile, _rootNode, &numberOfPointsWrittenToDisk);
	
	// Check if all root node children are swapped
	for (uint8_t i = 0; i < 8; ++i) 
		assert(!((_rootNode->children[i] != NULL) && (_rootNode->isChildInMemory(i))));
		
	// Update root node
	fseek(pointFile, 0, SEEK_SET);
	fwrite(_rootNode->children, sizeof(Octree::Node*), 8, pointFile);
	
	fclose(pointFile);
	
	printf("points: %u\n", numberOfPointsWrittenToDisk);
	APIFactory::GetInstance().unlock(OCTREE_LOCK);
	
	_leastRecentlyUsedNode = NULL;
	_mostRecentlyUsedNode = NULL;
	_nodeCount = 1;
}


const float_t* Octree::getRegionOrginArray() const
{
	return &(_regionOriginFloat[0].x);
}


const float_t* Octree::getVoxelIncircleDiameterArray() const
{
	return _voxelIncircleDiameter;
}


const float_t Octree::getVoxelCircumcircleRadius(const uint8_t level) const
{
	assert(level <= OCTREE_LEAF_LEVEL);
	return _voxelCircumcircleRadius[level];
}


const float_t* Octree::getNodeIncircleDiameterArray() const
{
	return _nodeIncircleDiameter;
}


void Octree::touchNode(Node* const node)
{
	assert(node);
	
	// If a locked node is touched, nothing happens
	if (node->prevUsedNode == (Node*)OCTREE_LOCKED_NODE_IDENTIFIER) return;
	
	// Root node is not allowed in LRU
	if (node == _rootNode) return;
	
	// No node is in the queue, yet
	if ((_leastRecentlyUsedNode == NULL) || (_mostRecentlyUsedNode == NULL))
	{
		_leastRecentlyUsedNode = node;
		_mostRecentlyUsedNode = node;
		node->nextUsedNode = NULL;
		node->prevUsedNode = NULL;
		return;
	}

	// One node is the queue
	if ((_leastRecentlyUsedNode == node) && (_mostRecentlyUsedNode == node)) return;
		
	if ((node->prevUsedNode != NULL) && (node->nextUsedNode != NULL))
	{
		// Node is somewhere in the chain, close the gap
		node->prevUsedNode->nextUsedNode = node->nextUsedNode;
		node->nextUsedNode->prevUsedNode = node->prevUsedNode;
	}
	else if ((node->prevUsedNode != NULL) && (node->nextUsedNode == NULL))
	{
		// Node is already most recently used
		assert(_mostRecentlyUsedNode == node);
		return;
	}
	else if ((node->prevUsedNode == NULL) && (node->nextUsedNode != NULL))
	{
		// Node is least recently used Node
		assert(_leastRecentlyUsedNode == node);
		
		// Set new last node
		_leastRecentlyUsedNode = node->nextUsedNode;
		_leastRecentlyUsedNode->prevUsedNode = NULL;
		
		assert(_leastRecentlyUsedNode->nextUsedNode != NULL);
		assert(_leastRecentlyUsedNode->prevUsedNode == NULL);
	}
	
	assert(_mostRecentlyUsedNode != NULL);
	
	// Put node into the front
	_mostRecentlyUsedNode->nextUsedNode = node;	
	node->prevUsedNode = _mostRecentlyUsedNode;
	node->nextUsedNode = NULL;
	_mostRecentlyUsedNode = node;
	
//#ifdef DEBUG
//	checkLRU();
//#endif
		
	assert(_leastRecentlyUsedNode != NULL);
	assert(_mostRecentlyUsedNode != NULL);
	assert(_leastRecentlyUsedNode->nextUsedNode != NULL);
	assert(_leastRecentlyUsedNode->prevUsedNode == NULL);
	assert(_mostRecentlyUsedNode->nextUsedNode == NULL);
	assert(_mostRecentlyUsedNode->prevUsedNode != NULL);
}


// Check LRU chain in both directions
void Octree::checkLRU()
{
	Node* n = _leastRecentlyUsedNode;
	int i = 0;
	while ((n != _mostRecentlyUsedNode) && (i < _nodeCount))
	{
		assert(n->nextUsedNode);
		n = n->nextUsedNode;
		++i;
	}
	
	n = _mostRecentlyUsedNode;
	while ((n != _leastRecentlyUsedNode) && (i > 0))
	{
		assert(n->prevUsedNode);
		n = n->prevUsedNode;
		--i;
	}
	assert(i == 0);
}


void Octree::lockNode(Node* const node)
{
	assert(node);
	
	// Do nothing if the node is already locked
	if (node->prevUsedNode == (Node*)OCTREE_LOCKED_NODE_IDENTIFIER) return;
	
	// Root node cannot be locked
	if (node == _rootNode) return;
	
//	#ifdef DEBUG
//	checkLRU();
//	#endif
	
	// Extract node out of LRU list (if in there)
	if (node->prevUsedNode == NULL)
	{
		assert(node == _leastRecentlyUsedNode);
		_leastRecentlyUsedNode = node->nextUsedNode;
	}
	else
	{
		node->prevUsedNode->nextUsedNode = node->nextUsedNode;
	}
	
	if (node->nextUsedNode == NULL)
	{
		assert(node == _mostRecentlyUsedNode);
		_mostRecentlyUsedNode = node->prevUsedNode;
	}
	else
	{
		node->nextUsedNode->prevUsedNode = node->prevUsedNode;
	}
	
//	#ifdef DEBUG
//	checkLRU();
//	#endif
	
	// Set magic byte sequence to identify locked nodes
	node->prevUsedNode = (Node*)OCTREE_LOCKED_NODE_IDENTIFIER;
	node->nextUsedNode = NULL;
	
//	#ifdef DEBUG
//	checkLRU();
//	#endif
}


void Octree::unlockNode(Node* const node)
{
	// Check if the node is locked
	assert(node->prevUsedNode == (Node*)OCTREE_LOCKED_NODE_IDENTIFIER);
	assert(node->nextUsedNode == NULL);
	
	// Root node cannot be locked
	if (node == _rootNode) return;
	
	// Clear magic byte sequence
	node->prevUsedNode = NULL;
	
	// Put into LRU, again
	touchNode(node);
}


/**
	Frees all memory that is occpuied by a given node (including points associated to that node).
	@param node Node to be freed. This node must not have any child nodes in memory.

 */
void Octree::freeNodeFromMemory(const Node* const node)
{
	// Never free the most recently used node
	assert(_mostRecentlyUsedNode != node);
	assert(node->nextUsedNode != NULL);
	
	// Never free root node
	assert(node != _rootNode);
	
	// Never free a node that has children in memory
	for (int i = 0; i < 8; ++i) assert(!((node->children[i] != NULL) && (node->isChildInMemory(i))));

	// Never free a locked node
	assert(!node->isLocked());
	
	// Remove node from LRU list
	if ((node->prevUsedNode != NULL) && (node->nextUsedNode != NULL))
	{
		// Node is somewhere in the chain, close the gap
		assert(node->prevUsedNode->nextUsedNode);
		assert(node->nextUsedNode->prevUsedNode);
		node->prevUsedNode->nextUsedNode = node->nextUsedNode;
		node->nextUsedNode->prevUsedNode = node->prevUsedNode;
	}
	else if ((node->prevUsedNode == NULL) && (node->nextUsedNode != NULL))
	{
		// Node is least recently used Node
		assert(_leastRecentlyUsedNode == node);
		
		// Set new last node
		_leastRecentlyUsedNode = node->nextUsedNode;
		_leastRecentlyUsedNode->prevUsedNode = NULL;
		
		assert(_leastRecentlyUsedNode->nextUsedNode != NULL);
		assert(_leastRecentlyUsedNode->prevUsedNode == NULL);
	}

	// Remove all data blocks from memory
	QuantPointBlock* block = node->data;
	QuantPointBlock* nextBlock;

	while (block != NULL)
	{
		nextBlock = block->next;
		OCTREE_FREE(block);
				
		// Jump to next block
		block = nextBlock;
	}

	// Remove node from memory
	OCTREE_FREE(node);
}


void Octree::restoreChildNodesFromBackingStore(Node* parentNode)
{
	// Make sure the parent node has no parent being the least recently used node
//	Node* parent = parentNode;
//	int levels = 0;
//	while (parent != _rootNode)
//	{
//		touchNode(parent);
//		parent = parent->parent;
//		levels++;
//		// Make sure we need at most octree_leaf_levels to the root (= circle check)
//		// Should never happen
//		assert(levels <= OCTREE_LEAF_LEVEL);
//	}
	
	// Load all children of the node				
	for (uint8_t j = 0; j < 8; ++j)
	{
		if (!(parentNode->isChildInMemory(j)) && 
			 (parentNode->children[j] != NULL))
		{
			restoreNodeFromBackingStore(parentNode, j);
		}
	}
}


/**
	Reads a node from backing store
	@param parentNode Parent node of the node that is going to be restored.
	@param nodeID Node ID that identifies the child of the parentNode that is going to be restored.
	@returns Returns true if the operation was successful.
 */
bool Octree::restoreNodeFromBackingStore(Node* const parentNode, const uint8_t nodeID)
{
#if USE_BACKING_STORE == 1

	// Make sure the child that is going to be restored is not yet in memory
	assert(parentNode->isChildInMemory(nodeID) == false);
	
	// Child does exist but is located on backing store
	bool success;
	Node* restoredNode;
	
	// Save file position of the node
	long pos = parentNode->childrenFilePosition[nodeID];
	
	// Allocate a new node
	OCTREE_MALLOC(restoredNode, Node);
	restoredNode->reset();
	
	// Restore node from from backing store
	APIFactory::GetInstance().lock(BACKINGSTORE_LOCK);
	
	success = _backingStore->readNode(pos, restoredNode);
	
	// Points per node is limited to quantization grid
	assert(restoredNode->quantPointCount <= OCTREE_NODE_EDGE_SEGEMENTATION *
											OCTREE_NODE_EDGE_SEGEMENTATION *
											OCTREE_NODE_EDGE_SEGEMENTATION);

	APIFactory::GetInstance().unlock(BACKINGSTORE_LOCK);
	
	if (!success)
	{
		logError("Node I/O Error. Read failure.");
		
		// Free node from memory
		freeNodeFromMemory(restoredNode);
		parentNode->children[nodeID] = NULL;
		parentNode->setChildInMemory(nodeID);
		
		return false;
	}
	
	++_nodeCount;
	_pointCount += restoredNode->quantPointCount;

	// Make this not the most recently used node (to avoid swapping in the near future)
	touchNode(restoredNode);
	
	// Connect restored node to parent node
	restoredNode->parent = parentNode;
	
	// Notifiy parent node that child node is in memory
	parentNode->children[nodeID] = restoredNode;
	
	parentNode->setChildInMemory(nodeID);
	
	// Check if the node has quant points, load them
	if (restoredNode->quantPointCount > 0)
	{
		uint32_t blockCount = ((restoredNode->quantPointCount - 1) / OCTREE_POINTS_PER_POINT_DATA_BLOCK) + 1;
		
		assert(blockCount > 0);
		assert(((blockCount-1) * OCTREE_POINTS_PER_POINT_DATA_BLOCK) < restoredNode->quantPointCount);
		assert(restoredNode->quantPointCount <= (blockCount * OCTREE_POINTS_PER_POINT_DATA_BLOCK));
		assert(((blockCount-1) * OCTREE_POINTS_PER_POINT_DATA_BLOCK) + (restoredNode->quantPointCount % OCTREE_POINTS_PER_POINT_DATA_BLOCK == 0 ? OCTREE_POINTS_PER_POINT_DATA_BLOCK : restoredNode->quantPointCount % OCTREE_POINTS_PER_POINT_DATA_BLOCK)== restoredNode->quantPointCount);
		
		// Allocate memory for QuantPoints
		QuantPointBlock* block;
		for (uint32_t i = 0; i < blockCount; ++i)
		{
			if (i == 0)
			{
				OCTREE_MALLOC(restoredNode->data, QuantPointBlock);
				block = restoredNode->data;
			}
			else
			{
				OCTREE_MALLOC(block->next, QuantPointBlock);
				block = block->next;
			}
		}
		
		// Last block has no successor
		// More data initialization is not necessary because readQuantPointBlock will overwrite
		// everything
		block->next = NULL;
		
		APIFactory::GetInstance().lock(BACKINGSTORE_LOCK);
		success = _backingStore->readQuantPointBlock(pos, blockCount, restoredNode->data);
		APIFactory::GetInstance().unlock(BACKINGSTORE_LOCK);
		
		if (!success)
		{
			logError("QuantPointBlock I/O Error. Read failure.");
			
			freeNodeFromMemory(restoredNode);
			parentNode->children[nodeID] = NULL;
			parentNode->setChildInMemory(nodeID);
			
			return false;
		}
	}
	
	return true;

#else
	parentNode->children[nodeID] = NULL;
	parentNode->setChildInMemory(nodeID);
		
	return false;
#endif
}


/**
	Checks if a node has children. If this is the case the function will call itself recursivly
	on the children. If no children are present the node including all QuantPointBlocks is written
	to backing store. The pointer of the parent to this node is changed to the backing store
	location and the "backing-store-bit" is set.
	
	@param node Node that is going to to be swapped to backing store.
	@param parentNodeID ID that determines the place of Node in parent node.
 */
void Octree::swapNodeToBackingStore(Node* node, int parentNodeID)
{
	// Never swap the most recently used node
	assert(_mostRecentlyUsedNode != node);
	assert(node->nextUsedNode != NULL);
	
	// Never swap the root node
	assert(node != _rootNode);
	
	// Never swap a locked node
	assert(!node->isLocked());

	// Never swap a node that has children in memory
	for (int i = 0; i < 8; ++i) assert(!((node->children[i] != NULL) && (node->isChildInMemory(i))));
	
	// Write node and node data to backing store and store location
#if USE_BACKING_STORE
	bool success;
	APIFactory::GetInstance().lock(BACKINGSTORE_LOCK);
	
	// Points per node is limited to quantization grid
	assert(node->quantPointCount <= OCTREE_NODE_EDGE_SEGEMENTATION *
									OCTREE_NODE_EDGE_SEGEMENTATION *
									OCTREE_NODE_EDGE_SEGEMENTATION);

	success = _backingStore->writeNode(node, &(node->parent->childrenFilePosition[parentNodeID]));
	APIFactory::GetInstance().unlock(BACKINGSTORE_LOCK);
	if (!success) logError("Node I/O Error. Write failure.");

	node->parent->unsetChildInMemory(parentNodeID);
#else
	node->parent->children[parentNodeID] = NULL;
#endif
	freeNodeFromMemory(node);
	
	--_nodeCount;
	_pointCount -= node->quantPointCount;
}


void Octree::swapLeastRecentlyUsedNodesToBackingStore()
{
	// The least recent node (node to be swapped)
	assert(_leastRecentlyUsedNode != NULL);
	assert(_leastRecentlyUsedNode->parent != NULL);
	assert(_leastRecentlyUsedNode->nextUsedNode != NULL);
	assert(_leastRecentlyUsedNode->prevUsedNode == NULL);
	assert(_leastRecentlyUsedNode != _rootNode);

	// Find first leaf node of the least recently used node
	uint8_t i = 0;
	Node* node = _leastRecentlyUsedNode;
	while (i < 8)
	{
		if ((node->children[i] != NULL) && (node->isChildInMemory(i)))
		{
			node = node->children[i];
			i = 0;
		}
		else
		{
			++i;
		}
	}
	
	// Find parentNodeID of the node
	uint8_t parentNodeID = 0;
	while (	(node->parent->children[parentNodeID] != node) && 
			(parentNodeID < 8))
			++parentNodeID;
	assert(parentNodeID < 8);

	swapNodeToBackingStore(node, parentNodeID);
}


const QuantPoint::PositionNormal Octree::quantizeRelativePosition(
	const FIXPVECTOR3* const pointPosition,
	const FIXPVECTOR3* const referenceCenter,
	const int8_t level) const
{
	// Calc all dimensions relative to node center
	const int32_t relativePositionX = pointPosition->x - referenceCenter->x;
	const int32_t relativePositionY = pointPosition->y - referenceCenter->y;
	const int32_t relativePositionZ = pointPosition->z - referenceCenter->z;
	
	// Calc all dimensions relativ to node center in "segmentation metric"
	const uint8_t x = (relativePositionX + _nodeIncircleRadius[level]) >> (OCTREE_LEAF_LEVEL - level + 1);
	const uint8_t y = (relativePositionY + _nodeIncircleRadius[level]) >> (OCTREE_LEAF_LEVEL - level + 1);
	const uint8_t z = (relativePositionZ + _nodeIncircleRadius[level]) >> (OCTREE_LEAF_LEVEL - level + 1);

	assert(x < OCTREE_NODE_EDGE_SEGEMENTATION);
	assert(y < OCTREE_NODE_EDGE_SEGEMENTATION);
	assert(z < OCTREE_NODE_EDGE_SEGEMENTATION);

	return ((x << 7)  | 
			(y << 10) | 
			(z << 13));
}


bool Octree::retrieveQuantPointInNode(
	Node* const node,
	const QuantPoint::PositionNormal quantizedPosition,
	QuantPoint** outQuantPoint)
{
	if (node->data == NULL)
	{
		// Node does not have a data block, create one and add first point
		OCTREE_MALLOC(node->data, QuantPointBlock);
		node->data->reset();
		*outQuantPoint = &(node->data->points[0]);
		return false;
	}

// Macros to help unroll the bsearch loop
#define INNER_BSEARCH(smaller, midValue, greater) \
	cmp = quantizedPosition - (*outQuantPoint = base + midValue)->getPosition();\
	if (cmp > 0) {greater}\
	else if (cmp < 0) {smaller}\
	else {return true;}
		
// Note: *outQuantPoint is not set in the cmp>0, cmp<0 branches. Profiling showed slower results
// propably related to branch prediction?!
#define LEAF_BSEARCH(midValue) \
	cmp = quantizedPosition - (*outQuantPoint = base + midValue)->getPosition();\
	if (cmp == 0) {return true;}\
	else if ((cmp > 0) && (quantizedPosition - (base + (midValue + 1))->getPosition() == 0))\
		{*outQuantPoint = base + (midValue + 1); return true;}\
	else if ((cmp < 0) && (quantizedPosition - (base + (midValue - 1))->getPosition() == 0))\
		{*outQuantPoint = base + (midValue - 1); return true;}	
	
	int cmp;
	QuantPoint* base;
	QuantPointBlock* block = node->data;
	uint16_t fullBlockCount = node->quantPointCount / OCTREE_POINTS_PER_POINT_DATA_BLOCK;
	
	// Iterate through all full blocks
	for (uint16_t i = 0; i < fullBlockCount; ++i)
	{
		if (i > 0) block = block->next;
		base = &(block->points[0]);
		
		INNER_BSEARCH(
			INNER_BSEARCH(
				INNER_BSEARCH(LEAF_BSEARCH(1), 3, LEAF_BSEARCH(5)),
				7,
				INNER_BSEARCH(LEAF_BSEARCH(9), 11, LEAF_BSEARCH(13))),
			15,
			INNER_BSEARCH(
				INNER_BSEARCH(LEAF_BSEARCH(17), 19, LEAF_BSEARCH(21)),
				23,
				INNER_BSEARCH(LEAF_BSEARCH(25), 27, LEAF_BSEARCH(29))));
	}

	// Check if the last full block has a successor
	if ((block->next == NULL) && (fullBlockCount > 0))
	{
		// All blocks are full. Add a new block and add new point
		OCTREE_MALLOC(block->next, QuantPointBlock);
		block->next->reset();
		*outQuantPoint = &(block->next->points[0]);
		return false;
	}
	else if (fullBlockCount > 0)
	{
		// Successor of last block is only relevant if there was at least one full block
		block = block->next;
	}

	// Last block is partially filled
	// Search for element
	base = &(block->points[0]);
	uint16_t pointsInLastBlock = node->quantPointCount % OCTREE_POINTS_PER_POINT_DATA_BLOCK;
	
	assert(pointsInLastBlock > 0);
	assert(pointsInLastBlock < 31);
	
	if (pointsInLastBlock < 3)
	{
		cmp = quantizedPosition - (*outQuantPoint = base)->getPosition();
		if (cmp == 0) return true;
		else if ((pointsInLastBlock == 2) && (cmp > 0))
		{
			cmp = quantizedPosition - (*outQuantPoint = base + 1)->getPosition();
			if (cmp == 0) return true;
		}
	}
	else
	{
		// binary search algorithm (equal to c bsearch)
		// from http://www.google.com/codesearch/p?hl=en#qoCVjtE_vOw/gcc4/trunk/gcc-4.4.3/libiberty/bsearch.c&q=bsearch&sa=N&cd=2&ct=rc
		uint16_t lim;
		for (lim = pointsInLastBlock; lim != 0; lim >>= 1)
		{
			*outQuantPoint = base + (lim >> 1);
			cmp = quantizedPosition - (*outQuantPoint)->getPosition();
			if (cmp > 0)
			{
				// quantizedPosition > (*outQuantPoint): move right
				base = (*outQuantPoint) + 1;
				--lim;
			}
			else if (cmp == 0) return true; // We found the QuantPoint
			// else move left
		}
	}
	
	// Check if all points in the block are in the right order
	for (int i = 0; i < pointsInLastBlock-1; ++i)
	{
		assert(block->points[i].getPosition() < block->points[i+1].getPosition());
	}
	
	// Make room for new element
	// Position of the new point depends on the last compare
	// If the last compare resulted in "greater" than the new element if behind the element that
	// was look at last
	*outQuantPoint += (cmp > 0 ? 1 : 0);
	int bytesToMove = (pointsInLastBlock - ((*outQuantPoint) - &(block->points[0]))) * sizeof(QuantPoint);
	memmove((*outQuantPoint) + 1, (*outQuantPoint), bytesToMove);
	
	return false;
}


void Octree::calcCenterOfChildNode(	FIXPVECTOR3* const center, 
									const uint8_t childNodeID,
									const uint8_t level) const
{
	center->x = center->x + (childNodeID & 1 ? 1 : -1) * _nodeIncircleRadius[level];
	center->y = center->y + (childNodeID & 2 ? 1 : -1) * _nodeIncircleRadius[level];
	center->z = center->z + (childNodeID & 4 ? 1 : -1) * _nodeIncircleRadius[level];
}


// Adds a point to the octree
// Attention: OCTREE_LOCK has to be active (we don't want to do locking within the method,
// because this is to much overhead/too fine)
void Octree::addPoint(const WVSPoint* const point)
{
	// Transform floating point vector in fix point format
	const FIXPVECTOR3 position = {	roundf(point->position.x),
									roundf(point->position.y),
									roundf(point->position.z) };
	
	// Check if the point is within the octree bounds
	if (!(	(-OCTREE_WORLD_HALF_EDGE_LENGTH < position.x) && 
			(position.x < OCTREE_WORLD_HALF_EDGE_LENGTH) &&
			(-OCTREE_WORLD_HALF_EDGE_LENGTH < position.y) &&
			(position.y < OCTREE_WORLD_HALF_EDGE_LENGTH) &&
			(-OCTREE_WORLD_HALF_EDGE_LENGTH < position.z) &&
			(position.z < OCTREE_WORLD_HALF_EDGE_LENGTH)))
	{
		logError("Adding point failed. Reason: Position out of octree bounds.\n");
		return;
	}
	
	// Quantize color to 15 bpp
	const uint16_t fiveBitColor =	((point->color.red >> 3) |	
									((point->color.green >> 3) << 5) |
									((point->color.blue >> 3) << 10));

	// Convert radius to int
	const uint32_t radius = roundf(point->radius);

	// Start on root level
	Node *node = _rootNode;
	FIXPVECTOR3 center = {0, 0, 0};
	
	uint8_t level = 0;
	while ((radius < _voxelIncircleRadius[level]) && (level < OCTREE_LEAF_LEVEL))
	{
		// Go one level deeper
		++level;
		
		// Determine appropriate child node for point
		uint8_t cellID = 0;
		if (center.x <= position.x) cellID |= 1;
		if (center.y <= position.y) cellID |= 2;
		if (center.z <= position.z) cellID |= 4;
		
		if ((node->isChildInMemory(cellID)) && (node->children[cellID] == NULL))
		{
			// Child node does not exist
			OCTREE_MALLOC(node->children[cellID], Node);
			node->children[cellID]->reset();
			node->children[cellID]->parent = node;
			
			// New node created
			_nodeCount++;
		}
		else if (!node->isChildInMemory(cellID))
		{
			// Child does exist but is located on backing store
			if (!restoreNodeFromBackingStore(node, cellID))
			{
				APIFactory::GetInstance().unlock(OCTREE_LOCK);
				logError("Adding point failed. Reason: Backing store node restore failed.\n");
				return;
			}
		}
		
		// Calculuate center of the next level
		calcCenterOfChildNode(&center, cellID, level);
		
		// From that point the node has to be in memory
		assert(node->isChildInMemory(cellID));
		node = node->children[cellID];
		assert(node->parent);
		assert(center.x != 0);
		assert(center.y != 0);
		assert(center.z != 0);
		touchNode(node);
		
		_insertionNodeCache[level].node = node;
		_insertionNodeCache[level].center = center;
	}
	
	
	// Iterate over all LODs and insert the point
	// Start from finest LOD to coarest LOD
	// Native bit is set for the finest level
	uint16_t nativeBit = 1 << 15;
	QuantPoint* quantPoint;

	while (level > 0)
	{
		// Calculate the position within the node
		QuantPoint::PositionNormal qPos = quantizeRelativePosition(
			&(position),
			&(_insertionNodeCache[level].center),
			level);

		// Get the memory location of that position
		if (retrieveQuantPointInNode(_insertionNodeCache[level].node, qPos, &quantPoint))
		{
			// Check if a native point is on your path to the root node			
			// If yes, we can exit early
			if ((quantPoint->isNative() == true) && (nativeBit == 0)) return;
			
			// No native point on the path. Thus we set the new normal and mix the colors
			// Attention: We also mix if the point in the octree is native and the point to be
			// inserted is native --> looks better.
		
			// Set normal of the new point
			quantPoint->positionNormal = qPos | point->normalIndex;
			
			// Merge colors
			static const uint16_t redMask = 31; 
			static const uint16_t greenMask = 31 << 5;
			static const uint16_t blueMask = 31 << 10;
			
			const uint16_t red = (((quantPoint->colorNative & redMask) + 
								  (fiveBitColor & redMask)) >> 1) & redMask;
			const uint16_t green = (((quantPoint->colorNative & greenMask) + 
									(fiveBitColor & greenMask)) >> 1) & greenMask;
			const uint16_t blue = (((quantPoint->colorNative & blueMask) + 
								   (fiveBitColor & blueMask)) >> 1) & blueMask;
			
			quantPoint->colorNative = red | green | blue | nativeBit;
		}
		else
		{
			// Position is not yet occupied in node
			// Set position, normal
			quantPoint->positionNormal = qPos | point->normalIndex;
			// Set color and native bit
			quantPoint->colorNative = fiveBitColor | nativeBit;
				
			// Increase quant point count in node
			++(_insertionNodeCache[level].node->quantPointCount);
			
			// Increase octree pointcount
			++_pointCount;
		}

		// Go to next level
		--level;
		
		// All lower levels provide no native point
		nativeBit = 0;
	}
}


void Octree::estimateInteractiveRenderQuality(float_t* const targetRenderQuality, const uint32_t maxPointCount)
{
	uint32_t counter, lastTryCount;
	_voxelCount = &counter;
	
	*_voxelCount = 0;
	_renderQuality = 0.0f;
	FIXPVECTOR3 rootCenter = {0, 0, 0};

	APIFactory::GetInstance().lock(OCTREE_LOCK);
	do
	{
		_renderQuality += RENDER_QUALITY_COARSE_ADJUSTMENT;
		lastTryCount = *_voxelCount;
		
		countRenderBufferPoints(
			_rootNode,
			&rootCenter,
			0,
			maxPointCount,
			MiniGL::ViewFrustum::INTERSECT);
	}
	while (	(*_voxelCount < maxPointCount) && 
			(lastTryCount < *_voxelCount));
	APIFactory::GetInstance().unlock(OCTREE_LOCK);
	
	*targetRenderQuality = _renderQuality;
}


// Count roughly points for render buffer
void Octree::countRenderBufferPoints(
	Node* const node,
	const FIXPVECTOR3* const nodeCenter,
	const uint8_t level,
	const uint32_t maxPointCount,
	MiniGL::ViewFrustum::Relation nodeViewFrustumRelation)
{
	// Early exit if we reached maxPoint Count
	if (*_voxelCount > maxPointCount) return;

	const VECTOR3 floatingNodeCenter = {nodeCenter->x, nodeCenter->y, nodeCenter->z};

	// If it is not known if the node is inside or outside the view frustum, check it
	if (nodeViewFrustumRelation == MiniGL::ViewFrustum::INTERSECT)
	{
		nodeViewFrustumRelation = _renderViewFrustum->testAgainstSphere(
			&floatingNodeCenter,
			_nodeCircumcircleRadius[level]);
	}
	
	// If the node is outside the view frustum, ignore it
	if (nodeViewFrustumRelation == MiniGL::ViewFrustum::OUT) return;
	
	if ((level == OCTREE_LEAF_LEVEL) ||
		(_renderViewFrustum->squaredDistanceToCamera(&floatingNodeCenter) > 
			_distanceLevelThreshold[level] * _renderQuality))
	{
		// Increase voxel count by number of voxels in the current node
		*_voxelCount = (*_voxelCount + node->quantPointCount);

	}
	else
	{
		Node* childNode;
		FIXPVECTOR3 childCenter;
		
		for (uint8_t i = 0; i < 8; ++i)
		{
			childNode = node->children[i];
			if ((node->children[i] != NULL) && (node->isChildInMemory(i)))
			{
				// Calc child node center
				childCenter.x = nodeCenter->x;
				childCenter.y = nodeCenter->y;
				childCenter.z = nodeCenter->z;
				calcCenterOfChildNode(&childCenter, i, level+1);
				
				countRenderBufferPoints(
						childNode,
						&childCenter,
						level+1,
						maxPointCount,
						nodeViewFrustumRelation);
			}
		}
	}
}



// Method is called from the OpenGL thread
void Octree::copyPointsToBuffer(
	Node* const node,
	const FIXPVECTOR3* const nodeCenter,
	const uint8_t level,
	MiniGL::ViewFrustum::Relation nodeViewFrustumRelation)
{
	assert(node);
	
	// The current node must either be INTERSECT or IN at this point
	assert(	(nodeViewFrustumRelation == MiniGL::ViewFrustum::INTERSECT) ||
			(nodeViewFrustumRelation == MiniGL::ViewFrustum::IN));
	assert(nodeViewFrustumRelation != MiniGL::ViewFrustum::OUT);
	
	// Early exit if rendering was canceled
	if (*_bufferFlags & OCTREE_RENDERING_CANCELED) return;
	
	const VECTOR3 floatingNodeCenter = {nodeCenter->x, nodeCenter->y, nodeCenter->z};

	// If it is not known if the node is inside or outside the view frustum, check it
	if (nodeViewFrustumRelation == MiniGL::ViewFrustum::INTERSECT)
	{
		nodeViewFrustumRelation = _renderViewFrustum->testAgainstSphere(
			&floatingNodeCenter,
			_nodeCircumcircleRadius[level]);
	}
	
	// If the node is outside the view frustum, ignore it
	if (nodeViewFrustumRelation == MiniGL::ViewFrustum::OUT) return;
	
	// Update next region boundaries if necessary
	if (nodeCenter->x < _nextFrameRegionMin[level].x) _nextFrameRegionMin[level].x = nodeCenter->x;
	if (nodeCenter->y < _nextFrameRegionMin[level].y) _nextFrameRegionMin[level].y = nodeCenter->y;
	if (nodeCenter->z < _nextFrameRegionMin[level].z) _nextFrameRegionMin[level].z = nodeCenter->z;

	if (nodeCenter->x > _nextFrameRegionMax[level].x) _nextFrameRegionMax[level].x = nodeCenter->x;
	if (nodeCenter->y > _nextFrameRegionMax[level].y) _nextFrameRegionMax[level].y = nodeCenter->y;
	if (nodeCenter->z > _nextFrameRegionMax[level].z) _nextFrameRegionMax[level].z = nodeCenter->z;
	
	bool copyVoxelsToGPU = false;

	#if BENCHMARK_1_MILLION
	if (*_voxelCount >= 1000000) return;
	#endif
	
	if (level == OCTREE_LEAF_LEVEL)
	{
		// Try to draw everything if we are at leaf level
		copyVoxelsToGPU = true;
	}
	else
	{
		// We are not at leaf level. Check the distance and draw based on distance (=splat size)
		// and desired render quality
		if (_renderViewFrustum->squaredDistanceToCamera(&floatingNodeCenter) > 
			_distanceLevelThreshold[level] * _renderQuality)
		{
			copyVoxelsToGPU = true;
		}
	}
	
	if (copyVoxelsToGPU == false)
	{
		// We are not at leaf level and according to the distance we should not draw this level
		// Check if we have really more detailed voxels in the next deeper level
		uint16_t finerLODVoxelCount = 0;
		for (uint8_t i = 0; i < 8; ++i)
		{
			if (node->children[i] != NULL)
			{
				if (node->isChildInMemory(i))
				{
					// We found a child.
					finerLODVoxelCount += node->children[i]->quantPointCount;
				}
				else if (_instantNodeRestore == true)
				{
					// Check user interaction after all 32 node restores
					if (_instantNodeRestoreCount > 32)
					{
						bool isRenderingCanceled;
						(_renderCallbackObject->*_renderCallbackMethod)(0, &isRenderingCanceled);
						if (isRenderingCanceled) _instantNodeRestore = false;
						_instantNodeRestoreCount = 0;
					}
					else
					{
						++_instantNodeRestoreCount;
					}

					// Since interactive rendering is not necessary we load the node right away
					restoreChildNodesFromBackingStore(node);
					finerLODVoxelCount += node->children[i]->quantPointCount;
					
					// Make sure the new loaded data is shown right away
					// (thus we avoid deep long loads)
					copyVoxelsToGPU = true;
				}
				else
				{
					// We found a child. But child is on backing store.
					if (_asyncRestoreRingBufferTail >= OCTREE_ASYNC_NODE_RESTORE_RING_BUFFER_LENGTH - 1)
					{
						_asyncRestoreRingBufferTail = 0;
					}
					else
					{
						++_asyncRestoreRingBufferTail;
					}
					
					_asyncRestoreRingBuffer[_asyncRestoreRingBufferTail] = node;
					*_bufferFlags |= OCTREE_NODES_IN_RESTORE_QUEUE;
				}
			}
		}

		// Check if we have greater detail in the next deeper level. If no, draw this level
		// TODO: Disable this switch in non-movement mode to suppress artifacts
		if (finerLODVoxelCount < node->quantPointCount) copyVoxelsToGPU = true;
	}
		
	#if USE_VOXEL_ACCU
	uint32_t accuIndex;
	
	if (copyVoxelsToGPU == true)
	{
		// Project node center onto the screen
		int16_t x, y;
		_renderViewFrustum->projectedPointOnScreen(&floatingNodeCenter, &x, &y);
	
		// Adjust projection to voxel accu resolution (1/8 * 1/8 of screen)
		x = x >> 3;
		y = y >> 3;
	
		// Don't draw anything if it is off-screen
		if ((x >= 0) &&	(x < _voxelAccuWidth) && 
			(y >= 0) && (y < _voxelAccuHeight))
		{				
			// Accu is stored as flat one dimensional array. Thus we have to encode the thre dimensions
			accuIndex = _voxelAccuWidth * (level * _voxelAccuHeight + y) + x;
			assert(accuIndex <= _voxelAccuHeight * _voxelAccuWidth * (OCTREE_LEAF_LEVEL + 1));
		
			// Check if the accu is full for this part of the screen
			// We don't recurse here because any deeper level will unlikly be "less full"
			if (_voxelAccu[accuIndex] > _voxelAccuThreshold) return;
		}
		else
		{
			// The node should have been drawn but it does not project on the screen.
			// Since the node should have been drawn it is unlikely that a child should be drawn,
			// therefore we stop the recursion here
			return;
		}
	}
	#endif
	
	if ((copyVoxelsToGPU == true) && (_regionOrginSet[level] == false))
	{
		// Region orgin is not set, yet. Thus we do not copy the voxels to GPU since the shader
		// would not be able to decode them with the right region. We visit the children to
		// calculate the region origin for all necessary levels. Consequently the next octree
		// traversal will have valid regions origins on every level.
		copyVoxelsToGPU = false;
		*_bufferFlags |= OCTREE_REGION_ORGIN_INVALID;
	}

	if (copyVoxelsToGPU)
	{
		// Points per node is limited to quantization grid
		assert(node->quantPointCount <= OCTREE_NODE_EDGE_SEGEMENTATION *
										OCTREE_NODE_EDGE_SEGEMENTATION *
										OCTREE_NODE_EDGE_SEGEMENTATION);
											
		#ifdef CHECK_VOXEL_COUNT
		// Checks number of quant points stated in a node
		uint32_t debugQuantPointCount = 0;
		#endif

		// Check if we have space left in buffer for new points
		if ((*_voxelCount + node->quantPointCount) >= _maxPointsInBuffer)
		{
			bool isRenderingCanceled;
			(_renderCallbackObject->*_renderCallbackMethod)(*_voxelCount, &isRenderingCanceled);
			
			if (isRenderingCanceled)
			{
				*_bufferFlags |= OCTREE_RENDERING_CANCELED;
				return;
			}
			else
			{
				*_voxelCount = 0;
			}
		}
		
		// regionLevelBuffer contains region and level data
		// voxelBuffer contains actual voxel data
		#if COMPACT_NODE_REGION_ENCODING
		// In compact mode every voxel has 32 bit region information and 32 bit actual voxel
		// information
		uint32_t* regionLevelBuffer = _regionLevelBuffer + (*_voxelCount);
		uint32_t* voxelBuffer = _voxelBuffer + (*_voxelCount);
		
		#else
		// In non-compact mode every voxel has 64 bit region information and 32 bit actual voxel
		// information
		uint32_t* regionLevelBuffer = _regionLevelBuffer + 2 * (*_voxelCount);
		uint32_t* voxelBuffer = _voxelBuffer + (*_voxelCount);
		#endif
		
		// Node position is quantized with x-bit relative to region origin
		const uint32_t nodePosX = (nodeCenter->x - _regionOrigin[level].x) >> (OCTREE_LEAF_LEVEL + 4 - level); // = / (2 * _nodeIncircleRadius[level]); 
		const uint32_t nodePosY = (nodeCenter->y - _regionOrigin[level].y) >> (OCTREE_LEAF_LEVEL + 4 - level);
		const uint32_t nodePosZ = (nodeCenter->z - _regionOrigin[level].z) >> (OCTREE_LEAF_LEVEL + 4 - level);
		assert(level < 32);

		// Check if node quantization is really within 9-bit
		// TODO: In case of non-Compact Mode we can change the octree region bits to 16
		if ((nodePosX >= (1 << OCTREE_REGION_BITS_PER_DIMENSION)) ||
			(nodePosY >= (1 << OCTREE_REGION_BITS_PER_DIMENSION)) ||
			(nodePosZ >= (1 << OCTREE_REGION_BITS_PER_DIMENSION)))
		{
			logError("Region bits are not sufficent to encode node center.");
			*_bufferFlags |= OCTREE_REGION_ORGIN_INVALID;
			return;
		}
		
		// Increase voxel count by number of voxels in the current node
		*_voxelCount = (*_voxelCount + node->quantPointCount);

		#if USE_VOXEL_ACCU
		_voxelAccu[accuIndex] += node->quantPointCount; 
		#endif
		
		#if COMPACT_NODE_REGION_ENCODING
		// Set quantized node position (24-bit of 32-bit) or level (8-bit of 32-bit)
		const uint32_t nodePositionLevel = (0 |	(nodePosX << (0 * OCTREE_REGION_BITS_PER_DIMENSION)) | 
												(nodePosY << (1 * OCTREE_REGION_BITS_PER_DIMENSION)) | 
												(nodePosZ << (2 * OCTREE_REGION_BITS_PER_DIMENSION)) |
												(level << (3 * OCTREE_REGION_BITS_PER_DIMENSION)));
												
		memset_pattern4(regionLevelBuffer, &nodePositionLevel, sizeof(uint32_t) * node->quantPointCount);
		#else
		const uint64_t nodePositionLevel = (0 |	(uint64_t(nodePosX) << (0 * OCTREE_REGION_BITS_PER_DIMENSION)) | 
												(uint64_t(nodePosY) << (1 * OCTREE_REGION_BITS_PER_DIMENSION)) |
												(uint64_t(nodePosZ) << (2 * OCTREE_REGION_BITS_PER_DIMENSION)) | 
												(uint64_t(level) << (3 * OCTREE_REGION_BITS_PER_DIMENSION)));											
		
		memset_pattern8(regionLevelBuffer, &nodePositionLevel, sizeof(uint64_t) * node->quantPointCount);
		#endif
	
		// copy points into buffer
		QuantPointBlock* block = node->data;
		
		while (block != NULL)
		{
			if ((block->next != NULL) || ((node->quantPointCount % OCTREE_POINTS_PER_POINT_DATA_BLOCK) == 0))
			{
				memcpy(voxelBuffer, block, sizeof(QuantPoint) * OCTREE_POINTS_PER_POINT_DATA_BLOCK);
	
				voxelBuffer += OCTREE_POINTS_PER_POINT_DATA_BLOCK;
				#ifdef CHECK_VOXEL_COUNT
				debugQuantPointCount += OCTREE_POINTS_PER_POINT_DATA_BLOCK;
				#endif
			}
			else
			{
				memcpy(voxelBuffer, block, sizeof(QuantPoint) * (node->quantPointCount % OCTREE_POINTS_PER_POINT_DATA_BLOCK));
	
				#ifdef CHECK_VOXEL_COUNT
				debugQuantPointCount += (node->quantPointCount % OCTREE_POINTS_PER_POINT_DATA_BLOCK);
				#endif
			}
			
			block = block->next;
		}
		#ifdef CHECK_VOXEL_COUNT
		assert(debugQuantPointCount == node->quantPointCount);
		#endif
	}
	else
	{
		#if TRAVERSE_OCTREE_BASED_ON_DISTANCE
		Node* childNode;
		FIXPVECTOR3 childCenter[8];
		bool active[8];
		double_t dist[8];
		
		for (uint8_t i = 0; i < 8; ++i)
		{
			childNode = node->children[i];
			active[i] = false;
			
			if ((node->children[i] != NULL) && (node->isChildInMemory(i)))
			{
				active[i] = true;
				
				lockNode(node->children[i]);
				
				// Calc child node center
				childCenter[i].x = nodeCenter->x;
				childCenter[i].y = nodeCenter->y;
				childCenter[i].z = nodeCenter->z;
				calcCenterOfChildNode(&(childCenter[i]), i, level+1);
			
				// TODO: This calcluation is done twice. Room for improvement
				const VECTOR3 floatChildNodeCenter = {childCenter[i].x, childCenter[i].y, childCenter[i].z};
				dist[i] = _renderViewFrustum->squaredDistanceToCamera(&floatChildNodeCenter);
			}
		}
		
		double_t minDistValue;
		uint8_t minDistIndex;
		bool firstDistFound;
		
		// TODO: This loop looks quite expensive (PROFILING!)
		do
		{
			firstDistFound = false;
			
			for (uint8_t j = 0; j < 8; ++j)
			{
				if (active[j] && ((dist[j] <= minDistValue) || !firstDistFound))
				{
					minDistValue = dist[j];
					minDistIndex = j;
					firstDistFound = true;
				}
			}
			
			if (firstDistFound)
			{
				assert(minDistIndex < 8);
				
				childNode = node->children[minDistIndex];

				copyPointsToBuffer(
						childNode,
						&(childCenter[minDistIndex]),
						level+1,
						nodeViewFrustumRelation);
				
				unlockNode(childNode);
				
				active[minDistIndex] = false;
			}
			
		} while (firstDistFound);
		
		#else

		Node* childNode;
		FIXPVECTOR3 childCenter;
		
		for (uint8_t i = 0; i < 8; ++i)
		{
			// TODO: node locking required?!
			
			childNode = node->children[i];
			if ((node->children[i] != NULL) && (node->isChildInMemory(i)))
			{
				// Calc child node center
				childCenter.x = nodeCenter->x;
				childCenter.y = nodeCenter->y;
				childCenter.z = nodeCenter->z;
				calcCenterOfChildNode(&childCenter, i, level+1);
				
				copyPointsToBuffer(
						childNode,
						&childCenter,
						level+1,
						nodeViewFrustumRelation);
			}
		}
		#endif
	}
}


// Method is called from the OpenGL thread
void Octree::copyPointsToBuffer(
	const float_t renderQuality,
	const bool instantNodeRestore,
	uint32_t* const outRegionLevelBuffer,
	uint32_t* const outVoxelBuffer,
	uint32_t* const outVoxelCount,
	uint8_t* const outBufferFlags)
{
	assert(*outBufferFlags == 0);
	
	_regionLevelBuffer = outRegionLevelBuffer;
	_voxelBuffer = outVoxelBuffer;
	_voxelCount = outVoxelCount;
	_bufferFlags = outBufferFlags;
	_renderQuality = renderQuality;
	_instantNodeRestore = instantNodeRestore;
	_instantNodeRestoreCount = 0;
	
	// No voxels in buffer, yet
	*_voxelCount = 0;
	
	// Clear all buffer flags
	//*_bufferFlags = 0;
			
	// Initilize min/max value to determine next frame regions
	for (uint8_t i = 0; i <= OCTREE_LEAF_LEVEL; ++i)
	{
		_nextFrameRegionMin[i].x = INT32_MAX;
		_nextFrameRegionMin[i].y = INT32_MAX;
		_nextFrameRegionMin[i].z = INT32_MAX;
		
		_nextFrameRegionMax[i].x = INT32_MIN;
		_nextFrameRegionMax[i].y = INT32_MIN;
		_nextFrameRegionMax[i].z = INT32_MIN;
	}

	#if USE_VOXEL_ACCU
	// Reset voxel accu
	memset(_voxelAccu, 0, _voxelAccuHeight * _voxelAccuWidth * (OCTREE_LEAF_LEVEL + 1) * sizeof(uint16_t));
	#endif

	FIXPVECTOR3 rootCenter = {0, 0, 0};
	
	APIFactory::GetInstance().lock(OCTREE_LOCK);
	
#if 0 // render low res underneath
	_renderQuality = 0.0005f;
	copyPointsToBuffer(
		_rootNode,
		&rootCenter,
		0,
		MiniGL::ViewFrustum::INTERSECT);
	bool isRenderingCanceled;
	(_renderCallbackObject->*_renderCallbackMethod)(*_voxelCount, &isRenderingCanceled);
	*_voxelCount = 0;
	glClear(GL_DEPTH_BUFFER_BIT);
	_renderQuality = renderQuality;
#endif

	copyPointsToBuffer(
		_rootNode,
		&rootCenter,
		0,
		MiniGL::ViewFrustum::INTERSECT);
				
	APIFactory::GetInstance().unlock(OCTREE_LOCK);

	// Send last point batch to GPU
	if ((*_bufferFlags & OCTREE_RENDERING_CANCELED) == false)
	{
		bool dummy;
		(_renderCallbackObject->*_renderCallbackMethod)(*_voxelCount, &dummy);
	}
	
	// Iterate over all levels exepct root level because root level orgin will never change
	for (uint8_t i = 1; i <= OCTREE_LEAF_LEVEL; ++i)
	{
		if ((_nextFrameRegionMin[i].x == INT32_MAX) ||
			(_nextFrameRegionMin[i].y == INT32_MAX) ||
			(_nextFrameRegionMin[i].z == INT32_MAX) ||
			(_nextFrameRegionMax[i].x == INT32_MIN) ||
			(_nextFrameRegionMax[i].y == INT32_MIN) ||
			(_nextFrameRegionMax[i].z == INT32_MIN))
		{
			continue;
		}
		
		_regionOrginSet[i] = true;
		
		assert((_nextFrameRegionMin[i].x - _nodeIncircleRadius[i]) % (2 * _nodeIncircleRadius[i]) == 0);
		assert((_nextFrameRegionMin[i].y - _nodeIncircleRadius[i]) % (2 * _nodeIncircleRadius[i]) == 0);
		assert((_nextFrameRegionMin[i].z - _nodeIncircleRadius[i]) % (2 * _nodeIncircleRadius[i]) == 0);
		assert((_nextFrameRegionMax[i].x - _nodeIncircleRadius[i]) % (2 * _nodeIncircleRadius[i]) == 0);
		assert((_nextFrameRegionMax[i].y - _nodeIncircleRadius[i]) % (2 * _nodeIncircleRadius[i]) == 0);
		assert((_nextFrameRegionMax[i].z - _nodeIncircleRadius[i]) % (2 * _nodeIncircleRadius[i]) == 0);
		
		// Increase every dimension by one node if the number of nodes is even to make them all odd
		//TODO: http://lab.polygonal.de/2007/05/10/bitwise-gems-fast-integer-math/
		// use fast even/uneven here
		if (((_nextFrameRegionMax[i].x - _nextFrameRegionMin[i].x) / (2 * _nodeIncircleRadius[i])) % 2 == 0)
			_nextFrameRegionMax[i].x += 2 * _nodeIncircleRadius[i];
		
		if (((_nextFrameRegionMax[i].y - _nextFrameRegionMin[i].y) / (2 * _nodeIncircleRadius[i])) % 2 == 0)
			_nextFrameRegionMax[i].y += 2 * _nodeIncircleRadius[i];
		
		if (((_nextFrameRegionMax[i].z - _nextFrameRegionMin[i].z) / (2 * _nodeIncircleRadius[i])) % 2 == 0)
			_nextFrameRegionMax[i].z += 2 * _nodeIncircleRadius[i];
			
		// Calc the position of the center node (this works because all dimension have an odd count)
		FIXPVECTOR3 nextFrameRegionCenter = {	(_nextFrameRegionMax[i].x - _nextFrameRegionMin[i].x) >> 1,
												(_nextFrameRegionMax[i].y - _nextFrameRegionMin[i].y) >> 1,
												(_nextFrameRegionMax[i].z - _nextFrameRegionMin[i].z) >> 1};


		// Calc region orgin by considering the region center and shifting half the precision
		_regionOrigin[i].x = _nextFrameRegionMin[i].x + nextFrameRegionCenter.x - _nodeIncircleRadius[i] - (1 << OCTREE_REGION_BITS_PER_DIMENSION) * _nodeIncircleRadius[i];
		_regionOrigin[i].y = _nextFrameRegionMin[i].y + nextFrameRegionCenter.y - _nodeIncircleRadius[i] - (1 << OCTREE_REGION_BITS_PER_DIMENSION) * _nodeIncircleRadius[i];
		_regionOrigin[i].z = _nextFrameRegionMin[i].z + nextFrameRegionCenter.z - _nodeIncircleRadius[i] - (1 << OCTREE_REGION_BITS_PER_DIMENSION) * _nodeIncircleRadius[i];
		
		// Precalc region origin in float for the shader.
		// Attention: region origin is shifted by nodeIncircleRadius to simplify voxel position
		// decoding in vertex shader
		// Before: (relativeVoxelPosition - 4.0) * voxelIncircleDiameter[level];
		// After: relativeVoxelPosition * voxelIncircleDiameter[level];
		_regionOriginFloat[i].x = _regionOrigin[i].x - _nodeIncircleRadius[i];
		_regionOriginFloat[i].y = _regionOrigin[i].y - _nodeIncircleRadius[i];
		_regionOriginFloat[i].z = _regionOrigin[i].z - _nodeIncircleRadius[i];
		
		assert((_nodeIncircleRadius[i] - _regionOrigin[i].x) % (2 * _nodeIncircleRadius[i]) == 0);
		assert((_nodeIncircleRadius[i] - _regionOrigin[i].y) % (2 * _nodeIncircleRadius[i]) == 0);
		assert((_nodeIncircleRadius[i] - _regionOrigin[i].z) % (2 * _nodeIncircleRadius[i]) == 0);
	}
}


uint32_t Octree::quantPointCount() const
{
	return _pointCount;
}


uint32_t Octree::nodeCount() const
{
	return _nodeCount;
}


void Octree::printStatistics() const
{
	std::cout << std::endl << ">> Octree Statistics" << std::endl;
	std::cout << "Number of Nodes in Memory: " << _nodeCount << std::endl;
	std::cout << "Number of Points in Memory: " << _pointCount << std::endl;
	std::cout << std::endl;
	
#if USE_MEMORY_POOL
	_memoryPool->printStatistics();
#endif
}


void Octree::restoreNodes(const uint32_t nodeCount)
{
	APIFactory::GetInstance().lock(OCTREE_LOCK);

	uint32_t counter = 0;
	while (	(_asyncRestoreRingBuffer[_asyncRestoreRingBufferTail] != NULL) &&
			(counter < nodeCount))
	{
		// Detach node from ringbuffer tail
		Node* node = _asyncRestoreRingBuffer[_asyncRestoreRingBufferTail];
		_asyncRestoreRingBuffer[_asyncRestoreRingBufferTail] = NULL;

		restoreChildNodesFromBackingStore(node);
		
		if (_asyncRestoreRingBufferTail == 0)
		{
			_asyncRestoreRingBufferTail = OCTREE_ASYNC_NODE_RESTORE_RING_BUFFER_LENGTH - 1;
		}
		else
		{
			--_asyncRestoreRingBufferTail;
		}
		
		++counter;
	}

	APIFactory::GetInstance().unlock(OCTREE_LOCK);
}


}