/**
 * @file macro.h
 * @brief Header containing macros necessary for cross platform compilation.
 */

#ifndef CORELINK_HEADERS_MACRO_H
#define CORELINK_HEADERS_MACRO_H

#define EXPORTED

# ifdef _WIN32
#  ifdef WIN_EXPORT
#   ifndef EXPORTED
#    define EXPORTED __declspec( dllexport )
#   endif
#  else
#   ifndef EXPORTED
#    define EXPORTED __declspec( dllimport )
#   endif
#  endif
# else
#  ifndef EXPORTED
#   define EXPORTED
#  endif
# endif

# ifdef _WIN32
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
# else
# endif

#ifndef STREAM_ID
#define STREAM_ID int
#endif

// Stream default (invalid) value
#ifndef STREAM_DEF 
#define STREAM_DEF -1
#endif

#endif