/**
 * \file InternalsCore.hh
 * \author Martial Bourquin
 * \version 1.0
 * \date 30/06/2012
 */

#ifndef __INTERNALS_CORE_HH__
# define __INTERNALS_CORE_HH__

# include <map>
# include <string>
# include "Core_ErrorHandler.hh"
# include "BinSlay.hpp"
# include "DLLmanager.h"
# include "CG_Core.hh"
# include "CFG_Core.hh"
# include "BasicBlock_Core.hh"
# include "Function.hh"
# include "BasicBlock.hh"

namespace BinSlay
{
  typedef std::pair<
      ReverseAPI::IBinary *,
      ReverseAPI::IBinary *
    > BINARIES_PAIR;

  typedef std::map<
      CG_Core *,
      BINARIES_PAIR
    > CG_CORES_LIST;

  typedef std::map<
      CFG_Core *,
      BINARIES_PAIR
    > CFG_CORES_LIST;

  typedef std::map<
      BasicBlock_Core *,
      BINARIES_PAIR
    > BASIC_BLOCK_CORES_LIST;

  //
  // Program internals management and objects factory (Core, Function, etc.)
  //

  class InternalsCore : public Core_ErrorHandler
  {
  public:
    InternalsCore();
    InternalsCore(InternalsCore const &o);
    ~InternalsCore();
    InternalsCore& operator=(InternalsCore const &o);

  public:
    // To init the InternalsCore object. This function HAS TO BE CALLED JUST
    // AFTER the constructor.
    bool init();
    // Cores manager functions
    CG_Core *createCG_Core(std::string const &bin1, std::string const &bin2);
    CFG_Core *createCFG_Core(CG_Core *cg_core, ReverseAPI::Address fct1, ReverseAPI::Address fct2);
    BasicBlock_Core *createBasicBlock_Core(CFG_Core *cfg_core, ReverseAPI::Address bb1,
				    ReverseAPI::Address bb2);
    void deleteCG_Core(CG_Core *cg_core);
    void deleteCFG_Core(CFG_Core *cfg_core);
    void deleteBasicBlock_Core(BasicBlock_Core *bb_core);

    // 'Function' and 'BasicBlock' factories:
    //		- which_binary == 0 => left
    //		- which_binary == 1 => right
    BinSlay::Function *create_function(CG_Core *cg_core, ReverseAPI::Address, int which_binary);
    BinSlay::BasicBlock *create_basic_block(CFG_Core *cfg_core, ReverseAPI::Address, int which_binary);

  private:
    bool _loadInternalLibraries();
    ReverseAPI::IBinary *_loadBinary(std::string const &bin) const;


  private:
    // Ptr on function to retrieve an instance of a 'IBinary' class
    BinSlay::ReverseAPI::IBinary *(*_ptr_bin_ctor)(std::string const &);
    void (*_ptr_bin_dtor)(BinSlay::ReverseAPI::IBinary *);
    // Dllmanager
    DllManager			_loader;
    // List de CG_Core
    CG_CORES_LIST		_cg_cores;
    // List de CFG_Core
    CFG_CORES_LIST		_cfg_cores;
    // List de BasicBlock_Core
    BASIC_BLOCK_CORES_LIST	_bb_cores;
  };
}

#endif // !__INTERNALS_CORE_HH__
