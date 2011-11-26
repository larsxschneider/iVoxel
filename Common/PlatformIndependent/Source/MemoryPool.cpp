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

#include "MemoryPool.h"
#include "DebugConfig.h"
#include <iomanip>
#include <limits>


namespace WVSClientCommon
{


MemoryPool::MemoryPool(const size_t binCount) :
	_binCount(binCount)
{
	_bin = new MemoryBin[binCount];
	
	for (size_t i = 0; i < _binCount; ++i)
	{
		_bin[i].index = NULL;
		
		for (uint32_t j = 0; j < INDEX_WITH_FREE_BLOCK_COUNT; ++j)
		{
			_bin[i].indexWithFreeBlock[j] = NULL;
		}
	}
}


MemoryPool::~MemoryPool()
{
	for (size_t i = 0; i < _binCount; i++)
	{
		if (_bin[i].index != NULL) free(_bin[i].index);
	}
	
	delete[] _bin;
}


void MemoryPool::setupElementBin(	const size_t binID, 
									const size_t elementSizeInBytes,
									const size_t poolSizeInBytes)
{
	assert(binID < _binCount);
	assert(poolSizeInBytes != 0);
	assert((poolSizeInBytes % 4096) == 0);
	
	// Attention: Multiply 8 sperate because poolSizeInBytes * 8 might exceed 32 bit integer
	size_t elementCount = 8 * (poolSizeInBytes / (1 + 8 * elementSizeInBytes));
	elementCount -= (elementCount % 32 == 0 ? 0 : elementCount % 32);
	assert(elementCount > 0);
	assert((elementCount % 32) == 0);
	
	_bin[binID].elementSize = elementSizeInBytes;
	_bin[binID].elementCount = elementCount;
	_bin[binID].index = (uint8_t*)malloc(poolSizeInBytes);
	_bin[binID].indexWithFreeBlock[0] = _bin[binID].index;
	_bin[binID].payload = _bin[binID].index + elementCount / 8;	
	memset(_bin[binID].index, 0, poolSizeInBytes);

	logInfo("MemPool: Bin ID:%li ElementSize:%li Bytes ElementCount:%li Memory:%.2f MB Padding:%li Bytes",
		binID, elementSizeInBytes, elementCount, float(poolSizeInBytes)/(1024*1024),
		poolSizeInBytes - (elementCount / 8) - elementCount * elementSizeInBytes);
}


void MemoryPool::pushFreeBlockPointer(const size_t binID, uint8_t* const ptr)
{
	uint16_t freeBlockID = 0;
	while (	(_bin[binID].indexWithFreeBlock[freeBlockID] != NULL) && 
			(freeBlockID < INDEX_WITH_FREE_BLOCK_COUNT))
		freeBlockID++;
		
	if (freeBlockID < INDEX_WITH_FREE_BLOCK_COUNT)
	{
		_bin[binID].indexWithFreeBlock[freeBlockID] = ptr;
	}
	else
	{
		// No free block in free-block list left, search the entire memory pool for a block
		//logInfo("MemPool: Free block was not set. List for pool %li is full.\n", binID);
	}
}


uint8_t* MemoryPool::popFreeBlockPointer(const size_t binID)
{
	uint16_t freeBlockID = 0;
	while (	(_bin[binID].indexWithFreeBlock[freeBlockID] == NULL) && 
			(freeBlockID < INDEX_WITH_FREE_BLOCK_COUNT))
		freeBlockID++;
	
	uint8_t* ptr;
	if (freeBlockID < INDEX_WITH_FREE_BLOCK_COUNT)
	{
		// Read pointer from list
		ptr = _bin[binID].indexWithFreeBlock[freeBlockID];
		
		// Reset list position
		_bin[binID].indexWithFreeBlock[freeBlockID] = NULL;
	}
	else
	{
		// No free block in free-block list left, search the entire memory pool for a block
		//logInfo("MemPool: No free block on free-block-list in pool %li. Entire search necessary.\n", binID);
		ptr = _bin[binID].index;		
		while ((*((uint32_t*)(ptr)) == 0xFFFFFFFF) && (ptr < _bin[binID].payload)) ptr += 4;
		while ((*((uint16_t*)(ptr)) == 0xFFFF) && (ptr < _bin[binID].payload)) ptr += 2;
		while ((*ptr == 0xFF) && (ptr < _bin[binID].payload)) ptr++;
	}
	
	return ptr;
}


void* MemoryPool::binAlloc(const size_t size)
{
	size_t binID = 0;
	while ((size != _bin[binID].elementSize) && (binID < _binCount)) binID++;
	assert(binID < _binCount);
	
	uint8_t* ptr = popFreeBlockPointer(binID);
	
	if ((*ptr != 0xFF) && (ptr < _bin[binID].payload))
	{
		// Free mem avialable
		
		// Find free block
		uint8_t j = 0;
		uint8_t v = *ptr;
		while ((~v & (1 << j)) == 0) j++;
		assert(j < 8);
		
		// Set block used
		(*ptr) |= (1 << j);
		
		// If the chuck has still free blocks, push it the the free pointer list
		// If not check the next chunk
		if ((*ptr != 0xFF) && (ptr < _bin[binID].payload)) pushFreeBlockPointer(binID, ptr);
		else if ((*(ptr+1) != 0xFF) && ((ptr+1) < _bin[binID].payload)) pushFreeBlockPointer(binID, (ptr+1));
				
		void* address = _bin[binID].payload + ((ptr - _bin[binID].index) * 8 + j) * _bin[binID].elementSize;
		
		// Make sure the newly allocated address space is clean
		// This should not be necessary...
		//memset(address, 0, _bin[binID].elementSize);
		
		return address;
	}
	else
	{
		//printf("Error: Bin %li is full.\n", binID);
		return NULL;
	}
}



void MemoryPool::binFree(const void* const ptr)
{
	// Find bin that contains the given address
	for (size_t i = 0; i < _binCount; i++)
	{
		// Check if pointer is within the memory of the bin
		if ((_bin[i].payload <= ptr) && (ptr <= _bin[i].payload + _bin[i].elementSize * _bin[i].elementCount))
		{
			// Bin found			
			uint32_t elementID = ((uint8_t*)ptr - _bin[i].payload) / _bin[i].elementSize;
			assert(elementID < _bin[i].elementCount);
			
			uint8_t bitID = elementID % 8;
			assert(bitID < 8);
			
			uint32_t byteID = (elementID - bitID) / 8;
			uint8_t* byte = _bin[i].index + byteID;
			assert(byte < _bin[i].payload);
			
			// Unset block
			*byte &= ~(1 << bitID);
			
			// Push free block to free block pointer list
			pushFreeBlockPointer(i, byte);
						
			return;
		}
	}
}


// see http://www.keil.com/support/docs/194.htm
// or http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetKernighan
uint8_t MemoryPool::countBitsInAByte(uint8_t byte) const
{
	uint8_t count;

	for (count = 0; byte != 0; count++)
	{
		byte &= byte - 1; // this clears the LSB-most set bit
	}

	return (count);
}


size_t MemoryPool::countElementsInBin(const size_t binID) const
{
	size_t count = 0;
	uint8_t* i = _bin[binID].index;
	
	while (i < _bin[binID].payload)
	{
		count += countBitsInAByte(*i);
		i++;
	}
	
	return count;
}


size_t MemoryPool::getMaximalNumberOfElementsInBin(const size_t binID) const
{
	return _bin[binID].elementCount;
}


const void* const MemoryPool::getIndexPointer(const size_t binID) const
{
	return _bin[binID].index;
}


const void* const MemoryPool::getPayloadPointer(const size_t binID) const
{
	return _bin[binID].payload;
}


void MemoryPool::printStatistics() const
{
	std::cout << std::endl << ">> Memory Pool Statistics" << std::endl;
	for (size_t i = 0; i < _binCount; i++)
	{
		uint32_t count = countElementsInBin(i);
		std::cout	<< "Bin " << i << " Used: << " << count << " ("  
					<< std::setprecision(2) << std::fixed << 100.0f * float(count) / float(_bin[i].elementCount)
					<< "%)" << std::endl;
	}
	std::cout << std::endl;	
}





}
	