/*
 *  PerformanceTests.cpp
 *  iPhoneApp
 *
 *  Created by Lars Schneider on 07.06.10.
 *  Copyright 2010 Lars Schneider. All rights reserved.
 *
 */

#include "PerformanceTests.h"
#include "APIFactory.h"
#include <stdio.h>

#include "Octree.h"
#include "AppCore.h"
#include <iostream>
#include "Timer.h"
#include "MemoryPool.h"

//#include "neon_memcpy_impl.h"

#if RUN_PERFORMANCE_TEST




namespace WVSClientCommon
{


namespace Tests
{



//void NEONMemCpyTest()
//{
//	int* pbuf1, *pbuf2; 
//	const int bufSize = 8*1024*1024; 
//	int i,j; 
//	
//	clock_t t1, t2; 
//	double tdiff; 
//	const int ITER =100; 
//	double rate; 
//	typedef int v4si __attribute__ ((vector_size(16))); 
//	v4si *p1, *p2; 
//	pbuf1 = (int*)calloc(4, bufSize*sizeof(int)); 
//	pbuf2 = (int*)calloc(4, bufSize*sizeof(int)); 
//	for (i=0; i<bufSize; i++) 
//	{ 
//		pbuf2[i] = i; 
//	} 
//	t1 = clock(); 
//	for(j=0; j<ITER; j++) 
//	{ 
//		memcpy_armneon(pbuf1, pbuf2, bufSize*sizeof(int));
//	}
//
//	t2 = clock(); 
//	tdiff = (double)(t2) - (double)t1; 
//	rate = ITER*bufSize*sizeof(int)/(tdiff/CLOCKS_PER_SEC); 
//	rate /= (1024.0*1024.0); 
//	printf("rate(MB/S) = %.3f, clocks_per_sec %d\n", rate, CLOCKS_PER_SEC); 
///* 
//        for(i=900; i<910; i++) 
//        { 
//                printf("%d\n", pbuf1[i]); 
//        } 
//*/ 
//        //SIMD version of the memory benchmark 
//    t1 = clock(); 
//        for(j=0; j<ITER; j++) 
//        { 
//                memcpy(pbuf1, pbuf2, bufSize*sizeof(int));
//        } 
//        t2 = clock(); 
//        tdiff = (double)(t2) - (double)t1; 
//        rate = ITER*bufSize*sizeof(int)/(tdiff/CLOCKS_PER_SEC); 
//        rate /= (1024.0*1024.0); 
//        printf("SIMD rate(MB/S) = %.3f, clocks_per_sec %d\n", rate, CLOCKS_PER_SEC); 
//        free(pbuf1); 
//        free(pbuf2); 
//}

void OctreeInsertion()
{
	//APIFactory::GetInstance().setupResourcePath();
//	APIFactory::GetInstance().createLocks(LOCK_COUNT);
//	
//	Octree *octree = new Octree(	1638.4,
//									11,
//									OCTREE_BACKING_STORE_FILENAME);
//							
//	WVSPoint point;
//	
//	float size = 80;
//	float inc = 1.0f;
//	
//	printf("Point: %li bytes, All points: %.0f MB", sizeof(WVSPoint),
//		float(sizeof(WVSPoint)) * size * size * size * 1.0f/inc * 1.0f/inc * 1.0f/inc / (1024.0f * 1024.0f));
//	
//	resetTimer();
//	for (float x = 0; x < size; x += inc)
//	{
//		for (float y = 0; y < size; y += inc)
//		{
//			for (float z = 0; z < size; z += inc)
//			{
//				point.position.x = x;
//				point.position.y = y;
//				point.position.z = z;
//				point.normal = 0;
//				point.color.red = 1;
//				point.color.green = 2;
//				point.color.blue = 3;
//				point.radius = 0.0001f;
//				
//				octree->addPoint(&point, false);
//			}
//		}
//	}
//	
//	printf("Elapsed time: %.0fms", getElapsedTime());
//	
//	std::cout << "\nData Statistics:\n";
//	octree->printStatistics();
}


void MemoryPoolTest()
{
	//MemoryPool* memoryPool = new MemoryPool(1);
	
	//resetTimer();
	//memoryPool->setupElementBin(0, sizeof(Octree::Node), 4096 * 1024 * 16);
	
	Octree::Node* test, *parent;
	int count = 1287456;
	
	resetTimer();
	for (int i = 0; i < count; ++i)
	{
#if 0
		test = (Octree::Node*)memoryPool->binAlloc(sizeof(Octree::Node));
#else
		test = (Octree::Node*)malloc(sizeof(Octree::Node));
#endif
		if (test == NULL) printf("error");
		test->parent = parent;
		parent = test;
	}
	
	printf("time: %.0fms\n", getElapsedTime());
}

void MemoryAllocation()
{
	//http://developer.apple.com/iphone/library/documentation/Performance/Conceptual/ManagingMemory/Articles/MemoryAlloc.html
	
	// Perform memory allocation with calloc to ensure 0 initilized memory
	int sizeInBytes = 1024 * 1024 * 100; // 100 MB start
	int maxSize = 0;
	bool success = true;
	void* ptr;
	
	printf("\n\nAllocation Test\n");
	while (success)
	{
		ptr = calloc(sizeInBytes, 1 /* byte */);
		success = (ptr != NULL);
		
		if (success)
		{
			printf("%i bytes (= %i MB) successful\n", sizeInBytes, sizeInBytes / (1024 * 1024));
			maxSize = sizeInBytes;
		}
		else
		{
			printf("%i bytes (= %i MB) failed\n", sizeInBytes, sizeInBytes / (1024 * 1024));
		}

		free(ptr);
		
		sizeInBytes = (size_t)((float)sizeInBytes * 1.1f);
	}
	
	printf("\n\nWriting Test\n");
	ptr = calloc(maxSize, 1 /* byte */);
	if (ptr != NULL)
	{
		for (int i = 0; i < maxSize; i++)
		{
			if (i % (1024 * 1024 * 10 /* 10 MB */) == 0) printf("writing %i bytes (= %i MB)\n", i, i / (1024 * 1024));
			((unsigned char*)ptr)[i] = (rand() > 100 ? 1 : 0);
		}
		free(ptr);
	}
	else
	{
		printf("ERROR");
	}
}


void JPEGDecodingSpeed(unsigned char *data, const size_t size)
{
	unsigned char *imageData;
	unsigned int width;
	unsigned int height;
	unsigned int trials = 100;
	
	double t1 = WVSClientCommon::APIFactory::GetInstance().getTimeInMS();
	
	for (int i = 0; i < trials; i++)
	{		
		bool allocationSuccessful = MiniGL::AllocJPEGImageFromMemory(
			data,
			size,
			imageData,
			width,
			height,
			true,
			true);
		
		if (!allocationSuccessful) printf("fail");
		free(imageData);
	}
	double t2 = WVSClientCommon::APIFactory::GetInstance().getTimeInMS();
	
	printf("Zeit (%i trials; filesize: %lu bytes): %.6fs", trials, size, t2 - t1);
}


void PNGDecodingSpeed(unsigned char *data, const size_t size)
{
	unsigned char *imageData;
	unsigned int width;
	unsigned int height;
	unsigned int trials = 100;
	
	double t1 = WVSClientCommon::APIFactory::GetInstance().getTimeInMS();
	
	for (int i = 0; i < trials; i++)
	{		
		bool allocationSuccessful = MiniGL::AllocPNGImageFromMemory(
			data,
			size,
			imageData,
			width,
			height,
			true,
			true);
		
		if (!allocationSuccessful) printf("fail");
		free(imageData);
	}
	double t2 = WVSClientCommon::APIFactory::GetInstance().getTimeInMS();
	
	printf("Zeit (%i trials; filesize: %lu bytes): %.6fs", trials, size, t2 - t1);
}


}


}


#endif // RUN_PERFORMANCE_TEST
