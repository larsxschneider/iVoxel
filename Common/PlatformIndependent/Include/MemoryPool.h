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

	struct MemoryBin
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
