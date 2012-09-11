#ifndef DYNINSTBIN_HH_
# define DYNINSTBIN_HH_

# include <list>
# include <set>
# include <string>
# include <vector>
# include "CodeSource.h"
# include "CodeObject.h"
# include "Visitor.h"
# include "BinaryFunction.h"
# include "Immediate.h"
# include "Register.h"
# include "Dereference.h"
# include "IBinary.hh"

namespace BinSlay {
  namespace ReverseAPI {

    class DyninstBin : public IBinary
    {
    public:
      DyninstBin(std::string path);
      virtual ~DyninstBin();
      
      virtual CG *recover_call_graph() const;
      virtual CFG *recover_function_cfg(Address function) const;
      virtual std::string const &getFileName() const;
      virtual Crc32 get_function_crc32(Address addr) const;
      virtual Crc32 get_basic_block_crc32(Address addr) const;
      virtual unsigned int get_basic_block_nbInstrs(Address addr) const;
      virtual SymName get_function_symname(Address addr) const;
      virtual unsigned int get_function_size(Address addr) const;
      virtual INSTR_LIST *recover_basic_block_instr(Address addr) const;
      virtual INSTR_LIST *recover_function_instr(Address addr) const;

    private:
      std::string _file_name;
      Dyninst::ParseAPI::SymtabCodeSource *_sts;
      Dyninst::ParseAPI::CodeObject *_co;

      void build_cfg(
		 Dyninst::ParseAPI::Function *f,
		 BinSlay::ReverseAPI::CFG *cfg,
		 std::list<Dyninst::ParseAPI::Block *> *blocks_list,
		 std::map<Dyninst::Address, bool> &seen,
		 int &level_of_depth,
		 int &nb_basic_blocks
      ) const;
      void label_nb_jump_to_exit(
		 Dyninst::ParseAPI::Function *f,
		 BinSlay::ReverseAPI::CFG *cfg,
		 std::map<Dyninst::Address, bool> &seen,
		 std::vector<Dyninst::ParseAPI::Block *> *blist,
		 int &level
      ) const;
    };
    
    // class Visitor : public Dyninst::InstructionAPI::Visitor
    // {
    // public:
    //   Visitor(Address eip);
    //   ~Visitor();
    //   virtual void visit(Dyninst::InstructionAPI::BinaryFunction *b);
    //   virtual void visit(Dyninst::InstructionAPI::Immediate *i);
    //   virtual void visit(Dyninst::InstructionAPI::RegisterAST *r);
    //   virtual void visit(Dyninst::InstructionAPI::Dereference *d);

    // // private:
    //   Address _eip;
    //   std::list<std::string>	_operands;
    //   std::list<std::string>	_operators;
    // };
  }
}

#endif // !DYNINSTBIN_HH_
