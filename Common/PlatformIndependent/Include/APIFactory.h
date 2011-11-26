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

/*
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

 
