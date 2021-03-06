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

#include "Plane.h"


namespace MiniGL
{
	
	
Plane::Plane(GLuint textureHandle, bool centered)
: Mesh(4, GL_TRIANGLE_STRIP, true, true)
{
	if (centered)
	{
		setVertex(0, +0.5f, -0.5f, 0.0f);
		setVertex(1, +0.5f, +0.5f, 0.0f);
		setVertex(2, -0.5f, -0.5f, 0.0f);
		setVertex(3, -0.5f, +0.5f, 0.0f);
	}
	else
	{
		setVertex(0, 1.0f, 0.0f, 0.0f);
		setVertex(1, 1.0f, 1.0f, 0.0f);
		setVertex(2, 0.0f, 0.0f, 0.0f);
		setVertex(3, 0.0f, 1.0f, 0.0f);
	}
	
	for (int i=0; i<3; i++)
	{
		setNormal(i, 0.0f, 0.0f, 1.0f);
	}
	
	setUV(0, 1.0f, 0.0f);
	setUV(1, 1.0f, 1.0f);
	setUV(2, 0.0f, 0.0f);
	setUV(3, 0.0f, 1.0f);
	
	createVertexBufferObjects();
	
	createTextureHandleArray(1);
	setTextureHandle(0, textureHandle);
}

	
Plane::~Plane()
{
}

	
} // namespace MiniGL
