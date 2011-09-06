//
//  Shader.vsh
//
//  Created by Lars Schneider on 19.05.10.
//  Copyright Lars Schneider 2010. All rights reserved.
//

// On non-ES GLSL Version lower or equal 1.10 empthy/non functional precision keywords are necessary
// to compile the shader without errors (GLSL 1.10 does not support precision hint)
#ifndef GL_ES
#if (__VERSION__ <= 110)
#define lowp
#define mediump
#define highp
#endif
#endif


attribute	highp		vec2	inVertex;
attribute	mediump		vec2	inTexCoord;

varying mediump vec2   TexCoord;

void main()
{
    // Pass through vertex
	gl_Position = vec4(inVertex, 0.0, 1.0);
	
	// Pass through texcoords
	TexCoord = inTexCoord;
}
