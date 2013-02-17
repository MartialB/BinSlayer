/*
 * See the dyninst/COPYRIGHT file for copyright information.
 * 
 * We provide the Paradyn Tools (below described as "Paradyn")
 * on an AS IS basis, and do not warrant its validity or performance.
 * We reserve the right to update, modify, or discontinue this
 * software at any time.  We shall have no obligation to supply such
 * updates or modifications or any other form of support to you.
 * 
 * By your use of Paradyn, you understand and agree that we (or any
 * other person or entity with proprietary rights in Paradyn) are
 * under no obligation to provide either maintenance services,
 * update services, notices of latent defects, or correction of
 * defects for Paradyn.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/************************************************************************
 * $Id: Types.h,v 1.38 2008/08/29 21:45:10 legendre Exp $
 * Types.h: commonly used types (used by runtime libs and other modules)
************************************************************************/

#if !defined(_Types_h_)
#define _Types_h_

/* Sets up 64 and 32 bit
   types:
      int64_t      uint64_t      int32_t       uint32_t
   constant macros:
      I64_C(x)     UI64_C(x)
   limits:
      I64_MAX      I64_MIN       UI64_MAX
      I32_MAX      I32_MIN       UI32_MAX

   note: needs to be included before anything that includes inttypes.h
         (eg. stdio on some systems)
*/

/* Set up the 32 AND 64 BIT TYPES ===================================== */
/*
   --- inttypes.h ---
             int32_t  uint32_t  int64_t uint64_t 32B lmts 64Blmts 64BlitMacros#
Sol5.6       yes      yes       yes     yes      yes      no*     yes   
Sol5.7       yes      yes       yes     yes      yes      no*     yes   
Linux        yes      yes       yes     yes      yes      yes     yes
Irix         yes      yes       yes     yes      yes      yes     yes
Osf4.0       nonexistant
Osf5.0       ?
Aix4.2       nonexistant
Aix4.3       yes      yes       yes     yes      yes      no*     yes
WindowsNT    nonexistant

  * the 64bit limits on solaris and aix are defined, but they are not defined
    properly to include the numeric literal postfix (eg. LL), so we need to
    explicitly define these
  # we rename all of the 64 bit literal macros to our shortened name
*/

#if defined(os_windows)
   typedef signed __int64 int64_t;
   typedef signed __int32 int32_t;
   typedef signed __int16 int16_t;
   typedef signed __int8 int8_t;
   typedef unsigned __int64 uint64_t;
   typedef unsigned __int32 uint32_t;
   typedef unsigned __int16 uint16_t;
   typedef unsigned __int8 uint8_t;

#elif defined(os_aix)  
#if defined (arch_power)
#  if defined(rs6000_ibm_aix64)
#       define TYPE64BIT
#  endif
#  ifndef _ALL_SOURCE
#     define _ALL_SOURCE
#  endif
#  include <sys/types.h>     /* if aix4.3, this will include inttypes.h */
#  ifndef _H_INTTYPES        /* for aix4.2 */
     typedef int int32_t;
     typedef unsigned int uint32_t;
#    if defined(rs6000_ibm_aix64)
        typedef long int64_t;
        typedef unsigned long uint64_t;
#    else
        typedef long long int64_t;
        typedef unsigned long long uint64_t;
#    endif
#  endif
#  endif /* defined (arch_power) */

#elif defined(arch_alpha)
#define TYPE64BIT
#  ifndef _H_INTTYPES
   typedef int int32_t;
   typedef long int64_t;
   typedef unsigned int uint32_t;
   typedef unsigned long uint64_t;
#  endif

#elif defined(os_linux)
#if !defined(__STDC_CONSTANT_MACROS)
#define __STDC_CONSTANT_MACROS
#endif
#if !defined(__STDC_LIMIT_MACROS)
#define __STDC_LIMIT_MACROS
#endif
#include <stdint.h>
#if defined(arch_x86_64) || defined(arch_64bit)
#define TYPE64BIT
#endif
typedef long double double128_t;

#elif defined(os_bg)
#if !defined(__STDC_CONSTANT_MACROS)
#define __STDC_CONSTANT_MACROS
#endif
#if !defined(__STDC_LIMIT_MACROS)
#define __STDC_LIMIT_MACROS
#endif
#include <stdint.h>

#elif defined(os_freebsd)
#if !defined(__STDC_CONSTANT_MACROS)
#define __STDC_CONSTANT_MACROS
#endif
#if !defined(__STDC_LIMIT_MACROS)
#define __STDC_LIMIT_MACROS
#endif
#include <stdint.h>
typedef long double double128_t;

/* FreeBSD doesn't define this */
typedef int64_t off64_t;

#elif defined(os_vxworks)
#if !defined(__STDC_CONSTANT_MACROS)
#define __STDC_CONSTANT_MACROS
#endif
#if !defined(__STDC_LIMIT_MACROS)
#define __STDC_LIMIT_MACROS
#endif
#ifndef __RTLIB__
#include <stdint.h>
#endif
#include <limits.h>
#if !defined(INT64_C)
#define INT64_C(c) ((signed long long) (c))
#endif
#if !defined(UINT64_C)
#define UINT64_C(c) ((unsigned long long) (c))
#endif

#else
#error Unknown architecture
#endif


/* Set up the 64 BIT LITERAL MACROS =================================== */
#if defined(os_aix) && !defined(_H_INTTYPES)  /* aix4.2 ---- */
#define I64_C(x)  (x##ll)
#define UI64_C(x) (x##ull)
#elif defined(os_osf)     /* osf ---------------------------- */
#define I64_C(x)  (x##l)
#define UI64_C(x) (x##ul)
#elif defined(os_windows)
				   /* nt ----------------------------- */
#define I64_C(x)  (x##i64)
#define UI64_C(x) (x##ui64)
#elif defined(os_bg)
#define I64_C(x) (x##ll)
#define U64_C(x) (x##ull)
#else                               /* linux, solaris, irix, aix4.3 --- */
#define I64_C(x)  INT64_C(x)
#define UI64_C(x) UINT64_C(x)
#endif

/* Set up the 32 and 64 BIT LIMITS for those not already set up ======= */
#if defined(os_osf)  || (defined(os_aix) && !defined(_H_INTTYPES))
#define INT32_MAX  (2147483647)
#define UINT32_MAX (4294967295U)
#define INT32_MIN  (-2147483647-1)
#endif

                                   /* solaris, aix4.{23}, osf -------- */
#if defined(os_aix)
/* see note (*) above */
#define I32_MAX    INT32_MAX
#define UI32_MAX   UINT32_MAX
#define I32_MIN    INT32_MIN
#define I64_MAX    I64_C(9223372036854775807)
#define UI64_MAX   UI64_C(18446744073709551615)
/* The GNU compilers on solaris and aix have what seems like a bug where a
   warning is printed when the ...808 int64 minimum is used, so we'll get the
   value with some trickery */
#define I64_MIN    (-I64_MAX-1)

#elif defined(os_windows)
			 /* nt ----------------------------- */
#include <limits.h>
#define I64_MAX  _I64_MAX
#define UI64_MAX _UI64_MAX
#define I64_MIN  _I64_MIN
#define I32_MAX  _I32_MAX
#define I32_MIN  _I32_MIN
#define UI32_MAX  _UI32_MAX
#else                              /* linux, irix -------------------- */
#define I64_MAX  INT64_MAX
#define UI64_MAX UINT64_MAX
#define I64_MIN  INT64_MIN
#define I32_MAX  INT32_MAX
#define I32_MIN  INT32_MIN
#define UI32_MAX UINT32_MAX
#endif

   /*
typedef int64_t time64;
*/

#if defined(__cplusplus)
#include "dynutil/h/dyntypes.h"
using namespace Dyninst;
static const Address ADDR_NULL = (Address)(0);
#else
#define ADDR_NULL (0)
typedef unsigned long Address;
#endif
/* Note the inherent assumption that the size of a "long" integer matches
   that of an address (void*) on every supported Paradyn/Dyninst system!
   (This can be checked with Address_chk().)
*/

typedef unsigned int Word;

typedef long int RegValue;      /* register content */
/* This needs to be an int since it is sometimes used to pass offsets
   to the code generator (i.e. if-statement) - jkh 5/24/99 */
typedef unsigned int Register;  /* a register number, e.g., [0..31]  */
static const Register Null_Register = (Register)(-1);   /* '255' */
/* Easily noticeable name... */
static const Register REG_NULL = (Register)(-1);

// Virtual Memory Map -- shared between platforms
#define PREMS_PRIVATE (1 << 4)
#define PREMS_SHARED  (1 << 3)
#define PREMS_READ    (1 << 2)
#define PREMS_WRITE   (1 << 1)
#define PREMS_EXEC    (1 << 0)

#define MAPENTRIES_PATH_SIZE 512
#define MAPENTRIES_PATH_SIZE_STR "512"
typedef struct maps_entries {
   Address start;
   Address end;
   unsigned prems;
   Address offset;
   int dev_major;
   int dev_minor;
   int inode;
   char path[MAPENTRIES_PATH_SIZE];
} map_entries;

#ifdef __cplusplus

#include "dynutil/h/util.h"

COMMON_EXPORT void Address_chk ();
COMMON_EXPORT char *Address_str (Address addr);

// NB: this is probably inappropriate for 64-bit addresses!
inline unsigned hash_address(const Address& addr) {
   return (unsigned) ((addr >> 2) & 0xffffffff);
}
#endif /* __cplusplus */

#endif /* !defined(_Types_h_) */

