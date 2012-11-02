#include <fstream>
#include <iostream>
#include <iomanip>
#include <list>
#include <map>
#include <sstream>
#include <vector>
#include "DyninstBin.hh"
#include "InstructionDecoder.h"
#include "beaengine/BeaEngine.h"

//////////////////////////////////////////////////////////////////////////////////////////////

BinSlay::ReverseAPI::FctVertex::FctVertex(
	int id,
	int nb_basic_blocks,
	int nb_internal_edges,
        int nb_outgoing_calls,
	int nb_incomming_calls,
	Address addr
)
  : _id(id),
    _nb_basic_blocks(nb_basic_blocks),
    _nb_internal_edges(nb_internal_edges),
    _nb_outgoing_calls(nb_outgoing_calls),
    _nb_incomming_calls(nb_incomming_calls),
    _addr(addr)
{
}

BinSlay::ReverseAPI::FctVertex::~FctVertex()
{
}

//////////////////////////////////////////////////////////////////////////////////////////////

BinSlay::ReverseAPI::BasicBlockBVertex::BasicBlockBVertex(
	int id,
	int alpha,
	int beta,
	int gamma,
	int nb_edges_in,
	int nb_edges_out,
	int nb_instrs,
	unsigned int crc32,
	Address addr
)
  : _id(id),
    _nb_jump_from_entry(alpha),
    _nb_jump_to_exit(beta),
    _nb_outgoing_calls(gamma),
    _nb_edges_in(nb_edges_in),
    _nb_edges_out(nb_edges_out),
    _nb_instrs(nb_instrs),
    _crc32(crc32),
    _addr(addr)

{
}

BinSlay::ReverseAPI::BasicBlockBVertex::~BasicBlockBVertex()
{
}

//////////////////////////////////////////////////////////////////////////////////////////////

BinSlay::ReverseAPI::Instr::Instr(
	BinSlay::ReverseAPI::Address offset,
	std::string raw,
	std::string disassembly
)
  : _offset(offset),
    _raw(raw),
    _disassembly(disassembly)
{
}

BinSlay::ReverseAPI::Instr::~Instr()
{
}

//////////////////////////////////////////////////////////////////////////////////////////////

BinSlay::ReverseAPI::DyninstBin::DyninstBin(std::string path)
  : _file_name(path)
{
#ifdef BINSLAYER_DEBUG
  std::cerr << "# Use of DyninstAPI" << std::endl;
  std::cerr << "# Binary name: " << path << std::endl;
#endif

  // Create the 'CodeObject' data
  this->_sts = new Dyninst::ParseAPI::SymtabCodeSource((char *)path.c_str());
  this->_co = new Dyninst::ParseAPI::CodeObject(this->_sts/*, NULL, NULL, true*/);

  // Parse the binary
  this->_co->parse();
  this->_co->finalize();

#ifdef BINSLAYER_DEBUG
  std::cerr << "# Number of functions found: " << std::dec << this->_co->funcs().size() << std::endl;
  for (Dyninst::ParseAPI::CodeObject::funclist::iterator it_fct =
	 this->_co->funcs().begin(); it_fct != this->_co->funcs().end(); ++it_fct)
    {
      std::cout << "Name: " << (*it_fct)->name()
		<< " - nb basic blocks: " << std::dec << (*it_fct)->blocks().size()
		<< std::endl;
    }
#endif
  // TODO: raise exception if DyninstAPI failed to recover functions
}

BinSlay::ReverseAPI::DyninstBin::~DyninstBin()
{
  delete this->_sts;
  delete this->_co;
}

std::string const &
BinSlay::ReverseAPI::DyninstBin::getFileName() const
{
  return this->_file_name;
}

BinSlay::ReverseAPI::CG *
BinSlay::ReverseAPI::DyninstBin::recover_call_graph() const
{
  // Create a Call-Graph data structure, used to communicate between the DynInst dll
  // and the BinSlayer core engine.
  BinSlay::ReverseAPI::CG *cg = new BinSlay::ReverseAPI::CG;

  // Record already visited blocks
  std::map<Dyninst::Address, bool> seen;

  // Save the CG in .dot file format
  std::fstream fs((this->_file_name + ".dot").c_str(),
		  std::fstream::in | std::fstream::out | std::fstream::trunc);
  fs << "digraph " << this->_file_name << " {" << std::endl;

  // Get all the functions from the code
  Dyninst::ParseAPI::CodeObject::funclist &all = this->_co->funcs();

  // Resize the Call-Graph data structure to the number of functions found
  cg->resize(all.size());

  Dyninst::ParseAPI::CodeObject::funclist::iterator fit = all.begin();
  // For each function found...
  unsigned int nb_nodes = 0;
  for (; fit != all.end(); ++fit) {
    Dyninst::ParseAPI::Function *f = *fit;
    Dyninst::ParseAPI::Function::blocklist::iterator bit = f->blocks().begin();

      // // //std::cout << "Fname: " << f->name() << std::endl;
      // if (f->name().find("targ", 0, 4) != std::string::npos)
      // 	{
      // 	  std::cout << f->name() << std::endl;
      // 	  //      	  continue;
      // 	}

      // Get nb internal edges in the CFG of the function
      unsigned int nb_internal_edges = 0;
      for(Dyninst::ParseAPI::Function::blocklist::iterator it_block =
	    f->blocks().begin(); it_block != f->blocks().end(); ++it_block) {
	for (Dyninst::ParseAPI::Block::edgelist::iterator it_edge =
	       (*it_block)->targets().begin(); it_edge != (*it_block)->targets().end();
	     ++it_edge) {
	  if ((*it_edge)->type() != Dyninst::ParseAPI::CALL &&
	      (*it_edge)->type() != Dyninst::ParseAPI::RET)
	      	++nb_internal_edges;
	    }
	}

      // Get all incomming edges to this function (all CALLs to this function)
      // We need this for the computation of the GED.
      unsigned int nb_incomming_calls = 0;
      {
	std::map<Dyninst::Address, bool> visited;

	for(Dyninst::ParseAPI::Block::edgelist::iterator it_edge = f->entry()->sources().begin();
	    it_edge != f->entry()->sources().end(); ++it_edge) {
	  // Do not revisit same edges in shared code
	  if (visited.find((*it_edge)->src()->start()) != visited.end())
	    continue;
	  visited[(*it_edge)->src()->start()] = true;

	  if ((*it_edge)->type() == Dyninst::ParseAPI::CALL)
	    ++nb_incomming_calls;
	}
      }

      // Get all outcomming edges from this function (all CALLs in this function)
      unsigned int nb_outcomming_calls = 0;
      {
	std::map<Dyninst::Address, bool> visited;

	for(Dyninst::ParseAPI::Function::edgelist::iterator it_edge = f->callEdges().begin();
	    it_edge != f->callEdges().end(); ++it_edge) {
	  // Do not revisit same edges in shared code
	  if (visited.find((*it_edge)->src()->start()) != visited.end())
	    continue;
	  visited[(*it_edge)->src()->start()] = true;
  
	  if ((*it_edge)->type() == Dyninst::ParseAPI::CALL)
	    ++nb_outcomming_calls;
	}
      }

      //
      // TODO: Handle the problem of static and dynamic library call
      //

      (*cg)[nb_nodes]= new BinSlay::ReverseAPI::FctVertex(
		       	nb_nodes,
			f->blocks().size(),
			nb_internal_edges,
			nb_outcomming_calls,
			nb_incomming_calls,
			f->entry()->start()
       		       );

      // Increase the number of node (one node => one function)
      ++nb_nodes;

      // For each basic block in the current function do...
      for(; bit != f->blocks().end(); ++bit) {
	Dyninst::ParseAPI::Block *b = *bit;
	// Don’t revisit blocks in shared code
	if (seen.find(b->start()) != seen.end())
	  continue;	  
	seen[b->start()] = true;
	Dyninst::ParseAPI::Block::edgelist::iterator it = b->targets().begin();
	// For each edges which points out of the current block,
	// check for function calls
	for (; it != b->targets().end(); ++it) {
	  // To recover the CALL GRAPH, we are only interested in edges
	  // which type is Dyninst::ParseAPI::CALL
	  if ((*it)->type() == Dyninst::ParseAPI::CALL) {
	    // Debug
	    fs << "\t\t\"" << std::hex << f->entry()->start() << "\" -> \""
	       << (*it)->trg()->start() << "\"" << " [color=blue]" << std::endl;
	    // Add link in our call graph data
	    (*cg)[nb_nodes - 1]->_link_to.push_front((*it)->trg()->start());
	  }
	}
      }
    }
  fs << "}" << std::endl;
  fs.close();
  return cg;
}

void BinSlay::ReverseAPI::DyninstBin::build_cfg(
		    Dyninst::ParseAPI::Function *f,
		    BinSlay::ReverseAPI::CFG *cfg,
		    std::list<Dyninst::ParseAPI::Block *> *blocks_list,
		    std::map<Dyninst::Address, bool> &seen,
		    int &level_of_depth,
		    int &nb_basic_blocks
	       ) const
{
  std::list<Dyninst::ParseAPI::Block *> *children_list  =
    new std::list<Dyninst::ParseAPI::Block *>;

  for (std::list<Dyninst::ParseAPI::Block *>::iterator it = blocks_list->begin();
       it != blocks_list->end(); ++it)
    {
      // Don’t revisit blocks in shared code (otherwise infinite loop)
      if (seen.find((*it)->start()) != seen.end())
	continue;
      seen[(*it)->start()] = true;

      int _nb_jump_from_entry = level_of_depth;
      int _nb_jump_to_exit = 0;
      int _nb_outgoing_calls = 0;	
      // Search for children and for outgoing call
      for(Dyninst::ParseAPI::Block::edgelist::iterator it_edge = (*it)->targets().begin();
      	  it_edge != (*it)->targets().end(); ++it_edge)
      	{
	  // Children blocks
      	  if ((*it_edge)->type() != Dyninst::ParseAPI::CALL &&
      	      (*it_edge)->type() != Dyninst::ParseAPI::RET)
      	    {
      	      if (f->contains((*it_edge)->trg()))
		{
		  // if (seen.find((*it_edge)->trg()->start()) == seen.end())
		  children_list->push_front((*it_edge)->trg());
		}
      	    }
	  // outgoing call
      	  if ((*it_edge)->type() == Dyninst::ParseAPI::CALL)
      	    ++_nb_outgoing_calls;
      	}
      
      // Add function node to our call graph data
      // std::cout << "New basic block: " << "idx = " << nb_basic_blocks
      // 		<< " - addr = " << std::hex << (*it)->start() << std::endl;
      (*cfg)[nb_basic_blocks] = new BinSlay::ReverseAPI::BasicBlockBVertex(
					nb_basic_blocks,
					_nb_jump_from_entry,
					_nb_jump_to_exit,
					_nb_outgoing_calls,
					(*it)->sources().size(),
					(*it)->targets().size(),
					get_basic_block_nbInstrs((*it)->start()),
					get_basic_block_crc32((*it)->start()),
					(*it)->start()
				 );
      // Link basic current block to other
      Dyninst::ParseAPI::Block::edgelist::iterator it_edge = (*it)->targets().begin();
      for (; it_edge != (*it)->targets().end(); ++it_edge)
	{
	  // To recover the CFG, we are not interested in edges
	  // which type is Dyninst::ParseAPI::CALL and Dyninst::ParseAPI::RET
	  if ((*it_edge)->type() != Dyninst::ParseAPI::CALL &&
	      (*it_edge)->type() != Dyninst::ParseAPI::RET)
	    {
	      (*cfg)[nb_basic_blocks]->_link_to.push_front((*it_edge)->trg()->start());
	    }
	}
      ++nb_basic_blocks;
    }
  
  delete blocks_list;

  // If the children list is empty, we have reached the end of the function
  if (!children_list->size())
    return;

  // Recursion to the children nodes UP->DOWN.
  build_cfg(f, cfg, children_list, seen, ++level_of_depth, nb_basic_blocks);
}

BinSlay::ReverseAPI::CFG *BinSlay::ReverseAPI::DyninstBin::recover_function_cfg(
	  Dyninst::Address addr) const
{
  Dyninst::ParseAPI::Function *f = NULL;
  BinSlay::ReverseAPI::CFG *cfg = new BinSlay::ReverseAPI::CFG;
  Dyninst::ParseAPI::CodeObject::funclist &all = this->_co->funcs();
  Dyninst::ParseAPI::CodeObject::funclist::iterator fit = all.begin();

  // Retrieve a ptr on the desired 'Function' object
  for (; fit != all.end(); ++fit)
    {
      if ((*fit)->entry()->start() == addr)
	{
	  f = *fit;
	  break;
	}
    }
  
  // Construct the CFG of the function
  std::map<Dyninst::Address, bool> seen;
  int level_of_depth = 0;
  int nb_basic_blocks = 0;
  std::list<Dyninst::ParseAPI::Block *> *blocks_list =
    new std::list<Dyninst::ParseAPI::Block *>;
  std::map<Dyninst::Address, bool> s;

  cfg->resize(f->blocks().size());
  blocks_list->push_front(f->entry());
  build_cfg(f, cfg, blocks_list, seen, level_of_depth, nb_basic_blocks);

  // Add the '_nb_jump_to_exit' label to all the basic blocks in the newly recovered CFG
  // First, find all the returning blocks
  std::vector<Dyninst::ParseAPI::Block *> *blist =
    new std::vector<Dyninst::ParseAPI::Block *>;
  {
    for(Dyninst::ParseAPI::Function::blocklist::iterator it_block = f->blocks().begin();
  	it_block != f->blocks().end(); ++it_block)
      {
  	// Check for a returning block
  	for(Dyninst::ParseAPI::Block::edgelist::iterator it_edge =
  	      (*it_block)->targets().begin(); it_edge != (*it_block)->targets().end();
  	    ++it_edge)
  	  {
  	    if ((*it_edge)->type() == Dyninst::ParseAPI::RET)
  	      {
  		// Don’t revisit blocks in shared code (otherwise infinite loop)
  		if (s.find((*it_edge)->src()->start()) != s.end())
  		  continue;
  		s[(*it_block)->start()] = true;	      
  		blist->push_back(*it_block);
  	      }
  	  }
      }
  }

  level_of_depth = 0;
  //  Then, start from the returning block and go up until
  // we reach the starting block
  label_nb_jump_to_exit(f, cfg, s, blist, level_of_depth);
  return cfg;
}

void BinSlay::ReverseAPI::DyninstBin::label_nb_jump_to_exit(
		      Dyninst::ParseAPI::Function *f,
		      BinSlay::ReverseAPI::CFG *cfg,
		      std::map<Dyninst::Address, bool> &s,
		      std::vector<Dyninst::ParseAPI::Block *> *blist,
		      int &level) const
{
  std::vector<Dyninst::ParseAPI::Block *> *parent_list =
    new std::vector<Dyninst::ParseAPI::Block *>;

  for (std::vector<Dyninst::ParseAPI::Block *>::iterator it_block = blist->begin();
       it_block != blist->end(); ++it_block)
    {
      // Get parent blocks
      if ((*it_block)->start() != f->entry()->start())
	for(Dyninst::ParseAPI::Block::edgelist::iterator it_edge =
	      (*it_block)->sources().begin(); it_edge != (*it_block)->sources().end();
	    ++it_edge)
	  {
	    if ((*it_edge)->type() != Dyninst::ParseAPI::RET &&
		(*it_edge)->type() != Dyninst::ParseAPI::CALL)
	      {
		// Don’t revisit blocks in shared code
		if (s.find((*it_edge)->src()->start()) != s.end())
		  continue;
		s[(*it_edge)->src()->start()] = true;
		parent_list->push_back((*it_edge)->src());
	      }
	  }

      // Set the '_nb_jump_to_exit'
      for (unsigned int i = 0; i < cfg->size(); ++i)
	{
	  if ((*cfg)[i]->_addr == (*it_block)->start())
	    (*cfg)[i]->_nb_jump_to_exit = level;
	}
    }
  ++level;
  delete blist;

  if (parent_list->size())
    label_nb_jump_to_exit(f, cfg, s, parent_list, level);
}

static unsigned int crc32table[256] =	{

	0x00000000, 0x77073096, 0xee0e612c, 0x990951ba,
	0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
	0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
	0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
	0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
	0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
	0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,
	0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
	0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
	0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
	0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940,
	0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
	0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116,
	0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
	0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
	0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,

	0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a,
	0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
	0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818,
	0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
	0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
	0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
	0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c,
	0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
	0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
	0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
	0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
	0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
	0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086,
	0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
	0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4,
	0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,

	0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
	0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
	0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
	0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
	0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe,
	0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
	0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
	0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
	0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252,
	0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
	0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60,
	0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
	0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
	0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
	0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04,
	0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,

	0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,
	0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
	0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
	0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
	0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e,
	0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
	0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
	0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
	0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
	0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
	0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0,
	0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
	0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6,
	0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
	0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
	0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d,
};

unsigned int BinSlay::ReverseAPI::DyninstBin::get_function_size(Address addr) const
{
  Dyninst::ParseAPI::Function *f = NULL;
  Dyninst::ParseAPI::CodeObject::funclist &all = this->_co->funcs();
  Dyninst::ParseAPI::CodeObject::funclist::iterator fit = all.begin();

  // Retrieve a ptr on the desired 'Function' object
  for (; fit != all.end(); ++fit)
    {
      if ((*fit)->entry()->start() == addr)
	{
	  f = *fit;
	  break;
	}
    }
  // "The blocks are guaranteed to be sorted by starting address"
  // Get the low address of this function
  unsigned char *fstart = (unsigned char *)(this->_sts->getPtrToInstruction(f->entry()->start()));
  // Get the last block
  Dyninst::ParseAPI::Function::blocklist::iterator last;
  for(Dyninst::ParseAPI::Function::blocklist::iterator it_block = f->blocks().begin();
      it_block != f->blocks().end(); ++it_block)
    {
      last = it_block;
    }
  // Get the high address of this function
  unsigned char *fend = (unsigned char *)(this->_sts->getPtrToInstruction((*last)->end()));
  return fend - fstart;
}

BinSlay::ReverseAPI::Crc32 BinSlay::ReverseAPI::DyninstBin::get_function_crc32(Address addr) const
{
  Dyninst::ParseAPI::Function *f = NULL;
  Dyninst::ParseAPI::CodeObject::funclist &all = this->_co->funcs();
  Dyninst::ParseAPI::CodeObject::funclist::iterator fit = all.begin();

  // Retrieve a ptr on the desired 'Function' object
  for (; fit != all.end(); ++fit)
    {
      if ((*fit)->entry()->start() == addr)
	{
	  f = *fit;
	  break;
	}
    }
  // "The blocks are guaranteed to be sorted by starting address"
  // Get the low address of this function
  unsigned char *fstart = (unsigned char *)(this->_sts->getPtrToInstruction(f->entry()->start()));
  // Get the last block
  Dyninst::ParseAPI::Function::blocklist::iterator last;
  for(Dyninst::ParseAPI::Function::blocklist::iterator it_block = f->blocks().begin();
      it_block != f->blocks().end(); ++it_block)
    {
      last = it_block;
    }
  // Get the high address of this function
  unsigned char *fend = (unsigned char *)(this->_sts->getPtrToInstruction((*last)->end()));
  if (fend) // Why fend == 0 sometimes ???
    {
      // Allocate and create a decoder
      Dyninst::InstructionAPI::InstructionDecoder *decoder =
	new Dyninst::InstructionAPI::InstructionDecoder(
	   this->_sts->getPtrToInstruction(f->entry()->start()),
	   fend - fstart,
	   this->_sts->getArch()
  	);

      // Compute the CRC32 of this function based on the opcodes
      unsigned int crc = ~0; // the initial value of a CRC32 is 0xFFFFFFFF.
      Dyninst::InstructionAPI::Instruction::Ptr instr;
      while ((instr = decoder->decode()))
	{
	  crc = (crc >> 8) ^ crc32table[instr->getOperation().getID() ^ (crc & 0xFF)];
	}
      delete decoder;
      return crc;
    }
  return 0;
}

unsigned int BinSlay::ReverseAPI::DyninstBin::get_basic_block_nbInstrs(Address addr) const
{
  // Get a ptr to an instance of 'Dyninst::ParseAPI::Block' corresponding to this current
  // basic block
  std::set<Dyninst::ParseAPI::CodeRegion *> regions;
  this->_co->cs()->findRegions(addr, regions);
  Dyninst::ParseAPI::Block *block = this->_co->findBlockByEntry(*regions.begin(), addr);

  // Allocate and create a decoder
  Dyninst::InstructionAPI::InstructionDecoder *decoder =
    new Dyninst::InstructionAPI::InstructionDecoder(
	   this->_sts->getPtrToInstruction(block->start()),
	   block->end() - block->start(),
	   this->_sts->getArch()
  	);

  unsigned int nbInstrs = 0;
  Dyninst::InstructionAPI::Instruction::Ptr instr;
  while ((instr = decoder->decode()))
    {
      ++nbInstrs;
    }
  delete decoder;
  return nbInstrs;
}

BinSlay::ReverseAPI::Crc32 BinSlay::ReverseAPI::DyninstBin::get_basic_block_crc32(Address addr) const
{
  // Get a ptr to an instance of 'Dyninst::ParseAPI::Block' corresponding to this current
  // basic block
  std::set<Dyninst::ParseAPI::CodeRegion *> regions;
  this->_co->cs()->findRegions(addr, regions);
  Dyninst::ParseAPI::Block *block = this->_co->findBlockByEntry(*regions.begin(), addr);

  // Allocate and create a decoder
  Dyninst::InstructionAPI::InstructionDecoder *decoder =
    new Dyninst::InstructionAPI::InstructionDecoder(
	   this->_sts->getPtrToInstruction(block->start()),
	   block->end() - block->start(),
	   this->_sts->getArch()
  	);

  // Compute the CRC32 of this basic blocks based on the opcodes
  unsigned int crc = ~0; // the initial value of a CRC32 is 0xFFFFFFFF.
  Dyninst::InstructionAPI::Instruction::Ptr instr;
  while ((instr = decoder->decode()))
    {
      crc = (crc >> 8) ^ crc32table[instr->getOperation().getID() ^ (crc & 0xFF)];
    }
  delete decoder;
  return crc;  
}

BinSlay::ReverseAPI::SymName BinSlay::ReverseAPI::DyninstBin::get_function_symname(Address addr) const
{
  // Retrieve a ptr on the desired 'Function' object
  for (Dyninst::ParseAPI::CodeObject::funclist::iterator fit = this->_co->funcs().begin();
       fit != this->_co->funcs().end(); ++fit)
    {
      if ((*fit)->entry()->start() == addr)
	{
	  if ((*fit)->name().substr(0, 4) != "targ")
	    return (*fit)->name();
	}
    }
  return "";
}

BinSlay::ReverseAPI::INSTR_LIST *
BinSlay::ReverseAPI::DyninstBin::recover_basic_block_instr(
		BinSlay::ReverseAPI::Address addr) const
{
  // Data to be returned by this function
  BinSlay::ReverseAPI::INSTR_LIST *instrs = new BinSlay::ReverseAPI::INSTR_LIST;

  // Get a ptr to an instance of 'Dyninst::ParseAPI::Block' corresponding to this current
  // basic block
  std::set<Dyninst::ParseAPI::CodeRegion *> regions;
  this->_co->cs()->findRegions(addr, regions);
  Dyninst::ParseAPI::Block *block = this->_co->findBlockByEntry(*regions.begin(), addr);

  // We will now use BeaEngine disassembler to recovert the instructions because it is a little
  // too complicated with DyninstAPI: it does TOO MUCH and moreover, I had some issues to fully
  // recover instructions on my linux plateforme (e.g. register with [NAME_NOT_FOUND], while
  // objdump has any problem. BeaEngine looks great and it is fully cross-plateform.
  // Main structure of the beaengine
  DISASM MyDisasm;
  int Error = 0;
  int len = 0;

  // Zero this structure to initialize it
  (void)memset(&MyDisasm, 0, sizeof(DISASM));

  // Set the start address of the disassembly
  uintptr_t bbStart = reinterpret_cast<uintptr_t>(this->_sts->getPtrToInstruction(block->start()));
  uintptr_t bbEnd = reinterpret_cast<uintptr_t>(this->_sts->getPtrToInstruction(block->end()));

  MyDisasm.EIP = bbStart;
  //  UInt64 EndCodeSection = (UInt64)bbEnd;
  // std::cout << "Start: " << std::hex << MyDisasm.EIP << " - End:" << EndCodeSection << std::endl;

  // Disassembly loop
  while (!Error) {
    // Set "Security block"
    MyDisasm.SecurityBlock = bbEnd - bbStart;
 
    len = Disasm(&MyDisasm);
    if (len == OUT_OF_BLOCK || len == UNKNOWN_OPCODE) {
      Error = 1;
    } else {
      // raw bytes of the instruction
      std::stringstream raw;
      for (size_t i = 0; i < static_cast<size_t>(len); ++i) {
	raw << std::hex << std::setw(2) << setfill('0')
	    << (int)*(reinterpret_cast<unsigned char *>(bbStart)) << " ";
      }
      // Add a 'Instr' object in the list
      instrs->push_back(new BinSlay::ReverseAPI::Instr(
       			    addr,
       			    raw.str(),
       			    std::string(MyDisasm.CompleteInstr)
        		));
      
      MyDisasm.EIP = MyDisasm.EIP + len;
      if (MyDisasm.EIP >= bbEnd) {
	Error = 1;
      }
      // Do not forget to update the fstart and addr value
      bbStart += len;
      addr += len;
    }
  }
  return instrs;
}

BinSlay::ReverseAPI::INSTR_LIST *
BinSlay::ReverseAPI::DyninstBin::recover_function_instr(
		BinSlay::ReverseAPI::Address addr) const
{
  // Data to be returned by this function
  BinSlay::ReverseAPI::INSTR_LIST *instrs = new BinSlay::ReverseAPI::INSTR_LIST;

  Dyninst::ParseAPI::Function *f = NULL;
  Dyninst::ParseAPI::CodeObject::funclist &all = this->_co->funcs();
  Dyninst::ParseAPI::CodeObject::funclist::iterator fit = all.begin();

  // Retrieve a ptr on the desired 'Function' object
  for (; fit != all.end(); ++fit) {
    if ((*fit)->entry()->start() == addr) {
      f = *fit;
      break;
    }
  }

  // "The blocks are guaranteed to be sorted by starting address"
  // Get the low address of this function
  unsigned char *fstart = (unsigned char *)(this->_sts->getPtrToInstruction(f->entry()->start()));

  // Get the last block
  Dyninst::ParseAPI::Function::blocklist::iterator last;
  for(Dyninst::ParseAPI::Function::blocklist::iterator it_block = f->blocks().begin();
      it_block != f->blocks().end(); ++it_block)
    {
      // TODO: Cmp the addr and return the high
      last = it_block;
    }

  // Get the high address of this function
  unsigned char *fend = (unsigned char *)(this->_sts->getPtrToInstruction((*last)->end()));

  //
  // We will now use BeaEngine disassembler to recovert the instructions because it is a little
  // too complicated with DyninstAPI: it does TOO MUCH and moreover, I had some issues to fully
  // recover instructions on my linux plateforme (e.g. register with [NAME_NOT_FOUND], while
  // objdump has any problem. BeaEngine looks great and it is fully cross-plateform.
  //

  // Main structure of the beaengine
  DISASM MyDisasm;
  int Error = 0;
  int len = 0;

  // Zero this structure to initialize it
  (void)memset(&MyDisasm, 0, sizeof(DISASM));

  // Set the start address of the disassembly
  MyDisasm.EIP = (UIntPtr)fstart;
  UInt64 EndCodeSection = (UInt64)fend;

  // Disassembly loop
  while (!Error){
    // Set "Security block"
    MyDisasm.SecurityBlock = (UIntPtr)EndCodeSection - (UIntPtr)MyDisasm.EIP;
 
    len = Disasm(&MyDisasm);
    if (len == OUT_OF_BLOCK)
      {
	Error = 1;
      }
    else if (len == UNKNOWN_OPCODE)
      {
	Error = 1;
      }
    else
      {
	// raw bytes of the instruction
	std::stringstream raw;
	for (int i = 0; i < len; i++)
	  raw << std::hex << std::setw(2) << setfill('0')
	      << (int)(fstart[i]) << " ";
	
	// Add a 'Instr' object in the list
	instrs->push_back(new BinSlay::ReverseAPI::Instr(
				    addr,
				    raw.str(),
				    std::string(MyDisasm.CompleteInstr)
			 ));
	
	MyDisasm.EIP = MyDisasm.EIP + (UIntPtr)len;
	if (MyDisasm.EIP >= (unsigned long)fend)
	  {
	    Error = 1;
	  }
	// Do not forget to update the fstart and addr value
	fstart += len;
	addr += len;
      }
  }
  return instrs;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __linux__
# define EXPORT
#else
# define EXPORT __declspec(dllexport)
#endif

extern "C"
{
  EXPORT BinSlay::ReverseAPI::IBinary *get_instance_of_binary(std::string const &file)
  {
     return new BinSlay::ReverseAPI::DyninstBin(file);
  }
}
