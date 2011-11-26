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
