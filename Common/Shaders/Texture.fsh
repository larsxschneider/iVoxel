//
//  Shader.fsh
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

varying mediump vec2	texCoord;
uniform sampler2D		textureHandle;

void main()
{     
	gl_FragColor = texture2D(textureHandle, texCoord);
}        