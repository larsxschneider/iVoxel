/**
 * Cross Platform API Factory
 * @author Robert Timm <robert.timm@studen.hpi.uni-potsdam.de>
 */

#ifdef CROSSPLATFORM_API_FACTORY

#ifndef CROSSPLATFORMAPIFACTORY_H
#define CROSSPLATFORMAPIFACTORY_H 1

#include "CrossPlatformHelper.h"

#include "APIFactory.h"
#include "DebugConfig.h"
#include "HttpMultipartParser.h"

#include <vector>

#include <OpenThreads/Mutex>
#include <OpenThreads/Thread>
#include <curl/curl.h>

namespace WVSClientCommon {
/* forward declare the curl call backs */
size_t curl_header_func(void*, size_t, size_t, void*);
size_t curl_write_func(void*, size_t, size_t, void*);

class CrossPlatformAPIFactory : public APIFactory {
  /* curls call backs need to know about the CurlRequestThread type */
  friend size_t curl_header_func(void*, size_t, size_t, void*);
  friend size_t curl_write_func(void*, size_t, size_t, void*);

  struct BackingStoreRestoreThread : public OpenThreads::Thread {
    Octree* _octree;
    void run();
  };

  struct CurlRequestThread : public OpenThreads::Thread {
    /* curls call backs need to access the CurlRequestThread type */
    friend size_t curl_header_func(void*, size_t, size_t, void*);
    friend size_t curl_write_func(void*, size_t, size_t, void*);

    /* contains the mime type or the multipart separator in case of multipart */
    std::string _contentInfo;

    /* contains the raw result of the request */
    void* _data;

    /* contains the size of the request's result */
    size_t _dataSize;

    /* curl handle */
    CURL* _curl;

    /* the resource location to access */
    const char* _url;

    /* the object which receives a call back to decode the response */
    const WVSCodec* _object;

    /* the method of the decoder */
    CallbackMethodT _callback;

    /* some custom data */
    void* _userInfo;

    /* the threads 'main' function (runs the curl request) */
    void run();

    CurlRequestThread()
      : _data(NULL),
        _dataSize(0) { }

    ~CurlRequestThread() { }
  };

  /* list of mutexes created, can be accessed by index later on */
  std::vector<OpenThreads::Mutex*> _locks;

  /* list of curl threads started, needed for cleanup */
  std::vector<OpenThreads::Thread*> _threads;

  /* the backing store thread */
  BackingStoreRestoreThread* _backingStoreThread;

public: CrossPlatformAPIFactory();
  ~CrossPlatformAPIFactory();
  double getTimeInMS();
  void getResourcePathASCII(char* const cBuffer,
      const int iLength,
      const char* const filename = NULL);
  void getCachePathASCII(char* const cBuffer,
      const int iLength,
      const char* const filename = NULL);
  void getResourceFromURL(const char* const url,
      const bool isAsynchronousRequest,
      const WVSCodec* const object,
      CallbackMethodT callback,
      void* userInfo);
  void createLocks(const uint32_t numberOfLocks);
  void lock(const uint32_t lockID) const;
  bool tryLock(const uint32_t lockID) const;
  void unlock(const uint32_t lockID) const;
  void forkBackingStoreRestoreThread(Octree* const octree);
  void showMessage(const char* const title, const char* const message) const;
};
}

#endif  /* end of include guard: CROSSPLATFORMAPIFACTORY_H */

#endif