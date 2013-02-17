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

#ifndef _BPatch_eventLock_h_
#define _BPatch_eventLock_h_

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "BPatch_dll.h"

#if defined (_MSC_VER)
#define CONST_EXPORT const
#else
#define CONST_EXPORT const
#endif

#if defined (_MSC_VER)
#define CONST_ARG
#else
#define CONST_ARG const
#endif

extern unsigned long primary_thread_id;
/*
 * Class BPatch_eventLock
 *
 *
 * This class contains a (single) mutex lock controlling access to all
 * underlying BPatch (and lower level) data.
 * This object is a virtual class that should be a base class for all user
 * api's to dyninst
 */


//  BPatch_eventLock
//  
//  This class forms a base class for most DyninstAPI classes and is currently
//  responsible for managing a global mutex variable which controls access to 
//  _all_ internal dyninst/paradyn data structures.
//
//  It works in conjunction with the locking macros to isolate all mutex operations,
//  but can be used seperately as needed (should not be necessary, think twice).
//
//  Eventually may become more specialized if we move to a finer-grain of data locking.

#define __LOCK _Lock(__FILE__, __LINE__)
#define __UNLOCK _Unlock(__FILE__, __LINE__)
#define __BROADCAST _Broadcast(__FILE__, __LINE__)
#define __WAIT_FOR_SIGNAL _WaitForSignal(__FILE__, __LINE__)

class eventLock;
BPATCH_DLL_EXPORT int bpatch_printf(const char *format, ...);

class BPATCH_DLL_EXPORT BPatch_eventLock {
protected:

  BPatch_eventLock(); 
  virtual ~BPatch_eventLock();

public:
  static eventLock *getLock();
  unsigned long threadID() const;

  int _Lock(const char *__file__, unsigned int __line__) const; 
  int _Trylock(const char *__file__, unsigned int __line__) const; 
  int _Unlock(const char *__file__, unsigned int __line__) const; 

  int _Broadcast(const char *__file__, unsigned int __line__) const;
  int _WaitForSignal(const char *__file__, unsigned int __line__) const;

  unsigned int lockDepth() const;
};

#define pp2str(s) #s

#define LOCK_FUNCTION(t,x,w)     \
  __LOCK; \
  if (lockDepth() == 1) \
    bpatch_printf("Calling %s %s::%s %s...\n", \
                  #t, pp2str(DYNINST_CLASS_NAME), #x, #w); \
  t ret = x w; \
  if (lockDepth() == 1) \
   bpatch_printf("  Finished call %s::%s\n", \
                 pp2str(DYNINST_CLASS_NAME), #x); \
  __UNLOCK; \
  return ret

#define LOCK_FUNCTION_V(x,w)     \
  __LOCK; \
  if (lockDepth() == 1) \
    bpatch_printf("Calling void %s::%s %s\n", \
                  pp2str(DYNINST_CLASS_NAME), #x, #w); \
  x w; \
  if (lockDepth() == 1) \
    bpatch_printf("  Finished call %s::%s\n", \
                  pp2str(DYNINST_CLASS_NAME), #x); \
  __UNLOCK

//  API_EXPORT is a multithread-lock wrapper.
//  Argument summary:
//
//  z:  The suffix appended onto the wrapped function, usually "Int", no quotes.
//      The wrapped function and the wrapping function must have distinct names
//      or else they will not be distinguishible (type-wise) overloads.
//  w:  The list of arguments passed to the wrapped function.  Really just the 
//      argument list without type information.
//  t:  Return type of the function
//  x:  The name of the function as exported to the API user.  The "internal",
//      ie unlocked function prototype is constructed by appending arg <z> to this.
//  y:  The parameter list for the function exported to the API user.

#define API_EXPORT(z, w, t, x, y)      private:  t x##z y; \
                                       public:   t x y \
                                       { LOCK_FUNCTION(t, x##z, w);}

#define API_EXPORT_VIRT(z, w, t, x, y)      private:  t x##z y; \
                                            public:   virtual t x y \
                                            { LOCK_FUNCTION(t, x##z, w);}

//  API_EXPORT_V is used for functions that return void.
//  This variant would not be necessary were (windows) vc6 not 
//  buggy in its template specialization code.  But here we are...
//  If we move to vc7, this should be re-evaluated.  The (documented)
//  windows compiler bugs only apply to vc6.

#define API_EXPORT_V(z, w, t, x, y)    private:  t x##z y; \
                                       public:   t x y \
                                       { LOCK_FUNCTION_V(x##z,w);}

//  "CTOR" and "DTOR" flavors do not have return type.
//  wrapped function implicitly returns void.

#define API_EXPORT_CTOR(z, w, x, y)    private:  void x##z y; \
                                       public:   x y \
                                       { LOCK_FUNCTION_V(x##z,w);}

//  here t is for "tilde" (ignored, but there for dtor appearance)

#define API_EXPORT_DTOR(z, w, t, x, y) private:  void x##z y; \
                                       public:   virtual ~x y \
                                       { LOCK_FUNCTION_V(x##z,w);}

//  operators present a special case for the preprocessor
//  suffix <z> is appended to the word "operator", eg, operator_lessthan
//  (with <z> = "_lessthan"), forms the wrapped function name

#define API_EXPORT_OPER(z, w, t, x, y) private:  t operator##z y; \
                                       public:   t x y \
                                       { LOCK_FUNCTION(t,operator##z,w);}


#endif

