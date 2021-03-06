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

#include "Skybox.h"


namespace MiniGL
{
	
	
Skybox::Skybox(float scale, bool adjustUV, int textureSize, GLuint* textureHandle, bool withGroundPlane)
: Mesh(24, GL_TRIANGLE_STRIP, true, true)
{
	_planes = (withGroundPlane ? 6 : 5);
	
	GLfloat unit = 1.0f;
	GLfloat a0 = 0, a1 = unit;
	
	if (adjustUV)
	{
		GLfloat oneover = 1.0f / (GLfloat)textureSize;
		a0 = 4.0f * oneover;
		a1 = unit - a0;
	}
	
	unit = unit * scale;
	
	// Front
	setVertex(0, -unit, +unit, -unit);
	setVertex(1, +unit, +unit, -unit);
	setVertex(2, -unit, -unit, -unit);
	setVertex(3, +unit, -unit, -unit);
	setUV(0, a0, a1);
	setUV(1, a1, a1);
	setUV(2, a0, a0);
	setUV(3, a1, a0);
	
	// Right
	setVertex(4, +unit, +unit, -unit);
	setVertex(5, +unit, +unit, +unit);
	setVertex(6, +unit, -unit, -unit);
	setVertex(7, +unit, -unit, +unit);
	setUV(4, a0, a1);
	setUV(5, a1, a1);
	setUV(6, a0, a0);
	setUV(7, a1, a0);
	
	// Back
	setVertex(8 , +unit, +unit, +unit);
	setVertex(9 , -unit, +unit, +unit);
	setVertex(10, +unit, -unit, +unit);
	setVertex(11, -unit, -unit, +unit);
	setUV(8 , a0, a1);
	setUV(9 , a1, a1);
	setUV(10, a0, a0);
	setUV(11, a1, a0);
	
	// Left
	setVertex(12, -unit, +unit, +unit);
	setVertex(13, -unit, +unit, -unit);
	setVertex(14, -unit, -unit, +unit);
	setVertex(15, -unit, -unit, -unit);
	setUV(12, a0, a1);
	setUV(13, a1, a1);
	setUV(14, a0, a0);
	setUV(15, a1, a0);
	
	// Top
	setVertex(16, -unit, +unit, +unit);
	setVertex(17, +unit, +unit, +unit);
	setVertex(18, -unit, +unit, -unit);
	setVertex(19, +unit, +unit, -unit);
	setUV(16, a0, a1);
	setUV(17, a1, a1);
	setUV(18, a0, a0);
	setUV(19, a1, a0);
	
	// Bottom
	setVertex(20, -unit, -unit, -unit);
	setVertex(21, +unit, -unit, -unit);
	setVertex(22, -unit, -unit, +unit);
	setVertex(23, +unit, -unit, +unit);
	setUV(20, a0, a1);
	setUV(21, a1, a1);
	setUV(22, a0, a0);
	setUV(23, a1, a0);
	
	createVertexBufferObjects();
	
	createTextureHandleArray(_planes);
	for (int i = 0; i < _planes; ++i) setTextureHandle(i, textureHandle[i]);
}

	
Skybox::~Skybox()
{
}


void Skybox::render()
{
	for (int i = 0; i < _planes; i++)
	{
		renderWithIndexBuffer(GL_FALSE, 0, 0, i*4, 4, i);
	}
	
	
}
	
} // namespace MiniGL
