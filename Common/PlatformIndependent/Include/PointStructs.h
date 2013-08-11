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

#ifndef POINT_STRUCTS_H
#define POINT_STRUCTS_H


namespace WVSClientCommon
{


typedef struct
{
	uint8_t red;
    uint8_t green;
	uint8_t blue;
} RGBColor;


/**
	WVSPoints are generated by the WVSCodec and consumed by the Octree
 */
struct WVSPoint
{
	VECTOR3 position;
	float_t radius;
	RGBColor color;
};



/**
	QuantPoints are used in the Octree internally (plus they are swapped to backing store)
 */
struct QuantPoint
{
	typedef uint16_t PositionRColorBlock;       //  9bit position + 7bit color
	typedef uint16_t GBColorBlock;              // 16bit color

	PositionRColorBlock	positionColor;          // zzzyyyxxxrrrrrrr
	GBColorBlock        color;                  // ggggggggbbbbbbbb
	
	inline const uint16_t getPosition() const
	{
		static const uint16_t positionMask = (1 << 16) - (1 << 7);
		return (positionColor & positionMask);
	}
};


/**
	GPUPoints are generated by the Octree and consumed by the GPU
 */
struct GPUPoint
{
		uint32_t	cellPositionNormalColor;	// *bbbbbggg|ggrrrrr|zzzyyyxx|xnnnnnnn
		uint32_t	nodePositionLevel;			// LLLLLZZZ|ZZZZZZYY|YYYYYYYX|XXXXXXXX
};


}

#endif // POINT_STRUCTS_H