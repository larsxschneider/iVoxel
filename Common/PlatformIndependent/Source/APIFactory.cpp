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

#include "APIFactory.h"
#include <stdio.h>
#include <stddef.h>
#include "ResourceFile.h"

#ifdef CROSSPLATFORM_API_FACTORY
# include "CrossPlatformAPIFactory.h"
# define FACTORY CrossPlatformAPIFactory
#else
# ifdef __APPLE__
#  include "AppleAPIFactory.h"
#  define FACTORY AppleAPIFactory
# endif
#endif

namespace WVSClientCommon
{


APIFactory* APIFactory::_instance = NULL;


APIFactory& APIFactory::GetInstance()
{
	if (!_instance)
	{
		_instance = new FACTORY();
	}
	return *_instance;
}


void APIFactory::Destroy()
{
	// static
	delete (FACTORY*)_instance;
	_instance = NULL;
}


void APIFactory::setupResourcePath()
{
	getResourcePathASCII(_ressourcePathBuffer, 2048);
	MiniGL::CPVRTResourceFile::SetReadPath(_ressourcePathBuffer);
}
		

} // end of namespace WVSClientCommon