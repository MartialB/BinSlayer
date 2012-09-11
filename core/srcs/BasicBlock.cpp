#include "BasicBlock.hh"

BinSlay::BasicBlock::BasicBlock(
			    BinSlay::ReverseAPI::IBinary *bin,
			    BinSlay::BbNode const &bb_node,
			    BinSlay::ReverseAPI::Address)
  : _binary(bin),
    _node(bb_node)
{
}

// BinSlay::BasicBlock::BasicBlock(BasicBlock const &)
// {
// }

BinSlay::BasicBlock::~BasicBlock()
{
}

// BinSlay::BasicBlock& operator=(BasicBlock const &)
// {
// }

BinSlay::BbNode const &BinSlay::BasicBlock::getNode() const
{
  return this->_node;
}

BinSlay::ReverseAPI::IBinary *BinSlay::BasicBlock::getBinary()
{
  return this->_binary;
}
