#ifndef AXIUM_DEFINE_H
#define AXIUM_DEFINE_H

#include <sys/types.h>

#include <ace/Basic_Types.h>
#include <ace/ACE_export.h>

#include "CompilerDefs.h"

#define AXIUM_LITTLEENDIAN 0
#define AXIUM_BIGENDIAN    1

#if !defined(AXIUM_ENDIAN)
#  if defined (ACE_BIG_ENDIAN)
#    define AXIUM_ENDIAN AXIUM_BIGENDIAN
#  else //ACE_BYTE_ORDER != ACE_BIG_ENDIAN
#    define AXIUM_ENDIAN AXIUM_LITTLEENDIAN
#  endif //ACE_BYTE_ORDER
#endif //AXIUM_ENDIAN

#if PLATFORM == PLATFORM_WINDOWS
#  define AXIUM_PATH_MAX MAX_PATH
#  ifndef DECLSPEC_NORETURN
#    define DECLSPEC_NORETURN __declspec(noreturn)
#  endif //DECLSPEC_NORETURN
#  ifndef DECLSPEC_DEPRECATED
#    define DECLSPEC_DEPRECATED __declspec(deprecated)
#  endif //DECLSPEC_DEPRECATED
#else //PLATFORM != PLATFORM_WINDOWS
#  define AXIUM_PATH_MAX PATH_MAX
#  define DECLSPEC_NORETURN
#  define DECLSPEC_DEPRECATED
#endif //PLATFORM

#if !defined(COREDEBUG)
#  define AXIUM_INLINE inline
#else //COREDEBUG
#  if !defined(AXIUM_DEBUG)
#    define AXIUM_DEBUG
#  endif //AXIUM_DEBUG
#  define AXIUM_INLINE
#endif //!COREDEBUG

#if COMPILER == COMPILER_GNU
#  define ATTR_NORETURN __attribute__((noreturn))
#  define ATTR_PRINTF(F, V) __attribute__ ((format (printf, F, V)))
#  define ATTR_DEPRECATED __attribute__((deprecated))
#else //COMPILER != COMPILER_GNU
#  define ATTR_NORETURN
#  define ATTR_PRINTF(F, V)
#  define ATTR_DEPRECATED
#endif //COMPILER == COMPILER_GNU

typedef ACE_INT64 int64;
typedef ACE_INT32 int32;
typedef ACE_INT16 int16;
typedef ACE_INT8 int8;
typedef ACE_UINT64 uint64;
typedef ACE_UINT32 uint32;
typedef ACE_UINT16 uint16;
typedef ACE_UINT8 uint8;

#endif //AXIUM_DEFINE_H
