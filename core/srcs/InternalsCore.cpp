#include "InternalsCore.hh"

BinSlay::InternalsCore::InternalsCore()
  : BinSlay::Core_ErrorHandler(),
    _ptr_bin_ctor(nullptr),
    _ptr_bin_dtor(nullptr)
{
}

bool BinSlay::InternalsCore::init()
{
  // Load 'BinaryHelper' dll
  if (!_loadInternalLibraries())
    return false;
  // We set the current status to true meaning the initialisation of the BinSlayer
  // internal core went just fine.
  _current_status = BinSlay::Core_ErrorHandler::Status::OK;
  return true;
}

BinSlay::InternalsCore::~InternalsCore()
{
  // If the current status is 'DLL_LOADING_FAIL' or 'SYMBOL_LOADING_FAIL', we do not
  // perform any cleaning operations because we do not know if the object is still valid.
  if (_current_status == BinSlay::Core_ErrorHandler::Status::DLL_LOADING_FAIL ||
      _current_status == BinSlay::Core_ErrorHandler::Status::SYMBOL_LOADING_FAIL)
    return;
  // We can delete all internals lists.
  for (auto it_bb_core = _bb_cores.begin(); it_bb_core != _bb_cores.end(); ++it_bb_core) {
    delete it_bb_core->first;
  }
  for (auto it_cfg_core = _cfg_cores.begin(); it_cfg_core != _cfg_cores.end(); ++it_cfg_core) {
    delete it_cfg_core->first;
  }
  // For the _cg_cores list, we also want to free the memory used by the 'IBinary' object
  for (auto it_cg_core = _cg_cores.begin(); it_cg_core != _cg_cores.end(); ++it_cg_core) {
    delete it_cg_core->first;
    (*_ptr_bin_dtor)(it_cg_core->second.first);
    (*_ptr_bin_dtor)(it_cg_core->second.second);
  }
  _loader.CloseLibrary("BinaryHelper");
}

BinSlay::ReverseAPI::IBinary *
BinSlay::InternalsCore::_loadBinary(
	std::string const &bin
) const
{
  return (*_ptr_bin_ctor)(bin);
}

BinSlay::CG_Core *
BinSlay::InternalsCore::createCG_Core(
	std::string const &bin1,
	std::string const &bin2
)
{
  // Load binary 1
  ReverseAPI::IBinary *left = _loadBinary(bin1);
  IF_NULLPTR_RETURN_NULL(left, BinSlay::Core_ErrorHandler::Status::BINARY_LOADING_FAIL,
			 "Critical: Failed to load binary '%s'.", bin1.c_str());
  // Load binary 2
  ReverseAPI::IBinary *right = _loadBinary(bin2);
  if (!right) {
    _buf_error << "Critical: Failed to load binary '" << bin2 << "'.";
    delete left;
    _current_status = BinSlay::Core_ErrorHandler::Status::BINARY_LOADING_FAIL;
    return nullptr;
  }
  // Create a new instance of a CG_Core object
  CG_Core *cg_core = new (std::nothrow) CG_Core(*left, *right);
  if (!cg_core) {
    _buf_error << "Critical: Bad allocation while trying to allocate a 'CG_Core' object."
	       << "  File: " << __FILE__ << std::endl
	       << "  Function: " << __FUNCTION__ << std::endl
	       << "  Line: " << __LINE__;
    _current_status = BinSlay::Core_ErrorHandler::Status::BAD_ALLOC;
    delete left;
    delete right;
    return nullptr;
  }
  // Load graphs into this new instance
  if (!cg_core->load_graphs()) {
    _buf_error << "Critical: Bad allocation while trying to allocate 'CallGraph' object."
	       << "  File: " << __FILE__ << std::endl
	       << "  Function: " << __FUNCTION__ << std::endl
	       << "  Line: " << __LINE__;
    _current_status = BinSlay::Core_ErrorHandler::Status::BAD_ALLOC;
    delete left;
    delete right;
    return nullptr;    
  }
  // Add this instance into the '_cg_cores' map
  _cg_cores.insert({cg_core, {left, right}});
  // Return the ptr on the created cg_core
  return cg_core;
}

BinSlay::CFG_Core *
BinSlay::InternalsCore::createCFG_Core(
	BinSlay::CG_Core *cg_core,
	ReverseAPI::Address fct1,
	ReverseAPI::Address fct2
)
{
  // Create a new instance of a CFG_Core object
  CFG_Core *cfg_core = new (std::nothrow) CFG_Core( *_cg_cores[cg_core].first,
	*_cg_cores[cg_core].second, fct1, fct2);
  IF_NULLPTR_RETURN_NULL(cfg_core, BinSlay::Core_ErrorHandler::Status::BAD_ALLOC,
		       "Critical: Bad allocation while trying to allocate a 'cfg_core'.");
  // Add this instance into the '_cfg_cores' map
  _cfg_cores.insert({cfg_core, {_cg_cores[cg_core].first, _cg_cores[cg_core].second}});
  // Return the ptr on the created cfg_core
  return cfg_core;
}

BinSlay::BasicBlock_Core *
BinSlay::InternalsCore::createBasicBlock_Core(
	BinSlay::CFG_Core *cfg_core,
	BinSlay::ReverseAPI::Address bb1,
	BinSlay::ReverseAPI::Address bb2
)
{
  // Create a new instance of a BasicBlock_Core object
  BasicBlock_Core *bb_core = new (std::nothrow) BasicBlock_Core(
	*create_basic_block(cfg_core, bb1, 0),
	*create_basic_block(cfg_core, bb2, 1)
  );
  IF_NULLPTR_RETURN_NULL(bb_core, BinSlay::Core_ErrorHandler::Status::BAD_ALLOC,
		       "Critical: Bad allocation while trying to allocate a 'bb_core'.");
  // Add this instance into the '_bb_cores' map
  _bb_cores.insert({bb_core, {_cfg_cores[cfg_core].first, _cfg_cores[cfg_core].second}});
  // Return the ptr on the created bb_core
  return bb_core;
}

void BinSlay::InternalsCore::deleteCG_Core(BinSlay::CG_Core *cg_core)
{
  // TODO: a check function -> we cannot delete a CG_Core as long as their are
  // some cfg or bb cores which have been spawned by this cg_core.

  // check if the cg_cores exist in the list
  BinSlay::CG_CORES_LIST::iterator it_cg_core = _cg_cores.find(cg_core);
  delete it_cg_core->first;
  (*_ptr_bin_dtor)(it_cg_core->second.first);
  (*_ptr_bin_dtor)(it_cg_core->second.second);
  _cg_cores.erase(it_cg_core);  
}

void BinSlay::InternalsCore::deleteCFG_Core(BinSlay::CFG_Core *cfg_core)
{
  // TODO: a check function -> we cannot delete a CFG_Core as long as their are
  // some bb cores which have been spawned by this cfg_core.
  BinSlay::CFG_CORES_LIST::iterator it = _cfg_cores.find(cfg_core);
  delete it->first;
  _cfg_cores.erase(it);
}

void BinSlay::InternalsCore::deleteBasicBlock_Core(BinSlay::BasicBlock_Core *bb_core)
{
  BinSlay::BASIC_BLOCK_CORES_LIST::iterator it = _bb_cores.find(bb_core);
  delete it->first;
  _bb_cores.erase(it);
}

BinSlay::Function *
BinSlay::InternalsCore::create_function(
	BinSlay::CG_Core *cg_core,
	BinSlay::ReverseAPI::Address addr,
	int which_binary
)
{
  BinSlay::Function *function = nullptr;

  if (which_binary == 0) {
    for (auto it_node = cg_core->get_graph_left().getMatrix().begin();
	 it_node != cg_core->get_graph_left().getMatrix().end(); ++it_node) {
      if ((*it_node)->getAddr() == addr)
	function = new (std::nothrow) BinSlay::Function(_cg_cores[cg_core].first, **it_node, addr);
    }
  } else {
    for (auto it_node = cg_core->get_graph_right().getMatrix().begin();
	 it_node != cg_core->get_graph_right().getMatrix().end(); ++it_node) {
      if ((*it_node)->getAddr() == addr)
	function = new (std::nothrow) BinSlay::Function(_cg_cores[cg_core].second, **it_node, addr);
    }
  }
  IF_NULLPTR(function, BinSlay::Core_ErrorHandler::Status::BAD_ALLOC,
	     "Critical: Bad allocation while trying to allocate a 'Function' object.");
  return function;
}

BinSlay::BasicBlock *
BinSlay::InternalsCore::create_basic_block(
	BinSlay::CFG_Core *cfg_core,
	BinSlay::ReverseAPI::Address addr,
	int which_binary
)
{
  BinSlay::BasicBlock *basic_block = nullptr;

  if (which_binary == 0) {
    for (auto it_node = cfg_core->get_graph_left().getMatrix().begin();
	 it_node != cfg_core->get_graph_left().getMatrix().end(); ++it_node) {
      if ((*it_node)->getAddr() == addr) {
	basic_block = new (std::nothrow)
	  BinSlay::BasicBlock(_cfg_cores[cfg_core].first, **it_node, addr);
      }
    }
  } else {
    for (auto it_node = cfg_core->get_graph_right().getMatrix().begin();
	 it_node != cfg_core->get_graph_right().getMatrix().end(); ++it_node) {
      if ((*it_node)->getAddr() == addr) {
	basic_block = new (std::nothrow)
	  BinSlay::BasicBlock(_cfg_cores[cfg_core].second, **it_node, addr);
      }
    }
  }
  IF_NULLPTR(basic_block, BinSlay::Core_ErrorHandler::Status::BAD_ALLOC,
	     "Critical: Bad allocation while trying to allocate a 'BasicBlock' object.");
  return basic_block;
}

bool BinSlay::InternalsCore::_loadInternalLibraries()
{
  // Open the library 'BinaryHelper'
  if (!_loader.OpenLibrary("BinaryHelper")) {
    _buf_error << "Critical: Fail to open dll 'BinaryHelper'." << std::endl
	       << "=> You need to place the 'libBinaryHelper.{ext}'" << std::endl
	       << " in the same directories where resides the 'BinSlayer' executable." << std::endl
	       << " Please see the readme and/or the documentation for more information.";
    _current_status = BinSlay::Core_ErrorHandler::Status::DLL_LOADING_FAIL;
    return false;
  }
  // Retrieve the symbol 'get_instance_of_binary' from the 'BinaryHelper' dynamic library
  _ptr_bin_ctor = _loader.GetSymboleFromLibrary<
    BinSlay::ReverseAPI::IBinary *(*)(std::string const &)
  >("BinaryHelper", "get_instance_of_binary");
  if (!_ptr_bin_ctor) {
    _buf_error << "Critical: Fail to load symbol 'get_instance_of_binary'." << std::endl;
    _loader.CloseLibrary("BinaryHelper");
    _current_status = BinSlay::Core_ErrorHandler::Status::SYMBOL_LOADING_FAIL;
    return false;
  }
  // Retrieve the symbol 'delete_instance_of_binary' from the 'BinaryHelper' dynamic library
  _ptr_bin_dtor = _loader.GetSymboleFromLibrary<void (*)(BinSlay::ReverseAPI::IBinary *)>
    ("BinaryHelper", "delete_instance_of_binary");
  if (!_ptr_bin_dtor) {
    _buf_error << "Critical: Fail to load symbol 'delete_instance_of_binary'." << std::endl;
    _loader.CloseLibrary("BinaryHelper");
    _current_status = BinSlay::Core_ErrorHandler::Status::SYMBOL_LOADING_FAIL;
    return false;
  }
  return true;
}
