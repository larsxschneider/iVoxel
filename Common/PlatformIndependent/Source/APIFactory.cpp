/*
 *  APIFactory.cpp
 *  iPhoneApp
 *
 *  Created by Lars Schneider on 21.05.10.
 *  Copyright 2010 Lars Schneider. All rights reserved.
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