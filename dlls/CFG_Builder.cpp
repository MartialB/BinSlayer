#include "CFG_Builder.hh"

BinSlay::ReverseAPI::CFG_Builder::CFG_Builder(
	BinSlay::ReverseAPI::DyninstBin const &bin,
	BinSlay::ReverseAPI::CFG &cfg
)
  : _bin(bin), 
    _cfg(cfg),
    _idx_block(0)
{
}

void
BinSlay::ReverseAPI::CFG_Builder::operator()(
	Dyninst::ParseAPI::Block &block,
	unsigned int level_of_depth
)
{
  unsigned int nb_jump_from_entry = level_of_depth;
  unsigned int nb_jump_to_exit = 0;
  unsigned int nb_outgoing_calls = 0;	
  // Search for outgoing calls
  for(auto it_edge = block.targets().begin(); it_edge != block.targets().end();
      ++it_edge) {
    // Outgoing call
    if ((*it_edge)->type() == Dyninst::ParseAPI::CALL) {
      ++nb_outgoing_calls;
    }
  }
  // Add this basic block to the cfg data structure
  _cfg[this->_idx_block] =
    new BinSlay::ReverseAPI::BasicBlockBVertex(
		this->_idx_block,
		nb_jump_from_entry,
		nb_jump_to_exit,
		nb_outgoing_calls,
		block.sources().size(),
		block.targets().size(),
		_bin.get_basic_block_nbInstrs(block.start()),
		_bin.get_basic_block_crc32(block.start()),
		block.start()
    );
  // Link current basic block to others
  for (auto it_edge = block.targets().begin();
       it_edge != block.targets().end(); ++it_edge) {
    // To recover the fcg, we are not interested in edges
    // which type is Dyninst::ParseAPI::CALL and Dyninst::ParseAPI::RET
    if ((*it_edge)->type() != Dyninst::ParseAPI::CALL &&
	(*it_edge)->type() != Dyninst::ParseAPI::RET) {
      _cfg[this->_idx_block]->_link_to.push_front((*it_edge)->trg()->start());
    }
  }
  ++this->_idx_block;
}
