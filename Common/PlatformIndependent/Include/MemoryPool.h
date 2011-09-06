/*
 *  MemoryPool.h
 *  iPhoneApp
 *
 *  Created by Lars Schneider on 03.08.10.
 *  Copyright 2010 Lars Schneider. All rights reserved.
 *
 */
#ifndef MEMORYPOOL_H
#define MEMORYPOOL_H


#include <stddef.h>
#include <stdint.h>


namespace WVSClientCommon
{


class MemoryPool
{

	// Every bin saves the last 1024 free blocks
	static const uint16_t INDEX_WITH_FREE_BLOCK_COUNT = 1024;

	typedef struct MemoryBin
	{
		uint8_t*	index;
		uint8_t*	indexWithFreeBlock[INDEX_WITH_FREE_BLOCK_COUNT];
		uint8_t*	payload;
		size_t		elementSize;
		size_t		elementCount;
	};

	const size_t	_binCount;
	MemoryBin*		_bin;

	uint8_t countBitsInAByte(uint8_t byte) const;
	uint8_t* popFreeBlockPointer(const size_t binID);
	void pushFreeBlockPointer(const size_t binID, uint8_t* const ptr);
	
public:
	MemoryPool(const size_t binCount);
	~MemoryPool();
	
	void setupElementBin(const size_t binID, const size_t elementSize, const size_t elementCount);
	void* binAlloc(const size_t size);
	void binFree(const void* const ptr);
	
	size_t countElementsInBin(const size_t binID) const;
	size_t getMaximalNumberOfElementsInBin(const size_t binID) const;
	
	const void* const getIndexPointer(const size_t binID) const;
	const void* const getPayloadPointer(const size_t binID) const;
	
	void printStatistics() const;
};
	

}

#endif
