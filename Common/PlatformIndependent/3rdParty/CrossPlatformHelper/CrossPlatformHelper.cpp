#include "CrossPlatformHelper.h"

/******************************************************************************/
#if defined(__APPLE__)
/******************************************************************************/

void sleep_ms(unsigned int p_ms) {
  usleep(p_ms * 1000);
}

/******************************************************************************/
#elif defined(_WIN32)
/******************************************************************************/
double roundf(double x) {
  return (x >= 0.0) ? floor(x + 0.5) : ceil(x - 0.5);
}

/* TODO memset_pattern4 and memset_pattern8 could be implemented by the same
 * function with dynamic size and then wrapped */
void memset_pattern4(void* p_destination, const void* p_pattern, size_t p_count) {
  /* TODO: see memset_pattern8 */
  for(size_t i = 0; i < (p_count / 4); i++) {
    memcpy(((char*)p_destination) + (i * 4), p_pattern, 4);
  }
}

void memset_pattern8(void* p_destination, const void* p_pattern, size_t p_count) {
  /* TODO: it would be faster, if we copy 8 bytes at first, then we have
   * 16 bytes with the pattern, now we could copy the 16 bytes and have
   * 32 bytes, now we can copy those 32 to have 64 etc... */
  for(size_t i = 0; i < (p_count / 8); i++) {
    memcpy(((char*)p_destination) + (i * 8), p_pattern, 8);
  }
}

void sleep_ms(unsigned int p_ms) {
  Sleep(p_ms);
}

#endif

