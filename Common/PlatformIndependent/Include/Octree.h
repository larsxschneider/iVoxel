/*
 *  Octree.h
 *  MacApp
 *
 *  Created by Lars Schneider on 24.06.10.
 *  Copyright 2010 Lars Schneider. All rights reserved.
 *
 */

#ifndef OCTREE_H
#define OCTREE_H


#include <string>
#include <stdint.h>
#include "AppCore.h"
#include "AppConfig.h"
#include "DebugConfig.h"
#include "PointStructs.h"

	
namespace MiniGL
{
class ViewFrustum;
class Camera;
}


namespace WVSClientCommon
{


class MemoryPool;
class BackingStore;


typedef void (AppCore::*RenderCallbackMethodT)(const uint32_t pointsToRenderCount, bool* const isCancled) const;


class Octree
{


// Magic byte sequence to identify locked nodes. The "1" in the on the LSB is necessary to ensure
// we don't collide with a real node pointer address (they are always 2 byte aligned)
#if PORTABLE_32_BIT
static const int		OCTREE_LOCKED_NODE_IDENTIFIER = 0xBADBABE1;
#elif PORTABLE_64_BIT
static const int		OCTREE_LOCKED_NODE_IDENTIFIER = 0xFFFFFFFFBADBABE1;
#endif

public:

	#if COMPACT_NODE_REGION_ENCODING
	static const uint8_t	OCTREE_REGION_BITS_PER_DIMENSION	= 8;
	#else
	static const uint8_t	OCTREE_REGION_BITS_PER_DIMENSION	= 16;
	#endif
		
	static const uint8_t	OCTREE_NODES_IN_RESTORE_QUEUE		= 0x1;
	static const uint8_t	OCTREE_RENDERING_CANCELED			= 0x2;
	static const uint8_t	OCTREE_REGION_ORGIN_INVALID			= 0x4;

	typedef struct QuantPointBlock
	{
		QuantPoint			points[OCTREE_POINTS_PER_POINT_DATA_BLOCK];
		QuantPointBlock*	next;
		
		inline void reset()
		{
			memset(points, 0, sizeof(QuantPointBlock));
		}
	};
	
	typedef struct Node
	{
		Node*				prevUsedNode;					// 4 byte
		Node*				nextUsedNode;					// 8 byte
		QuantPointBlock*	data;							// 12 byte
		Node*				parent;							// 16 byte
		union												// 48 byte
		{
			// iPhone ARM is a 32bit architecture
			// Thus sizeof(Node*) == sizeof(long) == 4
			Node*				children[8];
			long				childrenFilePosition[8];
		};
		uint16_t			quantPointCount;				// 50 byte
		uint8_t				childrenInMemory;				// 51 byte
		
		void reset()
		{
			prevUsedNode = NULL;
			nextUsedNode = NULL;
			data = NULL;
			parent = NULL;
			quantPointCount = 0;
			childrenInMemory = 0xFF; // = all children in memory
			
			for (uint8_t i = 0; i < 8; i++)
			{
				children[i] = NULL;
			}
		}
		
		inline bool isChildInMemory(const int8_t childID) const
		{
			assert(childID < 8);
			return (childrenInMemory & (1 << childID));
		}
		
		inline void setChildInMemory(const int8_t childID)
		{
			assert(childID < 8);
			childrenInMemory |= (1 << childID);
		}
		
		inline void unsetChildInMemory(const int8_t childID)
		{
			assert(childID < 8);
			childrenInMemory &= ~(1 << childID);
		}
		
		inline bool isLocked() const
		{
			return (prevUsedNode == (Node*)OCTREE_LOCKED_NODE_IDENTIFIER);
		}
	};
	
	
	typedef struct FIXPVECTOR3
	{
		int32_t x;	/*!< x coordinate */
		int32_t y;	/*!< y coordinate */
		int32_t z;	/*!< z coordinate */
	};
	
	
	typedef struct CachedNode
	{
		Node*						node;
		FIXPVECTOR3					center;
	};


private:
	const AppCore* const				_renderCallbackObject;
	const RenderCallbackMethodT			_renderCallbackMethod;
	const MiniGL::ViewFrustum* const	_renderViewFrustum;
	const uint32_t						_maxPointsInBuffer;
	CachedNode*							_insertionNodeCache;
	bool*								_regionOrginSet;
	FIXPVECTOR3*						_regionOrigin;
	VECTOR3*							_regionOriginFloat;
	FIXPVECTOR3*						_nextFrameRegionMin;
	FIXPVECTOR3*						_nextFrameRegionMax;
	int32_t*							_voxelIncircleRadius;
	int32_t*							_nodeIncircleRadius;
	float_t*							_voxelIncircleDiameter;
	float_t*							_nodeIncircleDiameter;
	float_t*							_voxelCircumcircleRadius;
	float_t*							_nodeCircumcircleRadius;
	double_t*							_distanceLevelThreshold;
	uint32_t							_nodeCount;
	uint32_t							_pointCount;
	Node*								_rootNode;
	Node*								_leastRecentlyUsedNode;
	Node*								_mostRecentlyUsedNode;
	MemoryPool*							_memoryPool;
	BackingStore*						_backingStore;
	Node*								_asyncRestoreRingBuffer[OCTREE_ASYNC_NODE_RESTORE_RING_BUFFER_LENGTH];
	uint32_t							_asyncRestoreRingBufferTail;
	bool								_instantNodeRestore;
	uint32_t							_instantNodeRestoreCount;
	
	uint32_t*							_regionLevelBuffer;
	uint32_t*							_voxelBuffer;
	uint32_t*							_voxelCount;
	uint8_t*							_bufferFlags;
	double_t							_renderQuality;	// higher value means less quality, highest quality = 1
	
#if USE_VOXEL_ACCU
	uint16_t							_voxelAccuHeight;
	uint16_t							_voxelAccuWidth;
	uint16_t							_voxelAccuThreshold;
	uint16_t*							_voxelAccu;
#endif

	void checkLRU();
	void touchNode(Node* const node);
	void lockNode(Node* const node);
	void unlockNode(Node* const node);
	
	void freeNodeFromMemory(const Node* const node);
	
	void restoreChildNodesFromBackingStore(Node* parentNode);
	bool restoreNodeFromBackingStore(Node* const parentNode, const uint8_t nodeID);
	
	void swapNodeToBackingStore(Node* node, int parentNodeID);
	void swapLeastRecentlyUsedNodesToBackingStore();
	
	void calcCenterOfChildNode(	FIXPVECTOR3* const center, 
								const uint8_t childNodeID,
								const uint8_t level) const;
	
	void copyPointsToBuffer(
		Node* const node,
		const FIXPVECTOR3* const nodeCenter,
		const uint8_t level,
		MiniGL::ViewFrustum::Relation nodeViewFrustumRelation);
		
	void countRenderBufferPoints(
		Node* const node,
		const FIXPVECTOR3* const nodeCenter,
		const uint8_t level,
		const uint32_t maxPointCount,
		MiniGL::ViewFrustum::Relation nodeViewFrustumRelation);
	
	bool retrieveQuantPointInNode(
		Node* const node,
		const QuantPoint::PositionNormal quantizedPosition,
		QuantPoint** outQuantPoint);
		
	void writeNodeToDisk(FILE* const file, Node* const node, uint32_t* numberOfPointsWrittenToDisk);
		
		
public:

	Octree(	const uint32_t maxPointsInBuffer,
			const char* const backingStoreFilename,
			const bool isStaticFile,
			const MiniGL::ViewFrustum* const viewFrustum,
			const AppCore* const callbackObject,
			const RenderCallbackMethodT callbackMethod);
	~Octree();
	
	void saveToDisk(const char* const filename);
	
	void updateScreenSizeRelatedConstants();
	
#if USE_VOXEL_ACCU
	void setVoxelAccuThreshold(const uint16_t threshold);
#endif

	const QuantPoint::PositionNormal quantizeRelativePosition(
		const FIXPVECTOR3* const pointPosition,
		const FIXPVECTOR3* const referenceCenter,
		const int8_t level) const;
	
	void addPoint(const WVSPoint* const point);
	
	void copyPointsToBuffer(
		const float_t projectedVoxelSizeThreshold,
		const bool instantNodeRestore,
		uint32_t* const outRegionLevelBuffer,
		uint32_t* const outVoxelBuffer,
		uint32_t* const outVoxelCount,
		uint8_t* const outBufferFlags);
	
	void estimateInteractiveRenderQuality(
		float_t* const targetRenderQuality,
		const uint32_t maxPointCount);
	
	const float_t* getRegionOrginArray() const;
	const float_t* getVoxelIncircleDiameterArray() const;
	const float_t* getNodeIncircleDiameterArray() const;
	const float_t getVoxelCircumcircleRadius(const uint8_t level) const;
	
	uint32_t quantPointCount() const;
	uint32_t nodeCount() const;
	
	void printStatistics() const;
	
	void restoreNodes(const uint32_t nodeCount);
};
	
	
}

#endif /* end of include guard: OCTREE_H */