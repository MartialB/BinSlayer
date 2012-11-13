#ifndef __DYNINSTBIN_HH__
# define __DYNINSTBIN_HH__

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

      virtual bool init();      
      virtual CG *recover_call_graph() const;
      virtual void delete_cg(CG *cg) const;
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

    private:
      unsigned int get_fct_nb_internals_edges(Dyninst::ParseAPI::Function &f) const;
      unsigned int get_fct_nb_incomming_calls(Dyninst::ParseAPI::Function &f) const;
      unsigned int get_fct_nb_outcomming_calls(Dyninst::ParseAPI::Function &f) const;
      Dyninst::ParseAPI::Function *get_fct_by_addr(Dyninst::Address addr) const;

      // void label_nb_jump_to_exit(
      //  	 Dyninst::ParseAPI::Function &f,
      // 	 BinSlay::ReverseAPI::CFG &cfg,
      // 	 std::map<Dyninst::Address, bool> &seen,
      // 	 std::vector<Dyninst::ParseAPI::Block *> &blist,
      // 	 unsigned int &level
      // ) const;
    };

  }
}

#endif // !__DYNINSTBIN_HH__
