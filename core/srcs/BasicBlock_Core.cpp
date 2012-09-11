#include "BasicBlock_Core.hh"

BinSlay::BasicBlock_Core::BasicBlock_Core(
		    BinSlay::BasicBlock &bb_left,
		    BinSlay::BasicBlock &bb_right)
  : _basic_block_left(bb_left),
    _basic_block_right(bb_right)
{
}

// BinSlay::BasicBlock_Core::BasicBlock_Core(BinSlay::BasicBlock_Core const &o)
// {
  
// }

BinSlay::BasicBlock_Core::~BasicBlock_Core()
{
  delete &_basic_block_left;
  delete &_basic_block_right;
}

// BinSlay::BasicBlock_Core& BinSlay::BasicBlock_Core::operator=(BinSlay::BasicBlock_Core const &o)
// {
  
// }

// BinSlay::BasicBlock_Core::diff()
// {
// }

BinSlay::BasicBlock &BinSlay::BasicBlock_Core::getLeft()
{
  return this->_basic_block_left;
}

BinSlay::BasicBlock &BinSlay::BasicBlock_Core::getRight()
{
  return this->_basic_block_right;
}
