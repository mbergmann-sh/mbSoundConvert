/*
 * Feste config.h für libmp3lame (LAME 4.0) – ersetzt das autoconf-/configure-Skript,
 * damit die Bibliothek direkt mit qmake (GCC, Clang, MSVC) gebaut werden kann.
 * Die Werte entsprechen dem, was ./configure --disable-decoder --disable-nasm auf
 * Linux/x86_64 erzeugt; Besonderheiten für Windows (LLP64) sind berücksichtigt.
 * Nicht Teil der LAME-Quellen (LGPL) – Teil von mbSoundConvert (MIT).
 */
#ifndef LAME_CONFIG_H
#define LAME_CONFIG_H

#define PACKAGE "lame"
#define PACKAGE_NAME "lame"
#define PACKAGE_TARNAME "lame"
#define PACKAGE_VERSION "4.0"
#define PACKAGE_STRING "lame 4.0"
#define PACKAGE_BUGREPORT "lame-dev@lists.sf.net"
#define VERSION "4.0"

#define LAME_LIBRARY_BUILD 1
#define STDC_HEADERS 1
#define HAVE_STRCHR 1
#define HAVE_MEMCPY 1

#define HAVE_ERRNO_H 1
#define HAVE_FCNTL_H 1
#define HAVE_LIMITS_H 1
#define HAVE_STDINT_H 1
#define HAVE_INTTYPES_H 1
#define HAVE_STDIO_H 1
#define HAVE_STDLIB_H 1
#define HAVE_STRING_H 1
#define HAVE_SYS_TYPES_H 1
#define HAVE_SYS_STAT_H 1
#if !defined(_WIN32)
# define HAVE_UNISTD_H 1
# define HAVE_SYS_TIME_H 1
# define HAVE_GETTIMEOFDAY 1
#endif

/* <stdint.h> liefert alle (u)intN_t-Typen */
#define HAVE_INT8_T 1
#define HAVE_UINT8_T 1
#define HAVE_INT16_T 1
#define HAVE_UINT16_T 1
#define HAVE_INT32_T 1
#define HAVE_UINT32_T 1
#define HAVE_INT64_T 1
#define HAVE_UINT64_T 1

typedef float       ieee754_float32_t;
typedef double      ieee754_float64_t;
typedef long double ieee854_float80_t;

#define SIZEOF_SHORT 2
#define SIZEOF_UNSIGNED_SHORT 2
#define SIZEOF_INT 4
#define SIZEOF_UNSIGNED_INT 4
#if defined(__SIZEOF_LONG__)
# define SIZEOF_LONG __SIZEOF_LONG__
# define SIZEOF_UNSIGNED_LONG __SIZEOF_LONG__
#elif defined(_WIN32)
# define SIZEOF_LONG 4
# define SIZEOF_UNSIGNED_LONG 4
#else
# define SIZEOF_LONG 8
# define SIZEOF_UNSIGNED_LONG 8
#endif
#define SIZEOF_LONG_LONG 8
#define SIZEOF_UNSIGNED_LONG_LONG 8
#define SIZEOF_FLOAT 4
#define SIZEOF_DOUBLE 8
#if defined(__SIZEOF_LONG_DOUBLE__)
# define SIZEOF_LONG_DOUBLE __SIZEOF_LONG_DOUBLE__
#else
# define SIZEOF_LONG_DOUBLE 8
#endif

#define TAKEHIRO_IEEE754_HACK 0
#define USE_FAST_LOG 1

/* Little Endian (x86, x86_64, ARM) – WORDS_BIGENDIAN bewusst nicht definiert. */
/* Kein SSE-/NASM-Pfad: HAVE_XMMINTRIN_H und HAVE_NASM bleiben undefiniert (reiner C-Code). */
/* Kein externer Decoder: HAVE_MPG123 und DECODE_ON_THE_FLY bleiben undefiniert (reiner Encoder). */

#endif /* LAME_CONFIG_H */
