/*
 *  DebugConfig.h
 *  MacApp
 *
 *  Created by Lars Schneider on 01.07.10.
 *  Copyright 2010 Lars Schneider. All rights reserved.
 *
 */
 
  
#ifndef DEBUG_CONFIG_H
#define DEBUG_CONFIG_H

#ifndef DEBUG
#define NDEBUG
#endif
#include <assert.h>
#include <iostream>

/***************************************************************************************************
	Log Methods
***************************************************************************************************/

#ifdef DEBUG
#define logInfo(...) { fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n"); }
#define logError(...) { fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n"); }
#else
#define logInfo(...)
#define logError(...)
#endif


/***************************************************************************************************
	General Switches
***************************************************************************************************/

//#define DISCARD_SKYBOX 1

#define STATIC_SKYBOX 1

// Switch here between custom memory allocation and OS memory allocation
#define USE_MEMORY_POOL 1

// Enabled/Disable backing store
#define USE_BACKING_STORE 1

//#define IMPORT_STATIC_POINT_CLOUD 1

#define USE_STATIC_POINT_CLOUD 1
#ifdef USE_STATIC_POINT_CLOUD
static const char* const STATIC_POINT_CLOUD_FILE = "lucy_and_manuscript.dat";
#endif

// Traverses the octree based on distance (near nodes first, far nodes last)
// Deactivated means all octree nodes are traversed in the same order
#define TRAVERSE_OCTREE_BASED_ON_DISTANCE 1

// Calc voxels on screen (only useful if octree is traveresed based on distance)
#define USE_VOXEL_ACCU 1

// Use Software Renderer on OS X (Macbook HW accelerated OpenGL has Errors)
//#define OSX_SOFTWARE_RENDERER 1

//#define RUN_PERFORMANCE_TEST 1

//#define RENDER_OCTREE_DEBUG_VOXELS 1

//#define USE_PRECALCULATED_NORMAL_ARRAYS 1

//#define MEASURE_TIMING 1

//#define POST_PROCESSING 0

//#define PROCESS_NORMALS 0


/***************************************************************************************************
	Vertex Submission Switches
***************************************************************************************************/

// GPU memory is mapped into main memory and consequently vertex data is directly written to GPU
// Switch between DIRECT_VBO and NO_VBO
#define DIRECT_VBO 0
	
#if DIRECT_VBO
	// Vertex data is double buffered to avoid stalling
	#define DIRECT_VBO_DOUBLE_BUFFERED 0
#else
	// No vertex buffer objects are used at all
	#define NO_VBO 1
#endif

// Encode region+level either with 8-bit or 16-bit
#define COMPACT_NODE_REGION_ENCODING 1


/***************************************************************************************************
	Benchmark Switches
***************************************************************************************************/
//#define BENCHMARK_1_MILLION 1


/***************************************************************************************************
	Debug Switches
***************************************************************************************************/

#ifdef DEBUG

//#define TIMER 1
//#define DISCARD_SKYBOX 1
#define PRINT_STATISTICS 1
//#define RENDER_OVERLAPPING_CELLS_RED 1
//#define TINT_OCTREE_LEVELS 1
//#define TINT_BACKING_STORE_LOADED_POINTS_RED 1
//#define RENDER_BACKFACE_CULLING_RED 1


#define CHECK_VOXEL_COUNT 1
#endif


/***************************************************************************************************
	Helper Defines
***************************************************************************************************/

#if TIMER
#include "Timer.h"
#define RESET_TIMER WVSClientCommon::resetTimer();
#define STOP_TIMER printf("Elapsed time: %.0fms", WVSClientCommon::getElapsedTime());
#else
#define RESET_TIMER
#define STOP_TIMER
#endif

#endif