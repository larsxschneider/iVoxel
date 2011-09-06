/*
 *  PerformanceTests.h
 *  iPhoneApp
 *
 *  Created by Lars Schneider on 07.06.10.
 *  Copyright 2010 Lars Schneider. All rights reserved.
 *
 */
#ifndef PERFORMANCE_TEST_H
#define PERFORMANCE_TEST_H
 
#include "DebugConfig.h"
#if RUN_PERFORMANCE_TEST
#include <stddef.h>


namespace WVSClientCommon
{


namespace Tests
{

void NEONMemCpyTest();
void MemoryPoolTest();
void OctreeInsertion();
void MemoryAllocation();
void JPEGDecodingSpeed(unsigned char *data, const size_t size);
void PNGDecodingSpeed(unsigned char *data, const size_t size);


}


}


#endif // RUN_PERFORMANCE_TEST
#endif // PERFORMANCE_TEST_H
