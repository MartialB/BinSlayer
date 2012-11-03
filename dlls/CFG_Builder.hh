#ifndef __CFG_BUILDER_HH__
# define __CFG_BUILDER_HH__

# include "DyninstBin.hh"

namespace BinSlay {
  namespace ReverseAPI {

    class CFG_Builder
    {
    public:
      CFG_Builder(
		BinSlay::ReverseAPI::DyninstBin const &bin,
		BinSlay::ReverseAPI::CFG &cfg
      );

      void operator()(Dyninst::ParseAPI::Block &block, unsigned int level_of_depth);

    private:
      BinSlay::ReverseAPI::DyninstBin const	&_bin;
      BinSlay::ReverseAPI::CFG			&_cfg;
      size_t					_idx_block;
    };
  }
}

#endif // !__CFG_BUILDER_HH__
