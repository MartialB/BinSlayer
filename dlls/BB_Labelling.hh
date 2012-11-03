#ifndef __BB_LABELLING_HH__
# define __BB_LABELLING_HH__

# include "DyninstBin.hh"

namespace BinSlay {
  namespace ReverseAPI {

    class BB_Labelling
    {
    public:
      BB_Labelling(
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

#endif // !__BB_LABELLING_HH__
