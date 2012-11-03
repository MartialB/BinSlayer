#ifndef IBINARY_HH_
# define IBINARY_HH_

# include <list>
# include <map>
# include <string>
# include <vector>
# include <valarray>

//
// API to retrieve needed data from the binary. If you want to use another library than
// the default one used in this project (DyninstAPI), you can simply use this interface
// to implement your own Binary class. You can see the DyninstBin.hh and .cpp files to
// have an example.
//

namespace BinSlay {
  namespace ReverseAPI {

    typedef unsigned int Crc32;    
    typedef unsigned long Address;
    typedef std::string SymName;

    struct Instr
    {
      Instr(BinSlay::ReverseAPI::Address, std::string, std::string);
      ~Instr();
      BinSlay::ReverseAPI::Address	_offset;
      std::string			_raw;
      std::string			_disassembly;
    };

    typedef std::list<Instr *> INSTR_LIST;

    struct FctVertex
    {
      FctVertex(unsigned int id, unsigned int nb_basic_blocks, unsigned int nb_internal_edges,
		unsigned int nb_outgoing_calls, unsigned int nb_incomming_calls, Address addr);
      ~FctVertex();
      unsigned int _id;
      unsigned int _nb_basic_blocks;
      unsigned int _nb_internal_edges;
      unsigned int _nb_outgoing_calls;
      unsigned int _nb_incomming_calls;
      Address _addr;
      std::list<Address>  _link_to;
    };

    struct BasicBlockBVertex
    {
      BasicBlockBVertex(unsigned int id, unsigned int alpha, unsigned int beta, unsigned int gamma,
			unsigned int nb_edges_in, unsigned int nb_edges_out,
			unsigned int nb_instrs, unsigned int crc32, Address addr);
      ~BasicBlockBVertex();
      unsigned int _id;
      unsigned int _nb_jump_from_entry;
      unsigned int _nb_jump_to_exit;
      unsigned int _nb_outgoing_calls;
      unsigned int _nb_edges_in;
      unsigned int _nb_edges_out;
      unsigned int _nb_instrs;
      unsigned int _crc32;
      Address _addr;
      std::list<Address>  _link_to;
    };

    // TODO: Too long with files which contain more than 2000 functions (~25s for 10.000)
    typedef std::vector<FctVertex *> CG;
    typedef std::vector<BasicBlockBVertex *> CFG;

    class IBinary
    {
    public:
      virtual ~IBinary() {};
      virtual CG *recover_call_graph() const = 0;
      virtual CFG *recover_function_cfg(Address function) const = 0;
      virtual Crc32 get_function_crc32(Address addr) const = 0;
      virtual Crc32 get_basic_block_crc32(Address addr) const = 0;
      virtual unsigned int get_basic_block_nbInstrs(Address addr) const = 0;
      virtual SymName get_function_symname(Address addr) const = 0;
      virtual unsigned int get_function_size(Address addr) const = 0;
      virtual std::string const &getFileName() const = 0;
      virtual INSTR_LIST *recover_basic_block_instr(Address addr) const = 0;
      virtual INSTR_LIST *recover_function_instr(Address addr) const = 0;
    };
  }
}

#endif // !IBINARY_HH_
