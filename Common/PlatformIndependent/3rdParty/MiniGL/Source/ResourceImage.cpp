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

#include "ResourceImage.h"
#include "ResourceFile.h"
#include "png.h" // Attention: Requires zlib!
#include "jpeglib.h"
#include "jerror.h"
#include <iostream>
#include <sstream>
#include <string>
#include <ostream>


namespace MiniGL
{

struct my_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */

  jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;


METHODDEF(void)
minigl_error_exit(j_common_ptr cinfo)
{  
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = (my_error_ptr) cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  (*cinfo->err->output_message) (cinfo);

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}


bool AllocJPEGImageFromMemory(
	const uint8_t* source,
	const size_t size,
	uint8_t* &pixels,
	uint32_t &width,
	uint32_t &height,
	const bool flipVertical,
	const bool powerOfTwo)
{
	struct jpeg_decompress_struct cinfo;
	struct my_error_mgr jerr;
	
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = minigl_error_exit;
	
	/* Establish the setjmp return context for my_error_exit to use. */
    if (setjmp(jerr.setjmp_buffer))
    {
        /* If we get here, the JPEG code has signaled an error.
         * We need to clean up the JPEG object, close the input file, and return.
         */
        jpeg_destroy_decompress(&cinfo);
        return false;
    }
	
	jpeg_create_decompress(&cinfo);
	jpeg_mem_src(&cinfo, (unsigned char*)source, size);
		  
	jpeg_read_header(&cinfo, TRUE);

	jpeg_start_decompress(&cinfo);

    uint32_t jpegWidth = cinfo.output_width;
    uint32_t jpegHeight = cinfo.output_height;
	int components = cinfo.num_components;
	
	width = jpegWidth;
	height = jpegHeight;
	
	if (powerOfTwo)
	{
		// width and height are the power of 2 versions
		// iPhone requires power of two textures
		width = GetPowerOfTwoInteger(jpegWidth);
		height = GetPowerOfTwoInteger(jpegHeight);
	}
	
	// Check for right number of components (we expect RGB)
	if (components != 3) return false;
	
	// Check for right color space
	if (cinfo.out_color_space != JCS_RGB) return false;
	
	int x = 0;
	int y = 0;
	JSAMPROW row_pointer[1];
	
	pixels = (uint8_t*)malloc(width * height * components);
	row_pointer[0] = (uint8_t *)malloc(jpegWidth * components);
	
	while (cinfo.output_scanline < jpegHeight)
	{
		jpeg_read_scanlines(&cinfo, row_pointer, 1);
		if (flipVertical)
		{
			for (x=0; x < jpegWidth * components; x++)
			{
				pixels[(height-y-1) * width * components + x] = row_pointer[0][x];
			}
		}
		else
		{
			for (x=0; x < jpegWidth * components; x++)
			{
				pixels[y * width * components + x] = row_pointer[0][x];
			}
		}
		y++;
	}

	jpeg_finish_decompress( &cinfo );
	jpeg_destroy_decompress( &cinfo );

	free(row_pointer[0]);
	
	return true;
}


// Based on http://github.com/newhouseb/screenbytes/blob/master/image_functions.c
bool AllocJPEGImageFromFile(
	const char* const filename,
	uint8_t* &pixels,
	uint32_t &width,
	uint32_t &height,
	bool flipVertical)
{
	string filenameWithPath(CPVRTResourceFile::GetReadPath());
	filenameWithPath += filename;
	
	FILE* fp = fopen(filenameWithPath.c_str(), "rb");
	if (!fp) return false;
	
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, fp);
	jpeg_read_header(&cinfo, TRUE);

	jpeg_start_decompress(&cinfo);

    width = cinfo.output_width;
    height = cinfo.output_height;
	int components = cinfo.num_components;
	
	// Check for right number of components (we expect RGB)
	if (components != 3) return false;
	
	// Check for right color space
	if (cinfo.out_color_space != JCS_RGB) return false;
	
	int x = 0;
	int y = 0;
	JSAMPROW row_pointer[1];
	
	pixels = (uint8_t*)malloc(width * height * components);
	row_pointer[0] = (uint8_t *)malloc(width * components);
	
	while (cinfo.output_scanline < height)
	{
		jpeg_read_scanlines(&cinfo, row_pointer, 1);
		if (flipVertical)
		{
			for (x=0; x < width * components; x++)
			{
				pixels[(height-y-1) * width * components + x] = row_pointer[0][x];
			}
		}
		else
		{
			for (x=0; x < width * components; x++)
			{
				pixels[y * width * components + x] = row_pointer[0][x];
			}
		}

		y++;
	}

	jpeg_finish_decompress( &cinfo );
	jpeg_destroy_decompress( &cinfo );

	free(row_pointer[0]);
	fclose(fp);
	
	return true;
}


void PNGReadData(png_structp pngPtr, png_bytep data, png_size_t length)
{
    //Here we get our IO pointer back from the read struct.
    //This is the parameter we passed to the png_set_read_fn() function.
    //Our std::istream pointer.
	std::istream *stream = (std::istream*)png_get_io_ptr(pngPtr);
	stream->read((char*)data, length);
}


uint32_t GetPowerOfTwoInteger(uint32_t intValue)
{
	unsigned int i = 0;
	if ((intValue != 1) && (intValue & (intValue - 1)))
	{
		i = 1;
		while (i < intValue) i *= 2;
		intValue = i;
	}
	return intValue;
}


#define PNG_SIG_BYTES 8
bool AllocPNGImageFromMemory(
	const uint8_t* source,
	const size_t size,
	uint8_t* &pixels,
	uint32_t &width,
	uint32_t &height,
	const bool flipVertical,
	const bool powerOfTwo)
{
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) return false;
	
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
        return false;
    }

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)0);
		//if (rowPtrs != NULL) delete [] rowPtrs;
		//if (data != NULL) delete [] data;
        return false;
    }
	
	std::string string((char*)source, size);
	std::stringbuf buffer(string, ios::in);
	std::istream stream(&buffer);

	png_set_read_fn(png_ptr, (voidp)&stream, PNGReadData);
	png_set_sig_bytes(png_ptr, 0);
	png_read_info(png_ptr, info_ptr);
	
	uint32_t pngWidth = png_get_image_width(png_ptr, info_ptr);
	uint32_t pngHeight = png_get_image_height(png_ptr, info_ptr);
	
	int bit_depth, color_type;
	bit_depth = png_get_bit_depth(png_ptr, info_ptr);
	color_type = png_get_color_type(png_ptr, info_ptr);

	if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png_ptr);
	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_expand_gray_1_2_4_to_8(png_ptr);
	if (png_get_valid( png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_tRNS_to_alpha (png_ptr);
	
	if (bit_depth == 16) png_set_strip_16(png_ptr);
	else if(bit_depth < 8) png_set_packing( png_ptr );
	
	png_read_update_info(png_ptr, info_ptr);

	png_get_IHDR(png_ptr, info_ptr,
				 &pngWidth, &pngHeight, &bit_depth, &color_type,
				 NULL, NULL, NULL);
	int bits = 0;
	switch(color_type)
	{
		case PNG_COLOR_TYPE_GRAY:
			bits = 1;
			break;
			
		case PNG_COLOR_TYPE_GRAY_ALPHA:
			bits = 2;
			break;
			
		case PNG_COLOR_TYPE_RGB:
			bits = 3;
			break;
			
		case PNG_COLOR_TYPE_RGB_ALPHA:
			bits = 4;
			break;
	}

	width = pngWidth;
	height = pngHeight;
	
	if (powerOfTwo)
	{
		// width and height are the power of 2 versions of pngWidth and pngHeight
		// iPhone requires power of two textures
		width = GetPowerOfTwoInteger(pngWidth);
		height = GetPowerOfTwoInteger(pngHeight);
	}
	
	pixels = (png_byte*)calloc(width * height * bits, sizeof(png_byte));
	png_byte** row_ptrs = (png_byte**)malloc(height * sizeof(png_bytep)); //TODO: fehlen hier bits?
	
	if (flipVertical)
	{
		for (int i=0; i<height; i++) row_ptrs[height-i-1] = pixels + i*width*bits;
	}
	else
	{
		for (int i=0; i<height; i++) row_ptrs[i] = pixels + i*width*bits;
	}

	png_read_image(png_ptr, row_ptrs);	
	png_read_end(png_ptr, NULL);
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)0);
	free(row_ptrs);

	return true;
}


bool AllocPNGImageFromFile(
	const char* const filename,
	uint8_t *&pixels,
	uint32_t &width,
	uint32_t &height,
	bool flipVertical)
{
	string filenameWithPath(CPVRTResourceFile::GetReadPath());
	filenameWithPath += filename;
	
	FILE* fp = fopen(filenameWithPath.c_str(), "rb");
	if (!fp) return false;

	int bits = 0;
	uint8_t header[PNG_SIG_BYTES];
	fread(header, 1, PNG_SIG_BYTES, fp);
	// Check if header is PNG
	if (png_sig_cmp(header, 0, PNG_SIG_BYTES)) return false;

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) return false;
	
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
        return false;
    }

    png_infop end_info = png_create_info_struct(png_ptr);
    if (!end_info)
    {
        png_destroy_read_struct(&png_ptr, &info_ptr,
          (png_infopp)NULL);
        return false;
    }

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_read_struct(&png_ptr, &info_ptr,
           &end_info);
        fclose(fp);
        return NULL;
    }
	
	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, PNG_SIG_BYTES);
	png_read_info(png_ptr, info_ptr);
	
	width = png_get_image_width(png_ptr, info_ptr);
	height = png_get_image_height(png_ptr, info_ptr);
	
	int bit_depth, color_type;
	bit_depth = png_get_bit_depth(png_ptr, info_ptr);
	color_type = png_get_color_type(png_ptr, info_ptr);

	if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png_ptr);
	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_expand_gray_1_2_4_to_8(png_ptr);
	if (png_get_valid( png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_tRNS_to_alpha (png_ptr);
	
	if (bit_depth == 16) png_set_strip_16(png_ptr);
	else if(bit_depth < 8) png_set_packing( png_ptr );
	
	png_read_update_info(png_ptr, info_ptr);

	png_get_IHDR(png_ptr, info_ptr,
				 &width, &height, &bit_depth, &color_type,
				 NULL, NULL, NULL);
	
	switch(color_type)
	{
		case PNG_COLOR_TYPE_GRAY:
			bits = 1;
			break;
			
		case PNG_COLOR_TYPE_GRAY_ALPHA:
			bits = 2;
			break;
			
		case PNG_COLOR_TYPE_RGB:
			bits = 3;
			break;
			
		case PNG_COLOR_TYPE_RGB_ALPHA:
			bits = 4;
			break;
	}

	pixels = (png_byte*)calloc(width * height * bits, sizeof(png_byte));
	png_byte** row_ptrs = (png_byte**)malloc(height * sizeof(png_bytep));
	
	if (flipVertical)
	{
		for (int i=0; i<height; i++) row_ptrs[height-i-1] = pixels + i*width*bits;
	}
	else
	{
		for (int i=0; i<height; i++) row_ptrs[i] = pixels + i*width*bits;
	}

	png_read_image(png_ptr, row_ptrs);	
	png_read_end(png_ptr, NULL);
	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
	free(row_ptrs);
	
	fclose(fp);
	return true;
}


} // namespace MiniGL
