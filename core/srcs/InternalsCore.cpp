#include "InternalsCore.hh"

// TODO: Exceptions !

BinSlay::InternalsCore::InternalsCore()
  : _ptr_bin_ctor(NULL)
{
}

bool BinSlay::InternalsCore::init()
{
  // Load 'BinaryHelper' dll
  if (!_loadInternalLibraries())
    return false;
  return true;
}

// BinSlay::InternalsCore::InternalsCore(BinSlay::InternalsCore const &o)
// {
  
// }

BinSlay::InternalsCore::~InternalsCore()
{
  _loader.CloseLibrary("BinaryHelper");
}

// BinSlay::InternalsCore& BinSlay::InternalsCore::operator=(BinSlay::InternalsCore const &o)
// {
  
// }

BinSlay::ReverseAPI::IBinary *BinSlay::InternalsCore::_loadBinary(std::string const &bin) const
{
  return (*_ptr_bin_ctor)(bin);  
}

BinSlay::CG_Core *BinSlay::InternalsCore::createCG_Core(
		 std::string const &bin1, std::string const &bin2)
{
  // Load binary 1
  ReverseAPI::IBinary *left = _loadBinary(bin1);
  if (!left)
    {
      _buf_error << "Critical: Failed to load binary '" << bin2 << "'." << std::endl;
      return NULL;
    }

  // Load binary 2
  ReverseAPI::IBinary *right = _loadBinary(bin2);
  if (!right)
    {
      _buf_error << "Critical: Failed to load binary '" << bin2 << "'." << std::endl;
      delete left;
      return NULL;
    }

  // Create a new instance of a CG_Core object
  CG_Core *cg_core = new CG_Core(*left, *right);

  // Add this instance into the '_cg_cores' map
  _cg_cores.insert(std::make_pair(
				  cg_core,
				  std::make_pair(left, right)
				  )
		   );

  // Return the ptr on the created cg_core
  return cg_core;
}

BinSlay::CFG_Core *BinSlay::InternalsCore::createCFG_Core(
	    BinSlay::CG_Core *cg_core, ReverseAPI::Address fct1, ReverseAPI::Address fct2)
{
  // Create a new instance of a CFG_Core object
  CFG_Core *cfg_core = new CFG_Core(
			     *_cg_cores[cg_core].first,
			     *_cg_cores[cg_core].second,
			     fct1,
			     fct2
  );

  // Add this instance into the '_cfg_cores' map
  _cfg_cores.insert(std::make_pair(
				   cfg_core,
				   std::make_pair(
						  _cg_cores[cg_core].first,
						  _cg_cores[cg_core].second
						  )
				   )
		    );

  // Return the ptr on the created cfg_core
  return cfg_core;
}

BinSlay::BasicBlock_Core *BinSlay::InternalsCore::createBasicBlock_Core(
    BinSlay::CFG_Core *cfg_core, BinSlay::ReverseAPI::Address bb1, BinSlay::ReverseAPI::Address bb2)
{
  // Create a new instance of a BasicBlock_Core object
  BasicBlock_Core *bb_core = new BasicBlock_Core(
		 *create_basic_block(cfg_core, bb1, 0),
		 *create_basic_block(cfg_core, bb2, 1)
  );

  // Add this instance into the '_bb_cores' map
  _bb_cores.insert(std::make_pair(
				   bb_core,
				   std::make_pair(
						  _cfg_cores[cfg_core].first,
						  _cfg_cores[cfg_core].second
						  )
				   )
		    );

  // Return the ptr on the created bb_core
  return bb_core;
}

void BinSlay::InternalsCore::deleteCG_Core(BinSlay::CG_Core *cg_core)
{
  BinSlay::CG_CORES_LIST::iterator it = _cg_cores.find(cg_core);
  delete it->first;
  _cg_cores.erase(it);  
}

void BinSlay::InternalsCore::deleteCFG_Core(BinSlay::CFG_Core *cfg_core)
{
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

std::stringstream const &BinSlay::InternalsCore::getErrorBuffer() const
{
  return this->_buf_error;
}

BinSlay::Function *BinSlay::InternalsCore::create_function(
     BinSlay::CG_Core *cg_core, BinSlay::ReverseAPI::Address addr, int which_binary)
{

  if (which_binary == 0)
    {
      for (std::vector<BinSlay::FctNode *>::const_iterator it =
	     cg_core->get_graph_left().getMatrix().begin();
	   it != cg_core->get_graph_left().getMatrix().end(); ++it)
	{
	  if ((*it)->getAddr() == addr)
	    return new BinSlay::Function(_cg_cores[cg_core].first, **it, addr);
	}
    }
  else
    {
      for (std::vector<BinSlay::FctNode *>::const_iterator it =
	     cg_core->get_graph_right().getMatrix().begin();
	   it != cg_core->get_graph_right().getMatrix().end(); ++it)
	{
	  if ((*it)->getAddr() == addr)
	    return new BinSlay::Function(_cg_cores[cg_core].second, **it, addr);
	}
    }
  return NULL;
}

BinSlay::BasicBlock *BinSlay::InternalsCore::create_basic_block(
     BinSlay::CFG_Core *cfg_core, BinSlay::ReverseAPI::Address addr, int which_binary)
{
  if (which_binary == 0)
    {
      for (std::vector<BinSlay::BbNode *>::const_iterator it =
	     cfg_core->get_graph_left().getMatrix().begin();
	   it != cfg_core->get_graph_left().getMatrix().end(); ++it)
	{
	  if ((*it)->getAddr() == addr)
	    return new BinSlay::BasicBlock(_cfg_cores[cfg_core].first, **it, addr);
	}
    }
  else
    {
      for (std::vector<BinSlay::BbNode *>::const_iterator it =
	     cfg_core->get_graph_right().getMatrix().begin();
	   it != cfg_core->get_graph_right().getMatrix().end(); ++it)
	{
	  if ((*it)->getAddr() == addr)
	    return new BinSlay::BasicBlock(_cfg_cores[cfg_core].second, **it, addr);
	}
    }
  return NULL;
}

bool BinSlay::InternalsCore::_loadInternalLibraries()
{
  // Open the library 'BinaryHelper'
  if (!_loader.OpenLibrary("BinaryHelper"))
    {
      _buf_error << "Critical: Fail to open dll 'BinaryHelper'. You need to place the 'libBinaryHelper.{ext}' in the same directories where resides the 'BinSlay' executable. Please see the readme and/or the documentation for more information." << std::endl;
      return false;
    }

  // Retrieve the symbol 'get_instance_of_binary' from the 'BinaryHelper' dynamic library
  _ptr_bin_ctor = _loader.GetSymboleFromLibrary
    <BinSlay::ReverseAPI::IBinary *(*)(std::string const &)>("BinaryHelper", "get_instance_of_binary");
  if (!_ptr_bin_ctor)
    {
      _buf_error << "Critical: Fail to load symbol 'get_instance_of_binary'." << std::endl;
      return false;
    }
  return true;
}
