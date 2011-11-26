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
 
// On non-ES GLSL Version lower or equal 1.10 empthy/non functional precision keywords are necessary
// to compile the shader without errors (GLSL 1.10 does not support precision hint)
#ifndef GL_ES
#if (__VERSION__ <= 110)
#define lowp
#define mediump
#define highp
#endif
#endif

#define SPLATTING 0

varying		lowp		vec3	color;
#if SPLATTING
varying		highp		vec3	eyeSpaceNormal;
#endif

void main()
{
#if SPLATTING
	// pointCoord in [-0.5, 0.5]^2
	lowp vec2 pointCoord;

#ifdef GL_ES
	pointCoord = gl_PointCoord.st - 0.5;
#else
	pointCoord = gl_TexCoord[0].st - 0.5;
#endif

	highp float a = (clamp(eyeSpaceNormal.x/eyeSpaceNormal.z, -2.0, 2.0));
	highp float b = (clamp(eyeSpaceNormal.y/eyeSpaceNormal.z, -2.0, 2.0));
	
	highp float deltaZ = -a * pointCoord.x - b * pointCoord.y;

	// discard fragments that are outside the disk
	if (pointCoord.x * pointCoord.x + 
		pointCoord.y * pointCoord.y + 
		deltaZ * deltaZ > 0.25)
	{
		discard;
	}

#endif

	gl_FragColor = vec4(color, 1.0);
}


//varying float sphere_radius;
//
////varying vec3 vertex_light_position;
//varying vec4 eye_position;
//
//void main()
//{
//	vec3 vertex_light_position = vec3(10.0, 0.0, 0.0);
//
//    // r^2 = (x - x0)^2 + (y - y0)^2 + (z - z0)^2
//    float x = gl_TexCoord[0].x + 0.5;
//    float y = gl_TexCoord[0].y + 0.5;
//    float zz = 1.0 - x*x - y*y;
//
//    if (zz <= 0.0)
//        discard;
//	
//    float z = sqrt(zz);
//	
//    vec3 normal = vec3(x, y, z);
//
//    // Lighting
////    float diffuse_value = max(dot(normal, vertex_light_position), 0.0);
////	
////    vec4 pos = eye_position;
////    pos.z += z*sphere_radius;
////    pos = gl_ProjectionMatrix * pos;
//	
//   // gl_FragDepth = (pos.z / pos.w + 1.0) / 2.0;
//    gl_FragColor = vec4(255.0, 0.0, 0.0, 1.0);// * diffuse_value;
//}