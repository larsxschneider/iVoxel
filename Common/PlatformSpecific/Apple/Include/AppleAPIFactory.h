/*
 *  AppleAPIFactory.h
 *  iPhoneApp
 *
 *  Created by Lars Schneider on 21.05.10.
 *  Copyright 2010 Lars Schneider. All rights reserved.
 *
 */

#ifndef APPLE_API_FACTORY_H_DB7LVEAS
#define APPLE_API_FACTORY_H_DB7LVEAS


#include "APIFactory.h"
#include "DebugConfig.h"


namespace WVSClientCommon
{


class AppleAPIFactory : public APIFactory
{
	void						*_locksArray;
	
public:

	AppleAPIFactory();
	~AppleAPIFactory();
	
	double getTimeInMS();
	
	void getResourcePathASCII(char* const cBuffer, const int iLength, const char* const filename = NULL);
	void getCachePathASCII(char* const cBuffer, const int iLength, const char* const filename = NULL);
	
	void createLocks(const uint32_t numberOfLocks);
	void lock(const uint32_t lockID) const;
	bool tryLock(const uint32_t lockID) const;
	void unlock(const uint32_t lockID) const;
	
	void forkBackingStoreRestoreThread(Octree* const octree);
	
	void showMessage(const char* const title, const char* const message) const;
	
	void processUserInterfaceEvents() const;
	void presentRenderBuffer(void* context) const;
};


} // end of namespace WVSClientCommon


#endif /* end of include guard: APPLE_API_FACTORY_H_DB7LVEAS */
