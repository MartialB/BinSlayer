#include "BB_Labelling.hh"

BinSlay::ReverseAPI::BB_Labelling::BB_Labelling(
	BinSlay::ReverseAPI::DyninstBin const &bin,
	BinSlay::ReverseAPI::CFG &cfg
)
  : _bin(bin),
    _cfg(cfg)
{
}

void
BinSlay::ReverseAPI::BB_Labelling::operator()(
	Dyninst::ParseAPI::Block &block,
	unsigned int level_of_depth
)
{
  // Set the '_nb_jump_to_exit'
  for (size_t i = 0; i < _cfg.size(); ++i) {
    if (_cfg[i]->_addr == block.start())
      _cfg[i]->_nb_jump_to_exit = level_of_depth;
  }
}

