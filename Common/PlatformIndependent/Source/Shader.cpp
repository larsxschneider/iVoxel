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

#include "Shader.h"
#include "DebugConfig.h"
#include "PlatformOpenGL.h"

namespace WVSClientCommon
{


const char* textureShaderAttributeNames[] =
{
	"inVertex",
	"inNormal",
	"inTexCoord"
};


const char* postProcessingShaderAttributeNames[] =
{
	"inVertex"
	"inTexCoord"
};

const char* blurShaderUniformNames[] =
{
	"exelOffsetX",
	"atexelOffsetY"
};

const char* shaderUniformNames[] =
{
	"viewProjectionMatrix",
	"modelMatrix"
};


const char* pointShaderAttributeNames[] =
{
	"data1",
	"data2"
};


const char* pointShaderUniformNames[] =
{
	"regionOrigin",
	"nodeIncircleDiameter",
	"voxelIncircleDiameter",
	"voxelScreensizeCircumCircleRadius",
	"cameraPosition",
	"projectionMatrix",
	"viewMatrix",
	"inverseViewMatrix",
	"normals",
};


bool loadBlurShaderProgram(ShaderProgram &blurShaderProgram)
{
	// Load texture vertex shader
	if (MiniGL::ShaderLoadFromFile("/Blur.vsh", GL_VERTEX_SHADER, &blurShaderProgram.vertexHandle) == 0)
	{
		logError("Loading the Blur vertex shader fails\n");
		return false;
	}
	
	// Load texture fragment shader
	if (MiniGL::ShaderLoadFromFile("/Blur.fsh", GL_FRAGMENT_SHADER, &blurShaderProgram.fragmenHandle) == 0)
	{
		logError("Loading the Blur fragment shader fails\n");
		return false;
	}
	
	// Compile texture shader programs
	MiniGL::CreateProgram( &blurShaderProgram.handle, 
							blurShaderProgram.vertexHandle, 
							blurShaderProgram.fragmenHandle, 
							postProcessingShaderAttributeNames, 
							POST_PROCESSING_SHADER_ATTRIBUTE_COUNT);

	// Shader has no uniforms
	blurShaderProgram.uniforms = new GLuint[BLUR_SHADER_UNIFORM_COUNT];
	
	// Set shader uniforms
	for (int i = 0; i < BLUR_SHADER_UNIFORM_COUNT; ++i)
	{
		blurShaderProgram.uniforms[i] = glGetUniformLocation(blurShaderProgram.handle,
			blurShaderUniformNames[i]);
	}
	
	return true;
}

bool loadPostProcessingShaderProgram(ShaderProgram &postProcessingShaderProgram)
{
	// Load texture vertex shader
	if (MiniGL::ShaderLoadFromFile("/PostProcessing.vsh", GL_VERTEX_SHADER, &postProcessingShaderProgram.vertexHandle) == 0)
	{
		logError("Loading the PostProcessing vertex shader fails\n");
		return false;
	}
	
	// Load texture fragment shader
	if (MiniGL::ShaderLoadFromFile("/PostProcessing.fsh", GL_FRAGMENT_SHADER, &postProcessingShaderProgram.fragmenHandle) == 0)
	{
		logError("Loading the PostProcessing fragment shader fails\n");
		return false;
	}
	
	// Compile texture shader programs
	MiniGL::CreateProgram( &postProcessingShaderProgram.handle, 
							postProcessingShaderProgram.vertexHandle, 
							postProcessingShaderProgram.fragmenHandle, 
							postProcessingShaderAttributeNames, 
							POST_PROCESSING_SHADER_ATTRIBUTE_COUNT);

	// Shader has no uniforms
	postProcessingShaderProgram.uniforms = NULL;
	
	return true;
}


bool loadTextureShaderProgram(ShaderProgram &textureShaderProgram)
{
	// Load texture vertex shader
	if (MiniGL::ShaderLoadFromFile("/Texture.vsh", GL_VERTEX_SHADER, &textureShaderProgram.vertexHandle) == 0)
	{
		logError("Loading the texture vertex shader fails\n");
		return false;
	}
	
	// Load texture fragment shader
	if (MiniGL::ShaderLoadFromFile("/Texture.fsh", GL_FRAGMENT_SHADER, &textureShaderProgram.fragmenHandle) == 0)
	{
		logError("Loading the texture fragment shader fails\n");
		return false;
	}
	
	// Compile texture shader programs
	MiniGL::CreateProgram( &textureShaderProgram.handle, 
							textureShaderProgram.vertexHandle, 
							textureShaderProgram.fragmenHandle, 
							textureShaderAttributeNames, 
							TEXTURE_SHADER_ATTRIBUTE_COUNT);

	textureShaderProgram.uniforms = new GLuint[SHADER_UNIFORM_COUNT];
	
	// Set shader uniforms
	for (int i = 0; i < SHADER_UNIFORM_COUNT; ++i)
	{
		textureShaderProgram.uniforms[i] = glGetUniformLocation(textureShaderProgram.handle,
			shaderUniformNames[i]);
	}
	
	return true;
}


//bool loadColorShaderProgram(ShaderProgram &colorShaderProgram)
//{
//	// Load color vertex shader
//	if (MiniGL::ShaderLoadFromFile("/Color.vsh", GL_VERTEX_SHADER, &colorShaderProgram.vertexHandle) == 0)
//	{
//		logError("Loading the color vertex shader fails\n");
//		return false;
//	}
//	
//	// Load color fragment shader
//	if (MiniGL::ShaderLoadFromFile("/Color.fsh", GL_FRAGMENT_SHADER, &colorShaderProgram.fragmenHandle) == 0)
//	{
//		logError("Loading the color fragment shader fails\n");
//		return false;
//	}
//
//	// Compile color shader programs
//	MiniGL::CreateProgram( &colorShaderProgram.handle, 
//							colorShaderProgram.vertexHandle, 
//							colorShaderProgram.fragmenHandle, 
//							colorShaderAttributeNames, 
//							COLOR_SHADER_ATTRIBUTE_COUNT);
//	
//	colorShaderProgram.uniforms = new GLuint[SHADER_UNIFORM_COUNT];
//	
//	// Set color shader uniforms
//	for (int i = 0; i < SHADER_UNIFORM_COUNT; ++i)
//	{
//		colorShaderProgram.uniforms[i] = glGetUniformLocation(colorShaderProgram.handle, shaderUniformNames[i]);
//	}
//
//	return true;
//}
//
//
//bool loadEWAPointSplattingShaderProgram(ShaderProgram &pointShaderProgram)
//{
//	// Load color vertex shader
//	if (MiniGL::ShaderLoadFromFile("/EWAPointSplatting.vsh", GL_VERTEX_SHADER, &pointShaderProgram.vertexHandle) == 0)
//	{
//		logError("Loading the EWAPointSplatting vertex shader fails\n");
//		return false;
//	}
//	
//	// Load color fragment shader
//	if (MiniGL::ShaderLoadFromFile("/EWAPointSplatting.fsh", GL_FRAGMENT_SHADER, &pointShaderProgram.fragmenHandle) == 0)
//	{
//		logError("Loading the EWAPointSplatting fragment shader fails\n");
//		return false;
//	}
//
//	// Compile color shader programs
//	MiniGL::CreateProgram( &pointShaderProgram.handle, 
//							pointShaderProgram.vertexHandle, 
//							pointShaderProgram.fragmenHandle, 
//							pointShaderAttributeNames, 
//							POINT_SHADER_ATTRIBUTE_COUNT);
//
//	pointShaderProgram.uniforms = new GLuint[POINT_SHADER_UNIFORM_COUNT];
//	
//	// Set color shader uniforms
//	for (int i = 0; i < POINT_SHADER_UNIFORM_COUNT; ++i)
//	{
//		pointShaderProgram.uniforms[i] = glGetUniformLocation(pointShaderProgram.handle, pointShaderUniformNames[i]);
//	}
//	
//	return true;
//}


bool loadSimplePointSplattingShaderProgram(ShaderProgram &pointShaderProgram)
{
	// Load color vertex shader
	if (MiniGL::ShaderLoadFromFile("/SimplePointSplatting.vsh", GL_VERTEX_SHADER, &pointShaderProgram.vertexHandle) == 0)
	{
		logError("Loading the SimplePointSplatting vertex shader fails\n");
		return false;
	}
	
	// Load color fragment shader
	if (MiniGL::ShaderLoadFromFile("/SimplePointSplatting.fsh", GL_FRAGMENT_SHADER, &pointShaderProgram.fragmenHandle) == 0)
	{
		logError("Loading the SimplePointSplatting fragment shader fails\n");
		return false;
	}

	// Compile color shader programs
	MiniGL::CreateProgram( &pointShaderProgram.handle, 
							pointShaderProgram.vertexHandle, 
							pointShaderProgram.fragmenHandle, 
							pointShaderAttributeNames, 
							POINT_SHADER_ATTRIBUTE_COUNT);

	pointShaderProgram.uniforms = new GLuint[POINT_SHADER_UNIFORM_COUNT];
	
	// Set color shader uniforms
	for (int i = 0; i < POINT_SHADER_UNIFORM_COUNT; ++i)
	{
		pointShaderProgram.uniforms[i] = glGetUniformLocation(pointShaderProgram.handle, pointShaderUniformNames[i]);
	}
	
	return true;
}


bool unloadShaderProgram(ShaderProgram &shaderProgram)
{
    if (shaderProgram.handle)
    {
        glDeleteProgram(shaderProgram.handle);
        shaderProgram.handle = 0;
		shaderProgram.vertexHandle = 0;
		shaderProgram.fragmenHandle = 0;
		delete[] shaderProgram.uniforms;
		return true;
    }
	return false;
}

}