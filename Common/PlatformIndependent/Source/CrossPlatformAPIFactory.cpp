/**
 * Cross Platform API Factory
 * @author Robert Timm <robert.timm@studen.hpi.uni-potsdam.de>
 */

#ifdef CROSSPLATFORM_API_FACTORY

#include "CrossPlatformAPIFactory.h"

#ifdef _WIN32
# include <windows.h>
#else
# include <sys/time.h>
#endif

#include "HttpMultipartParser.h"

/******************************************************************************/
namespace WVSClientCommon {
/******************************************************************************/

CrossPlatformAPIFactory::CrossPlatformAPIFactory()
  : _backingStoreThread(NULL) {
  std::cout << "CrossPlatformAPIFactory creating..." << std::endl;
  curl_global_init(CURL_GLOBAL_ALL);
}

/******************************************************************************/

CrossPlatformAPIFactory::~CrossPlatformAPIFactory() {
  /* cleanup all mutexes */
  while(_locks.size()) {
    delete _locks.back();
    _locks.pop_back();
  }

  /* join and cleanup all curl threads */
  while(_threads.size()) {
    _threads.back()->join();
    delete _threads.back();
    _threads.pop_back();
  }

  /* cleanup the backing store thread if any */
  if(_backingStoreThread) {
    _backingStoreThread->join();
    delete _backingStoreThread;
  }
}

/******************************************************************************/

double CrossPlatformAPIFactory::getTimeInMS() {
  double ret = 0.0f;

	/* TODO put this into cross platform helper */

#ifdef _WIN32
  /* Retrieves the number of milliseconds that have elapsed since
   * the system was started, up to 49.7 days. */
  ret = (double)GetTickCount();
#else
  struct timeval t;
  if(gettimeofday(&t, NULL) != 0) {
    std::cerr << "ERROR: gettimeofday()" << std::endl;
  }
  /* Retrieves the number of milliseconds since 01.01.1970. */
  ret = (t.tv_sec / 1000.0f) + (t.tv_usec * 1000.0f);
#endif

  return ret;
}

/******************************************************************************/

void CrossPlatformAPIFactory::getResourcePathASCII(char* const cBuffer,
    const int iLength,
    const char* const filename) {
  if(filename) {
    snprintf(cBuffer, iLength, "%s/Common/Shaders/%s", CODE_BASE_PATH, filename);
  }
  else {
    snprintf(cBuffer, iLength, "%s/Common/Shaders/", CODE_BASE_PATH);
  }
}

/******************************************************************************/

void CrossPlatformAPIFactory::getCachePathASCII(char* const cBuffer,
    const int iLength,
    const char* const filename) {
#ifdef _WIN32
  const char* base = CODE_BASE_PATH;
#else
  const char* base = "/tmp";
#endif

  if(filename) {
    snprintf(cBuffer, iLength, "%s/%s", base, filename);
  }
  else {
    snprintf(cBuffer, iLength, "%s/", base);
  }
}

/******************************************************************************/

size_t curl_header_func(void* ptr, size_t size, size_t nmemb, void* d) {
  /* get the thread object the request originated from */
  CrossPlatformAPIFactory::CurlRequestThread* threadObject =
    (CrossPlatformAPIFactory::CurlRequestThread*)d;

  /* get a reference to the thread objects content info string */
  std::string& contentInfo = threadObject->_contentInfo;

  /* create a thing from the header we just received */
  std::string hdr((const char*)ptr, size * nmemb);

  /* if this is the content type header */
  if((hdr.find("Content-Type: ") != hdr.npos)) {
    /* if this is a multi part content type */
    if(hdr.find("multipart/mixed") != hdr.npos) {
      /* store the boundary in contentInfo */
      size_t start = hdr.find("boundary=") + 9;
      size_t end   = hdr.find("\r\n");
      contentInfo = hdr.substr(start, end - start);
    }
    /* is this is any other content type */
    else {
      /* store the content type in contentInfo */
      size_t start = hdr.find("Content-Type: ") + 14;
      size_t end   = hdr.find(";");
      contentInfo = hdr.substr(start, end - start);
    }
  }

  /* tell curl we handled everything */
  return size * nmemb;
}

/******************************************************************************/

size_t curl_write_func(void* ptr, size_t size, size_t nmemb, void* d) {
  /* get the thread object the request originated from */
  CrossPlatformAPIFactory::CurlRequestThread* threadObject =
    (CrossPlatformAPIFactory::CurlRequestThread*)d;

  /* get the size of the content we already stored (0 the first time) */
  size_t oldDataSize = threadObject->_dataSize;

  /* first response, store size and allocate array to store response */
  if(threadObject->_data == NULL) {
    threadObject->_dataSize = size * nmemb;
    threadObject->_data     = malloc(threadObject->_dataSize);
  }
  /* further responses, realloc to extend the data array */
  else {
    threadObject->_dataSize = threadObject->_dataSize + (size * nmemb);
    threadObject->_data     = realloc(threadObject->_data,
        threadObject->_dataSize);
  }

  /* copy the new data into the thread object data array */
  memcpy(((char*)threadObject->_data) + oldDataSize, ptr, size * nmemb);

  /* tell curl we handled everything */
  return size * nmemb;
}

/******************************************************************************/

void CrossPlatformAPIFactory::CurlRequestThread::run() {
  /* setup curl for this request in this thread */
  _curl = curl_easy_init();
  /* curl_easy_setopt(_curl, CURLOPT_VERBOSE, 1L); */
  curl_easy_setopt(_curl, CURLOPT_URL, _url);
  curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, curl_write_func);
  curl_easy_setopt(_curl, CURLOPT_WRITEDATA, this);
  curl_easy_setopt(_curl, CURLOPT_HEADERFUNCTION, curl_header_func);
  curl_easy_setopt(_curl, CURLOPT_HEADERDATA, this);

  /* do the request (will call the call backs) and clean up */
  curl_easy_perform(_curl);
  curl_easy_cleanup(_curl);

  std::cout << "Received " << _dataSize << "b, " << _contentInfo << "\n";

  /* print plain text response (might be an error) */
  if(_contentInfo.find("text/") != _contentInfo.npos) {
    for(size_t i = 0; i < _dataSize; i++) {
      std::cout << ((char*)_data)[i];
    }
    std::cout << std::endl;
  }
  /* in case of a single image response, just forward it to the callback */
  else if((_contentInfo.find("image/jpeg") != _contentInfo.npos) ||
          (_contentInfo.find("image/png") != _contentInfo.npos)) {
    (_object->*_callback)((const uint8_t*)_data, _dataSize, 0, _userInfo);
  }
  /* multipart: split into content parts and forward each of them to callback */
  else {
    /* stores the offset of a message part relative to the whole message */
    size_t partOffset = 0;

    /* stores the size of a message part */
    size_t partSize = 0;

    /* every part has an id, starting from 0 */
    size_t partId = 0;

    /* get offset and size of the first part */
    HttpToolBox::parseMultipartResponse((const char*)_data, _dataSize,
        _contentInfo.c_str(), &partOffset, &partSize);

    /* as long as the part has non zero size */
    while(partSize != 0) {
      std::cout << "callback " << partId << " size:" << partSize << "b" <<
      " offset:" << partOffset << "\n";

      /* tell the callback about the part */
      (_object->*_callback)(
          ((const uint8_t*)_data) + partOffset, partSize, partId++, _userInfo);

      /* store where we start looking now */
      size_t startOffset = partOffset + partSize;

      /* get offset and size of the next part */
      HttpToolBox::parseMultipartResponse(
          ((const char*)_data) + startOffset, _dataSize - startOffset,
          _contentInfo.c_str(), &partOffset, &partSize);

      /* make the offset relative to data start again */
      partOffset += startOffset;
    }
  }

  /* free the data allocated in the curl callback */
  if(_data) {
    free(_data);
  }
}

/******************************************************************************/

void CrossPlatformAPIFactory::getResourceFromURL(const char* const url,
    const bool isAsynchronousRequest,
    const WVSCodec* const object,
    CallbackMethodT callback,
    void* userInfo) {
  std::cout << "starting getResourceFromURL async: " <<
  isAsynchronousRequest << " url:" << url << std::endl;

  /* create a new therad object */
  CurlRequestThread* thread = new CurlRequestThread;

  /* provide it with all the necessary paramters */
  thread->_url      = url;
  thread->_object   = object;
  thread->_callback = callback;
  thread->_userInfo = userInfo;

  /* start it as a thread if isAsynchronousRequest was set */
  if(isAsynchronousRequest) {
    _threads.push_back(thread);
    thread->startThread();
  }

  /* if synchronous, just run it */
  else {
    thread->run();
    delete thread;
  }
}

/******************************************************************************/

void CrossPlatformAPIFactory::createLocks(const uint32_t numberOfLocks) {
  /* create the requested number of mutexes */
  for(uint32_t i = 0; i < numberOfLocks; i++) {
    _locks.push_back(new OpenThreads::Mutex);
  }
}

/******************************************************************************/

void CrossPlatformAPIFactory::lock(const uint32_t lockID) const {
  assert(_locks.size() > lockID && "tried to access unknown mutex");

  /* try to lock the mutex, print an error if it fails */
  if(_locks[lockID]->lock() != 0) {
    std::cerr << "ERROR: lock()" << std::endl;
  }
}

/******************************************************************************/

bool CrossPlatformAPIFactory::tryLock(const uint32_t lockID) const {
  assert(_locks.size() > lockID && "tried to access unknown mutex");

  return _locks[lockID]->trylock();
}

/******************************************************************************/

void CrossPlatformAPIFactory::unlock(const uint32_t lockID) const {
  assert(_locks.size() > lockID && "tried to access unknown mutex");

  /* try to unlock the mutex, print an error if it fails */
  if(_locks[lockID]->unlock() != 0) {
    std::cerr << "ERROR: unlock()" << std::endl;
  }
}

/******************************************************************************/

void CrossPlatformAPIFactory::BackingStoreRestoreThread::run() {
  _octree->runBackingStoreRestoreThread();
}

/******************************************************************************/

void CrossPlatformAPIFactory::forkBackingStoreRestoreThread(
    Octree* const octree) {
  if(_backingStoreThread) {
    std::cerr << "BackingStoreRestoreThread already started" << std::endl;
    return;
  }

  /* set up and start the backing store thread */
  _backingStoreThread          = new BackingStoreRestoreThread;
  _backingStoreThread->_octree = octree;
  if(_backingStoreThread->startThread() != 0) {
    std::cerr << "ERROR: startThread" << std::endl;
  }
}

/******************************************************************************/

void CrossPlatformAPIFactory::showMessage(const char* const title,
    const char* const message) const {
  std::cout << "*************************************************" << std::endl;
  std::cout << "* " << title << std::endl;
  std::cout << "*************************************************" << std::endl;
  std::cout << "* " << message << std::endl;
  std::cout << "*************************************************" << std::endl;
  std::cout << std::endl << std::endl;
}
}

#endif
