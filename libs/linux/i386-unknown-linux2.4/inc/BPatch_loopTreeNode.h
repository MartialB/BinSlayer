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

#ifndef _BPatch_loopTreeNode_h_
#define _BPatch_loopTreeNode_h_

#include "BPatch_dll.h"
#include "BPatch_Vector.h"
#include "BPatch_eventLock.h"
#include "BPatch_function.h"

class BPatch_basicBlockLoop;
class func_instance;

/** A class to represent the tree of nested loops and 
 *  callees (functions) in the control flow graph.
 *  @see BPatch_basicBlockLoop
 *  @see BPatch_flowGraph
 */
#ifdef DYNINST_CLASS_NAME
#undef DYNINST_CLASS_NAME
#endif
#define DYNINST_CLASS_NAME BPatch_loopTreeNode

class BPATCH_DLL_EXPORT BPatch_loopTreeNode : public BPatch_eventLock {
    friend class BPatch_flowGraph;

 public:
    // A loop node contains a single BPatch_basicBlockLoop instance
    BPatch_basicBlockLoop *loop;

    // The BPatch_loopTreeNode instances nested within this loop.
    BPatch_Vector<BPatch_loopTreeNode *> children;

    //  BPatch_loopTreeNode::BPatch_loopTreeNode
    //  Create a loop tree node for BPatch_basicBlockLoop with name n 
    API_EXPORT_CTOR(Ctor, (l,n),

    BPatch_loopTreeNode,(BPatch_basicBlockLoop *l, const char *n));

    //  BPatch_loopTreeNode::~BPatch_loopTreeNode
    //  Destructor
    API_EXPORT_DTOR(_dtor, (),

    ~,BPatch_loopTreeNode,());

    //  BPatch_loopTreeNode::name
    //  Return the name of this loop. 
    API_EXPORT(Int, (),

    const char *,name,()); 

    //  BPatch_loopTreeNode::getCalleeName
    //  Return the function name of the ith callee. 
    API_EXPORT(Int, (i),

    const char *,getCalleeName,(unsigned int i));

    //  BPatch_loopTreeNode::numCallees
    //  Return the number of callees contained in this loop's body. 
    API_EXPORT(Int, (),

    unsigned int,numCallees,());

    //Returns a vector of the functions called by this loop.
    API_EXPORT(Int, (v, p),
    bool, getCallees, (BPatch_Vector<BPatch_function *> &v, BPatch_addressSpace *p))

    //  BPatch_loopTreeNode::findLoop
    //  find loop by hierarchical name
    API_EXPORT(Int, (name),

    BPatch_basicBlockLoop *,findLoop,(const char *name));

 private:

    /** name which indicates this loop's relative nesting */
    char *hierarchicalName;

    // A vector of functions called within the body of this loop (and
    // not the body of sub loops). 
    BPatch_Vector<func_instance *> callees;

};


#endif /* _BPatch_loopTreeNode_h_ */
