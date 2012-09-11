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

#ifndef _BPatch_module_h_
#define _BPatch_module_h_
#include "BPatch_dll.h"
#include "BPatch_Vector.h"
#include "BPatch_sourceObj.h"
#include "BPatch_eventLock.h"
#include "BPatch_enums.h"
#include "dyntypes.h"
#include <vector>
#include <map>
#if defined(IBM_BPATCH_COMPAT)
#include <string>
#endif

class mapped_module;

class BPatch_image;
class BPatch_function;
class BPatch_typeCollection;
class BPatch_builtInTypeCollection;
class BPatch_addressSpace;
class BPatch_process;
class BPatch_statement;
class func_instance;
class int_variable;
class instPoint;
class AddressSpace;
class BPatch_snippet;
class BPatchSnippetHandle;
class BPatch_module;

namespace Dyninst { 
   namespace ParseAPI { 
      class CodeObject; 
      CodeObject *convert(const BPatch_module *);
   }
   namespace PatchAPI {
      class PatchObject;
      PatchObject *convert(const BPatch_module *);
      class PatchFunction;
      class Point;
   }
   namespace SymtabAPI {
      class Symtab;
      Symtab *convert(const BPatch_module *);
   }
}

extern BPatch_builtInTypeCollection * builtInTypes;

#ifdef DYNINST_CLASS_NAME
#undef DYNINST_CLASS_NAME
#endif
#define DYNINST_CLASS_NAME BPatch_module

class BPATCH_DLL_EXPORT BPatch_module: public BPatch_sourceObj, public BPatch_eventLock{

    friend class BPatch_function;
    friend class BPatch_flowGraph;
    friend class BPatch_image;
    friend class InstrucIter;
    friend class BPatch_thread;
    friend class BPatch_process;
    friend class BPatch_binaryEdit;
    friend class BPatch_addressSpace;
    friend class BPatch_statement;
    friend Dyninst::ParseAPI::CodeObject *Dyninst::ParseAPI::convert(const BPatch_module *);
    friend Dyninst::PatchAPI::PatchObject *Dyninst::PatchAPI::convert(const BPatch_module *);
    friend Dyninst::SymtabAPI::Symtab *Dyninst::SymtabAPI::convert(const BPatch_module *);


    typedef std::map<Dyninst::PatchAPI::PatchFunction*, 
       BPatch_function*> BPatch_funcMap;
    typedef std::map<int_variable*, BPatch_variableExpr*> BPatch_varMap;
    typedef std::map<Dyninst::PatchAPI::Point *, 
       BPatch_point*> BPatch_instpMap;


    
    BPatch_addressSpace *addSpace;
    AddressSpace *lladdSpace;
    mapped_module      	 *mod;
    BPatch_image	 *img;
    AddressSpace *getAS();

public:

    //  This function should go away when paradyn is on top of dyninst
    mapped_module* lowlevel_mod() { return mod; }

    // The following functions are for internal use by  the library only:
    BPatch_module(BPatch_addressSpace *_addSpace,
                  AddressSpace *as,
                  mapped_module *_mod, 
                  BPatch_image *img);
    virtual ~BPatch_module();
    bool getSourceObj(BPatch_Vector<BPatch_sourceObj *>&);
    BPatch_sourceObj *getObjParent();
    void parseTypes();
    char *parseStabStringSymbol(int line, char *stabstr, void *stabptr);
    void setDefaultNamespacePrefix(char *name);    
    void handleUnload();
    bool isExploratoryModeOn();// true if exploratory or defensive mode is on
    bool setAnalyzedCodeWriteable(bool writeable);//sets write perm's analyzed code pages
    bool isSystemLib();
    bool remove(BPatch_function*);
    bool remove(instPoint*);
    // End functions for internal use only
  
    // BPatch_module::getName
    // Returns file name associated with module
    API_EXPORT(Int, (buffer, length),

    char *,getName,(char *buffer, int length));

    // BPatch_module::getFullName
    // Returns full path name of module, when available
    API_EXPORT(Int, (buffer, length),

    char *,getFullName,(char *buffer, int length));

    // BPatch_module::libraryName
    // Returns name if library, if this module is a shared object
    API_EXPORT(Int, (),

    const char *,libraryName,());

    // BPatch_module::getAddressWidth
    // Returns the width (in bytes) of an address in this module
    API_EXPORT(Int, (),

    size_t,getAddressWidth,());

    // BPatch_module::getVariables
    // Fills a vector with the global variables that are specified in this module
    API_EXPORT(Int, (vars),

    bool,getVariables,(BPatch_Vector<BPatch_variableExpr *> &vars));

    // BPatch_module::findVariable
    // Find and return a global variable (NULL if not found)
    API_EXPORT(Int, (name),

    BPatch_variableExpr*,findVariable,(const char* name));

	// BPatch_module::getProcedures
    // Returns a vector of all functions in this module
    API_EXPORT(Int, (incUninstrumentable),
    BPatch_Vector<BPatch_function *> *,getProcedures,(bool incUninstrumentable = false));

    API_EXPORT(Int, (procs, incUninstrumentable),
               bool, getProcedures,(BPatch_Vector<BPatch_function*> &procs, bool incUninstrumentable = false));

    // BPatch_module::findFunction
    // Returns a vector of BPatch_function *, matching specified <name>
    API_EXPORT(Int, (name, funcs, notify_on_failure,regex_case_sensitive,incUninstrumentable,dont_use_regex),

    BPatch_Vector<BPatch_function *> *,findFunction,(const char *name,
                          BPatch_Vector<BPatch_function *> &funcs,
                          bool notify_on_failure =true,
                          bool regex_case_sensitive =true,
                          bool incUninstrumentable =false,
                          bool dont_use_regex = false));


    //  BPatch_addressSpace::findFunctionByEntry
    //  Returns the function starting at the given address
    API_EXPORT(Int, (entry),
    BPatch_function *,findFunctionByEntry,(Dyninst::Address entry));


    // FIXME: This method is (undocumented) 
    API_EXPORT(Int, (addr, funcs, notify_on_failure, incUninstrumentable),

    BPatch_Vector<BPatch_function *> *,
    findFunctionByAddress,(void *addr,
                           BPatch_Vector<BPatch_function *> &funcs,
                           bool notify_on_failure = true,
                           bool incUninstrumentable = false));

    // get the module types member (instead of directly accessing)
    API_EXPORT(Int, (), BPatch_typeCollection *, getModuleTypes, ());

    // BPatch_module::findFunctionByMangled
    // Returns a function, if it exits, that matches the provided mangled name
    API_EXPORT(Int, (mangled_name, incUninstrumentable),

    BPatch_function *,findFunctionByMangled,(const char * mangled_name,
                                             bool incUninstrumentable=false));


    //  BPatch_module::findPoints
    //
    //  Returns a vector of BPatch_points that correspond with the provided address, one
    //  per function that includes an instruction at that address. Will have one element
    //  if there is not overlapping code. 
    API_EXPORT(Int, (addr, points), 
    bool, findPoints, (Dyninst::Address addr, std::vector<BPatch_point *> &points));


    // BPatch_module::dumpMangled
    // Prints all <mangled> function names in this module
    API_EXPORT(Int, (prefix),

    bool,dumpMangled,(char *prefix));

    // BPatch_module::isSharedLib
    // Returns true if this module represents a shared library
    API_EXPORT(Int, (),

    bool,isSharedLib,());

    // BPatch_module::isNativeCompiler
    // Returns true if this module was compiled with a native compiler for  
    // the particular platform
    API_EXPORT(Int, (),

    bool,isNativeCompiler,());


    // BPatch_module::getAddressRanges
    // 
    // function to get addresses for a line of the module
    // if fileName is NULL, uses the name of the module
    API_EXPORT(Int, (fileName, lineNo, ranges),

    bool,getAddressRanges,( const char * fileName, unsigned int lineNo, std::vector< std::pair< unsigned long, unsigned long > > & ranges ));
    
    // BPatch_module::getSourceLines
    //
    // function to get source file names and lines
    // for an address in the module
    API_EXPORT(Int, (addr, lines),
    
    bool,getSourceLines,( unsigned long addr, BPatch_Vector<BPatch_statement> &lines));


    // BPatch_mode::getStatements
    //
    // Fill supplied vector with all BPatch_statements from this module
    API_EXPORT(Int, (statements),
  
    bool,getStatements,(BPatch_Vector<BPatch_statement> &statements));

    // BPatch_module::wgetBaseAddr
    // Returns a base address of the module; defined as the start
    // of the first function.
    API_EXPORT(Int, (),
    void *,getBaseAddr,(void));

    API_EXPORT(Int, (),
               Dyninst::Address, getLoadAddr, (void));

    // BPatch_module::getSize
    // Returns the size of the module; defined as the end of the last
    // function minus the start of the first function.
    API_EXPORT(Int, (),
    unsigned long, getSize, (void));

    API_EXPORT(Int, (), 
               bool, isValid, ());

    // BPastch_module::getHybridMode
    // returns the hybrid Analysis mode: normal, exploratory, defensive
    API_EXPORT(Int, (), BPatch_hybridMode, getHybridMode,());
    
    API_EXPORT(Int, (callback),
               BPatchSnippetHandle*, insertInitCallback, (BPatch_snippet& callback));

    API_EXPORT(Int, (callback),
               BPatchSnippetHandle*, insertFiniCallback, (BPatch_snippet& callback));

#ifdef IBM_BPATCH_COMPAT
    API_EXPORT(Int, (start, end),
    bool,getLineNumbers,(unsigned int &start, unsigned int &end));

    API_EXPORT(Int, (start, end),
    bool,getAddressRange,(void * &start, void * &end));

    API_EXPORT(Int, (buffer, length),
    char *,getUniqueString,(char *buffer, int length));   

    char *sharedLibraryName(char *buffer, int length) { getFullName(buffer, length); return buffer;}
    char *getSharedLibName(char *buffer, int length) { getFullName(buffer, length); return buffer;}

    API_EXPORT(Int, (),
    int,getSharedLibType,());

    API_EXPORT(Int, (),
    int,getBindingType,());

    struct Statement {
        unsigned long begin;  // Beginning address for this statement
        unsigned long end;    // Ending address for this statement
        std::string path;     // Path name of this statement's source file
        int line;             // Line number of the statement
	int column;           // Column number of the statement
    } Statement;

    API_EXPORT(Int, (),
    std::vector<struct BPatch_module::Statement>, getStatements, ());
#endif

private:
    // Parse wrapper
    bool parseTypesIfNecessary();
    BPatch_typeCollection *moduleTypes;

    // In particular, we understand the type information
    // in both DWARF and STABS format.
    void parseStabTypes();
    void parseDwarfTypes();

   BPatch_funcMap func_map;
   BPatch_instpMap instp_map;
   BPatch_varMap var_map;
   
   bool full_func_parse;
   bool full_var_parse;
};

#ifdef IBM_BPATCH_COMPAT
#define	BPatch_sharedPublic	1
#define BPatch_sharedPrivate	2
#define BPatch_nonShared	3

#define BPatch_static		1
#define BPatch_dynamic		2

#endif

#endif /* _BPatch_module_h_ */
