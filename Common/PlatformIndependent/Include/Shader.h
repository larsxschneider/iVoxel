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

#ifndef SHADER_H_V2Q20ZVP
#define SHADER_H_V2Q20ZVP


#include "MiniGL.h"


namespace WVSClientCommon
{


enum TextureShaderAttributes
{
	TEXTURE_SHADER_ATTRIBUTE_VERTEX_ARRAY,
	TEXTURE_SHADER_ATTRIBUTE_NORMAL_ARRAY, 
	TEXTURE_SHADER_ATTRIBUTE_TEXCOORD_ARRAY,
	TEXTURE_SHADER_ATTRIBUTE_COUNT
};


enum PostProcessingShaderAttributes
{
	POST_PROCESSING_SHADER_ATTRIBUTE_VERTEX_ARRAY,
	POST_PROCESSING_SHADER_ATTRIBUTE_TEXCOORD_ARRAY,
	POST_PROCESSING_SHADER_ATTRIBUTE_COUNT
};


enum BlurShaderUniforms
{
	BLUR_SHADER_UNIFORM_TEXEL_OFFSET_X,
	BLUR_SHADER_UNIFORM_TEXEL_OFFSET_Y,
	BLUR_SHADER_UNIFORM_COUNT
};


enum ShaderUniforms
{
	SHADER_UNIFORM_VIEW_PROJECTION_MATRIX,
	SHADER_UNIFORM_MODEL_MATRIX, 
	SHADER_UNIFORM_COUNT
};


enum PointShaderAttributes
{
	POINT_SHADER_ATTRIBUTE_DATA1,
	POINT_SHADER_ATTRIBUTE_DATA2,
	POINT_SHADER_ATTRIBUTE_COUNT
};


enum PointShaderUniforms
{
	POINT_SHADER_UNIFORM_REGION_ORIGIN,
	POINT_SHADER_UNIFORM_NODE_INCIRCLE_DIAMETER,
	POINT_SHADER_UNIFORM_VOXEL_INCIRCLE_DIAMETER,
	POINT_SHADER_UNIFORM_VOXEL_SCREENSIZE_CIRCUMCIRCLE_RADIUS,
	POINT_SHADER_UNIFORM_CAMERA_POSITION,
	POINT_SHADER_UNIFORM_PROJECTION_MATRIX,
	POINT_SHADER_UNIFORM_VIEW_MATRIX,
	POINT_SHADER_UNIFORM_INVERSE_VIEW_MATRIX,
	POINT_SHADER_UNIFORM_NORMALS_ARRAY,
	POINT_SHADER_UNIFORM_COUNT
};


struct ShaderProgram
{
	GLuint handle;
	GLuint vertexHandle;
	GLuint fragmenHandle;
	GLuint *uniforms;
};


bool loadTextureShaderProgram(ShaderProgram &textureShaderProgram);
bool loadPostProcessingShaderProgram(ShaderProgram &postProcessingShaderProgram);
bool loadBlurShaderProgram(ShaderProgram &blurShaderProgram);
bool loadSimplePointSplattingShaderProgram(ShaderProgram &pointShaderProgram);

bool unloadShaderProgram(ShaderProgram &shaderProgram);

}


#endif /* end of include guard: SHADER_H_V2Q20ZVP */