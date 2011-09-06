#ifndef CROSSPLATFORMHELPER_H
#define CROSSPLATFORMHELPER_H 1

/******************************************************************************/

/* cross platform sleep functionality */
void sleep_ms(unsigned int);

/******************************************************************************/
#if defined(__APPLE__)
/******************************************************************************/

/* for usleep */
# include <unistd.h>

# include <stdint.h>

/******************************************************************************/
#elif defined(_WIN32)
/******************************************************************************/

/* include the windows header */
# include <windows.h>

/* include special vc stdint.h version with limit macros enabled */
# define __STDC_LIMIT_MACROS
# include <stdint.h>

/* to implement roundf, we need math functions */
#include <cmath>


/* define a double_t */
# ifndef double_t
#  define double_t double
# endif

/* define a float_t */
# ifndef float_t
#  define float_t float
# endif

/* undef OUT if defined */
# ifdef OUT
#  undef OUT
# endif

/* undef IN if defined */
# ifdef IN
#  undef IN
# endif

/* undef NEAR if defined */
# ifdef NEAR
#  undef NEAR
# endif

/* undef FAR if defined */
# ifdef FAR
#  undef FAR
# endif

/* define some math constants */
# ifndef M_PI
#  define M_PI 3.14159265358979323846264338327950288419716939937510
# endif

# ifndef M_PI_2
#  define M_PI_2 M_PI * M_PI
# endif

# ifndef M_SQRT2
#  define M_SQRT2 1.41421356237309504880168872420969807856967187537694
# endif

# ifndef boolean
#  define HAVE_BOOLEAN 1
#  define boolean int
# endif

/* snprintf has a different name... */
# define snprintf sprintf_s

/* roundf is not available on windows */
double roundf(double x);

/* memset_pattern4 is not available on windows */
void memset_pattern4(void*, const void*, size_t);

/* memset_pattern8 is not available on windows */
void memset_pattern8(void*, const void*, size_t);

/******************************************************************************/
#endif
/******************************************************************************/

#endif  /* end of include guard: CROSSPLATFORMHELPER_H */
