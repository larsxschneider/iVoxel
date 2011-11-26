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

#ifndef _RESOURCE_IMAGE_H_
#define _RESOURCE_IMAGE_H_

#include "PlatformOpenGL.h"
#include <string>
#include <stdint.h>

namespace MiniGL
{


bool AllocJPEGImageFromMemory(
	const uint8_t* source,
	const size_t size,
	uint8_t* &pixels,
	uint32_t &width,
	uint32_t &height,
	const bool flipVertical,
	const bool powerOfTwo);
	
bool AllocJPEGImageFromFile(
	const char* const filename,
	uint8_t *&pixels,
	uint32_t &width,
	uint32_t &height,
	bool flipVertical);

/**
	Returns the next greater power of two value
	@param intValue input value
	@returns power of two value
 */
uint32_t GetPowerOfTwoInteger(uint32_t intValue);

			
/**
	Loads a PNG from the resource directory.
	@param filename PNG filename in resource directory
	@param &pixels pointer to image data
	@param width image width
	@param height image height
	@param flipVertical option to flip the image vertically (in case the image is used as texture)
	@returns true if image load was successful
 */
bool AllocPNGImageFromFile(
	const char* const filename,
	uint8_t *&pixels,
	uint32_t &width,
	uint32_t &height,
	bool flipVertical);
	
bool AllocPNGImageFromMemory(
	const uint8_t* source,
	const size_t size,
	uint8_t* &pixels,
	uint32_t &width,
	uint32_t &height,
	const bool flipVertical,
	const bool powerOfTwo);


} // namespace MiniGL


#endif // _RESOURCE_IMAGE_H_