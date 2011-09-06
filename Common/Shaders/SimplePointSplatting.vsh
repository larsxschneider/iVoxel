//#version 120
//
//  Shader.vsh
//
//  Created by Lars Schneider on 19.05.10.
//  Copyright Lars Schneider 2010. All rights reserved.
//

// Force all output variables to be invariant
#pragma STDGL invariant(all)

// GLSL optimizations (MAD - Multiply than Add)
//see http://www.opengl.org/wiki/GLSL_%3a_common_mistakes
//see http://developer.apple.com/iphone/library/documentation/3DDrawing/Conceptual/OpenGLES_ProgrammingGuide/Performance/Performance.html#//apple_ref/doc/uid/TP40008793-CH105-SW10

// On non-ES GLSL Version lower or equal 1.10 empthy/non functional precision keywords are necessary
// to compile the shader without errors (GLSL 1.10 does not support precision hint)
#ifndef GL_ES
#if (__VERSION__ <= 110)
#define lowp
#define mediump
#define highp
#endif
#endif

#define OCTREE_LEAF_LEVEL 15
#define BACKFACE_CULLING 0 // Only possible if point has normal
#define SPLATTING 0

attribute	highp		vec4		data1;
attribute	highp		vec4		data2;

uniform		mediump		vec3		normals[128];
uniform		highp		vec3		regionOrigin[OCTREE_LEAF_LEVEL + 1];
uniform		highp		float		nodeIncircleDiameter[OCTREE_LEAF_LEVEL + 1];
uniform		highp		float		voxelIncircleDiameter[OCTREE_LEAF_LEVEL + 1];
uniform		highp		float		voxelScreensizeCircumCircleRadius[OCTREE_LEAF_LEVEL + 1];

uniform		highp		vec3		cameraPosition;
uniform		highp		mat4		projectionMatrix;
uniform		highp		mat4		viewMatrix;
uniform		highp		mat4		inverseViewMatrix;

varying		lowp		vec3		color;

#if SPLATTING
varying		highp		vec3		eyeSpaceNormal;
#endif


void main()
{
	// Decode relative voxel position bit pattern
	// data1.x = x-------
	// data1.y = zzzyyyxx
	highp vec3 relativeVoxelPosition = vec3(
		mod(data1.y, 4.0) * 2.0 + floor(data1.x * 0.0078125),
		mod(floor(data1.y * 0.25), 8.0),
		mod(floor(data1.y * 0.03125), 8.0));

	// Decode region level bit pattern
	// data2.w = LLLLLLLL
	lowp int level = int(data2.w);
	
	// Decode node position bit pattern
	// data2.x = XXXXXXXX
	// data2.y = YYYYYYYY
	// data2.z = ZZZZZZZZ
	// data2.w = --------
	highp vec4 position = vec4(data2.x, data2.y, data2.z, 1.0);
		
	// TODO: Try to keep it scalar as long as possible.
	// see POWERVR SGX OpenGL ES 2.0 Application Development, page 22
	position.xyz =	nodeIncircleDiameter[level] * position.xyz + 
					relativeVoxelPosition * voxelIncircleDiameter[level] + 
					regionOrigin[level];

#if BACKFACE_CULLING
	// Decode normal bit pattern
	// data1.x = -nnnnnnn
	highp vec3 normal = normals[int(floor(mod(data1.x, 128.0)))];	
	
	// Determine front/back faces (see Koppitz Diplomarbeit, page 34/35)								
	if (dot(normal, position.xyz - cameraPosition) > 0.0)
	{
		// Backfacing voxel
		// Move the voxel behind the camera to cull it before fragment shader stage
		// discard operation is not recommended
		// (see POWERVR SGX OpenGL ES 2.0 Application Development, page 22)
		gl_Position = vec4(0.0, 0.0, 0.0, 0.0);
		gl_PointSize = 0.0;	
	}
	else
#endif
	{
		// Frontfacing voxel
		
		// Decode color bit pattern
		// data1.z = gggrrrrr
		// data1.w = -bbbbbgg
		//TODO check if data1.w = -ggbbbbb is more efficent?! (same with position)
		color = vec3(	mod(floor(data1.z), 32.0),
						8.0 * mod(data1.w, 4.0) + floor(data1.z * 0.03125),
						mod(floor(data1.w * 0.25), 32.0))
						* 0.032258064516129;	// = 1/31
						
		// Transform world space to camera space
		// TODO: Sparse Matrices
		// see optimization POWERVR SGX OpenGL ES 2.0 Application Development, page 21
		highp vec4 cameraSpacePosition = viewMatrix * position;

		// Project eye space
		gl_Position = projectionMatrix * cameraSpacePosition;

		#if SPLATTING
		// Eyespace normal should work because we do not have non-uniform scaling
		// http://www.lighthouse3d.com/opengl/glsl/index.php?normalmatrix
		eyeSpaceNormal = mat3(viewMatrix) * normal;
		gl_PointSize = voxelScreensizeCircumCircleRadius[level] / -cameraSpacePosition.z + 1.5;

		#else
		gl_PointSize = voxelScreensizeCircumCircleRadius[level] / -cameraSpacePosition.z + 1.5;
		#endif
	}
}
