/*
 *  AppleAPIFactory.cpp
 *  iPhoneApp
 *
 *  Created by Lars Schneider on 21.05.10.
 *  Copyright 2010 Lars Schneider. All rights reserved.
 *
 */

#include "AppleAPIFactory.h"
#import <QuartzCore/QuartzCore.h>
#import <mach/mach.h> 
#import <mach/mach_host.h>

#if (!TARGET_IPHONE_SIMULATOR && !TARGET_OS_IPHONE && TARGET_OS_MAC)
#import <Cocoa/Cocoa.h>
#endif

namespace WVSClientCommon
{


// see http://stackoverflow.com/questions/2798638/available-memory-for-iphone-os-app
natural_t  freeMemory(void)
{
	mach_port_t					host_port = mach_host_self();
	mach_msg_type_number_t		host_size = sizeof(vm_statistics_data_t) / sizeof(integer_t);
	vm_size_t					pagesize;
	vm_statistics_data_t		vm_stat;

	host_page_size(host_port, &pagesize);

	if (host_statistics(host_port, HOST_VM_INFO, (host_info_t)&vm_stat, &host_size) != KERN_SUCCESS) NSLog(@"Failed to fetch vm statistics");

	//natural_t   mem_used = (vm_stat.active_count + vm_stat.inactive_count + vm_stat.wire_count) * pagesize;
	natural_t   mem_free = vm_stat.free_count * pagesize;
	//natural_t   mem_total = mem_used + mem_free;

	return mem_free;
}


AppleAPIFactory::AppleAPIFactory() : APIFactory()
{
	
#if 0
	// prints OpenGL Extensions on Apple platform
	NSString *extensionString = [NSString stringWithUTF8String:(char *)glGetString(GL_EXTENSIONS)];
    NSArray *extensions = [extensionString componentsSeparatedByCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
    for (NSString *oneExtension in extensions) NSLog(oneExtension);
#endif
	
	NSLog(@"Avialable memory: %.2f MB", float(freeMemory()) / (1024.0f * 1024.0f));
	
	_locksArray = nil;
}


AppleAPIFactory::~AppleAPIFactory()
{
	[(NSMutableArray*)_locksArray release];
}


double AppleAPIFactory::getTimeInMS()
{
	// see https://devforums.apple.com/message/118806
	double time = CACurrentMediaTime() * 1000.0;
	//NSLog(@"Avialable memory: %i byte", freeMemory());
	
	return time;
}


void AppleAPIFactory::getResourcePathASCII(char* const cBuffer, const int iLength, const char* const filename)
{
	NSString* readPath = [[NSBundle mainBundle] resourcePath];
	
	if (filename)
	{
		readPath = [readPath stringByAppendingPathComponent:
			[NSString stringWithCString:filename encoding:NSASCIIStringEncoding]];
	}
	
	[readPath getCString:cBuffer maxLength:iLength encoding:NSASCIIStringEncoding];
}


// see http://developer.apple.com/iphone/library/documentation/iphone/conceptual/iphoneosprogrammingguide/RuntimeEnvironment/RuntimeEnvironment.html
void AppleAPIFactory::getCachePathASCII(char* const cBuffer, const int iLength, const char* const filename)
{
	NSArray* paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
	NSString* documentsDirectoryPath = [paths objectAtIndex:0];
	NSString* path = documentsDirectoryPath;
	
	if (filename)
	{
		path = [documentsDirectoryPath stringByAppendingPathComponent:
			[NSString stringWithCString:filename encoding:NSASCIIStringEncoding]];
	}

	[path getCString:cBuffer maxLength:iLength encoding:NSASCIIStringEncoding];
}


void AppleAPIFactory::createLocks(const uint32_t numberOfLocks)
{
	[(NSMutableArray*)_locksArray release];
	NSMutableArray* locks = [[NSMutableArray alloc] init];
	NSLock* lock;
	
	for (uint32_t i = 0; i < numberOfLocks; ++i)
	{
		lock = [[NSLock alloc] init];
		[locks addObject:lock];
		[lock release];
	}
	
	_locksArray = locks;
}


void AppleAPIFactory::lock(const uint32_t lockID) const
{
	assert(lockID < [(NSMutableArray*)_locksArray count]);
	[[(NSMutableArray*)_locksArray objectAtIndex:lockID] lock];
}


bool AppleAPIFactory::tryLock(const uint32_t lockID) const
{
	assert(lockID < [(NSMutableArray*)_locksArray count]);
	return [[(NSMutableArray*)_locksArray objectAtIndex:lockID] tryLock];
}


void AppleAPIFactory::unlock(const uint32_t lockID) const
{
	assert(lockID < [(NSMutableArray*)_locksArray count]);
	[[(NSMutableArray*)_locksArray objectAtIndex:lockID] unlock];
}


void AppleAPIFactory::showMessage(const char* const title, const char* const message) const
{
#if (TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE)
	UIAlertView* alert = [[UIAlertView alloc]
		initWithTitle:[NSString stringWithCString:title encoding:NSASCIIStringEncoding]
		message:[NSString stringWithCString:message encoding:NSASCIIStringEncoding]
		delegate:nil
		cancelButtonTitle:@"OK"
		otherButtonTitles:nil];
	[alert show];
	[alert release];
#elif (!TARGET_IPHONE_SIMULATOR && !TARGET_OS_IPHONE && TARGET_OS_MAC)
	NSAlert* alert = [NSAlert alertWithMessageText:[NSString stringWithCString:title encoding:NSASCIIStringEncoding]
		defaultButton:@"OK"
		alternateButton:nil
		otherButton:nil
		informativeTextWithFormat:[NSString stringWithCString:message encoding:NSASCIIStringEncoding]];
	[alert runModal];
#endif
}


void AppleAPIFactory::processUserInterfaceEvents() const
{
#if (TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE)
	while(CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0.002, TRUE) == kCFRunLoopRunHandledSource);
#elif (!TARGET_IPHONE_SIMULATOR && !TARGET_OS_IPHONE && TARGET_OS_MAC)
	 NSEvent* event = [NSApp nextEventMatchingMask:NSAnyEventMask untilDate:nil inMode:NSDefaultRunLoopMode dequeue:YES];	 
	 if (event) [NSApp sendEvent:event];
	 sleep(0.001);
#endif
}


void AppleAPIFactory::presentRenderBuffer(void* context) const
{
#if (TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE)
	[(EAGLContext*)context presentRenderbuffer:GL_RENDERBUFFER];
#elif (!TARGET_IPHONE_SIMULATOR && !TARGET_OS_IPHONE && TARGET_OS_MAC)
	[context flushBuffer];
#endif
}
	
			
void* runBackingStoreRestoreThread(void* octree)
{
	//((Octree*)octree)->runBackingStoreRestoreThread();
    return NULL;
}


void AppleAPIFactory::forkBackingStoreRestoreThread(Octree* const octree)
{
	// see http://developer.apple.com/mac/library/documentation/cocoa/conceptual/Multithreading/CreatingThreads/CreatingThreads.html
	// Create the thread using POSIX routines.
    pthread_attr_t  attr;
    pthread_t       posixThreadID;
    int             returnVal;

 
    returnVal = pthread_attr_init(&attr);
    assert(!returnVal);
	
	returnVal = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
	assert(!returnVal);
	
    returnVal = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    assert(!returnVal);
 
    int     threadError = pthread_create(	&posixThreadID,
											&attr, 
											&runBackingStoreRestoreThread,
											octree);
 
    returnVal = pthread_attr_destroy(&attr);
    assert(!returnVal);
	
    if (threadError != 0)
    {
         logError("Backing store thread fork failed.");
    }
}

	
} // end of namespace WVSClientCommon

