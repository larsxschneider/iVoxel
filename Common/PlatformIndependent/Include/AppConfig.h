/*
 *  Config.h
 *  MacApp
 *
 *  Created by Lars Schneider on 01.06.10.
 *  Copyright 2010 Lars Schneider. All rights reserved.
 *
 */
 
#ifndef APP_CONFIG_H
#define APP_CONFIG_H


#include "Vector.h"


namespace WVSClientCommon
{


static const float_t		WVS_SKYBOX_THRESHOLD = 0.01f; // deprecated?!

#if USE_STATIC_POINT_CLOUD
static const VECTOR3		WVS_CENTER_POSITION = {0,0,0};
#else
static const VECTOR3		WVS_CENTER_POSITION = {	WVS_LOWER_CORNER.x + 
													(WVS_UPPER_CORNER.x - WVS_LOWER_CORNER.x) / 2.0,
													WVS_LOWER_CORNER.y + 
													(WVS_UPPER_CORNER.y - WVS_LOWER_CORNER.y) / 2.0,
													WVS_LOWER_CORNER.z + 
													(WVS_UPPER_CORNER.z - WVS_LOWER_CORNER.z) / 2.0};
#endif


/***************************************************************************************************
	GPU Config
***************************************************************************************************/
// Batchsize of points per OpenGL draw call. One interactive frame should fit into that batch
// (on iPad ca 5 MB)
#if (TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE)
static const uint32_t		GPU_POINT_VRAM_MB = 6;
#else
static const uint32_t		GPU_POINT_VRAM_MB = 6;
#endif
#if COMPACT_NODE_REGION_ENCODING
// Compact encoding, 8 bytes per voxel (4 region+level + 4 voxel)
static const uint32_t		GPU_MAX_POINTS = GPU_POINT_VRAM_MB * (1024*1024) / 8;
#else
// Compact encoding, 12 bytes per voxel (8 region+level + 4 voxel)
static const uint32_t		GPU_MAX_POINTS = GPU_POINT_VRAM_MB * (1024*1024) / 12;
#endif
static const uint16_t		GPU_SKYBOX_SIZE = 512;


/***************************************************************************************************
	Camera Config
***************************************************************************************************/
static const float_t		CAMERA_NEAR_PLANE = 10.0f;
static const float_t		CAMERA_FAR_PLANE = 100000.0f;


#if USE_STATIC_POINT_CLOUD
static const float_t		CAMERA_FIELD_OF_VIEW_IN_DEGREE = 60.0f;

// Lucy settings
static const VECTOR3		CAMERA_START_POSITION = {-2584.86,   920.00,  -9026.70};
static const float_t		CAMERA_START_HEADING_RADIANS = -2.740166;

// Berlin settings
//static const float_t		CAMERA_START_HEADING_RADIANS = 0.0;
//static const VECTOR3		CAMERA_START_POSITION = {2970,0,4000};

// Head
//static const float_t		CAMERA_START_HEADING_RADIANS = 0.0;
//static const VECTOR3		CAMERA_START_POSITION = {0,0,4000};


//static const float_t		CAMERA_FIELD_OF_VIEW_IN_DEGREE = 90.0f;
//static const VECTOR3		CAMERA_START_POSITION = {0,0,0};
//static const float_t		CAMERA_START_HEADING_RADIANS = 0.0;

#else
static const float_t		CAMERA_FIELD_OF_VIEW_IN_DEGREE = 90.0f;
static const VECTOR3		CAMERA_START_POSITION = {-23058.830078, -5808.000000, -17174.701172};
static const float_t		CAMERA_START_HEADING_RADIANS = 0.0;
#endif

/***************************************************************************************************
	Octree Config
***************************************************************************************************/
// Leaf level is determined by (single) floating point precision of the camera.
// see http://de.wikipedia.org/wiki/IEEE_754
static const uint8_t		OCTREE_LEAF_LEVEL = 15;

static const float_t		OCTREE_SCALE = 20.0f;

// We assume in city models every node is cutted by a plane thus x^2 node cells are occupied
static const uint16_t		OCTREE_POINTS_PER_POINT_DATA_BLOCK = 31;

// Length of the ring buffer to restore nodes from backing store. The rendering thread pushes
// nodes to be restored into the buffer and the backing store thread pulls them out.
static const uint32_t		OCTREE_ASYNC_NODE_RESTORE_RING_BUFFER_LENGTH = 4096;


// Both values should be factors ot 4096 for best memory alignment/least iOS padding
// see http://developer.apple.com/mac/library/documentation/Performance/Conceptual/ManagingMemory/Articles/MemoryAlloc.html
#if (TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE)
static const uint32_t		OCTREE_NODE_MEMORY_MB = 16;
static const uint32_t		OCTREE_QUANT_POINT_BLOCK_MEMORY_MB = 80;
#else
static const uint32_t		OCTREE_NODE_MEMORY_MB = 16;
static const uint32_t		OCTREE_QUANT_POINT_BLOCK_MEMORY_MB = 80;
#endif

static const char* const	OCTREE_BACKING_STORE_FILENAME = "nodes.dat";


/***************************************************************************************************
	General Config
***************************************************************************************************/
static const float_t		RENDER_QUALITY_COARSE_ADJUSTMENT = 0.02f;
static const float_t		RENDER_QUALITY_FINE_ADJUSTMENT = 0.002f;
static const double_t		MINIMUM_FRAMERATE = 20.0;
static const double_t		MAXIMUM_FRAMERATE = 30.0;
#if USE_STATIC_POINT_CLOUD
static const uint32_t		OCTREE_POINT_ACCU_THRESHOLD = 1024;
#else
static const uint32_t		OCTREE_POINT_ACCU_THRESHOLD = 256;
#endif
static const uint32_t		OCTREE_INTERACTIVE_RENDERING_POINT_THRESHOLD = 50000;

// Reload delay for full resolution after camera change in ms
static const float_t		WVS_RELOAD_DELAY[] = {100.0f, 1000.0f}; 


/***************************************************************************************************
	Locks Config
***************************************************************************************************/
// Locks could have been implemented as objects. I opted for objects and reference them by ID to
// make their access easier on various places
static const int			OPENGL_LOCK = 0;
static const int			OCTREE_LOCK = 1;
static const int			BACKINGSTORE_LOCK = 2;
static const int			WVS_CODEC_LOCK = 3;
static const int			LOCK_COUNT = 4;

}

/***************************************************************************************************
	Architecture Config
***************************************************************************************************/
//#define PORTABLE_64_BIT 1
#define PORTABLE_32_BIT 1

#endif