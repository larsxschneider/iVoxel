/*
 *  APIFactory.h
 *  iPhoneApp
 *
 *  Created by Lars Schneider on 21.05.10.
 *  Copyright 2010 Lars Schneider. All rights reserved.
 *
 *	Based on:
 *	Cross-Platform Development in C++, Syd Logan (p27ff)
 *	http://de.wikipedia.org/wiki/Singleton_(Entwurfsmuster)#Implementierung_in_C.2B.2B
 *
 *	ATTENTION: This implementation is not thread safe!
 */

#ifndef API_FACTORY_V3J8GFDS
#define API_FACTORY_V3J8GFDS


#include "MiniGL.h"
#include "Octree.h"


namespace WVSClientCommon
{


class APIFactory
{
	// Pointer to instance
	static APIFactory*			_instance;
	char						_ressourcePathBuffer[2048];
	
	APIFactory(const APIFactory& cc);
	
protected:
	APIFactory() {};
	
	
public:
	static APIFactory& GetInstance();
	static void Destroy();
	virtual double getTimeInMS() = 0;
	void setupResourcePath();
	
	virtual void getResourcePathASCII(char* const cBuffer, const int iLength, const char* const filename = NULL) = 0;
	virtual void getCachePathASCII(char* const cBuffer, const int iLength, const char* const filename = NULL) = 0;
	
	virtual void createLocks(const uint32_t numberOfLocks) = 0;
	virtual void lock(const uint32_t lockID) const = 0;
	virtual bool tryLock(const uint32_t lockID) const = 0;
	virtual void unlock(const uint32_t lockID) const = 0;
	
	virtual void forkBackingStoreRestoreThread(Octree* const octree) = 0;
	
	virtual void showMessage(const char* const title, const char* const message) const = 0;
	
	virtual void processUserInterfaceEvents() const = 0;
	virtual void presentRenderBuffer(void* context) const = 0;
};



} // end of namespace WVSClientCommon

#endif /* end of include guard: API_FACTORY_V3J8GFDS */

 
