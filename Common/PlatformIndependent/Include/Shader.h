/*
 *  Shader.h
 *  MacApp
 *
 *  Created by Lars Schneider on 11.06.10.
 *  Copyright 2010 Lars Schneider. All rights reserved.
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