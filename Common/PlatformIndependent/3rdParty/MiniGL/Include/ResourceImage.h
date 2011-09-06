/*
 *  ResourceImage.h
 *  iPhoneApp
 *
 *  Created by Lars Schneider on 26.05.10.
 *  Copyright 2010 Lars Schneider. All rights reserved.
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