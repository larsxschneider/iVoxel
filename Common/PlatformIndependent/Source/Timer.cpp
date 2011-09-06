/*
 *  Timer.cpp
 *  MacApp
 *
 *  Created by Lars Schneider on 04.07.10.
 *  Copyright 2010 Lars Schneider. All rights reserved.
 *
 */

#include "Timer.h"
#include "APIFactory.h"


namespace WVSClientCommon
{


double time;
	

void resetTimer()
{
	time = APIFactory::GetInstance().getTimeInMS();
}


double getElapsedTime()
{
	return (APIFactory::GetInstance().getTimeInMS() - time);
}

}