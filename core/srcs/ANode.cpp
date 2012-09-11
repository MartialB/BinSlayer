#include "ANode.hh"

BinSlay::ANode::ANode()
{
}

BinSlay::ANode::ANode(
	      unsigned int idx,
	      unsigned long addr,
	      int x, int y, int z,
	      BinSlay::ReverseAPI::Crc32 crc32,
	      unsigned int size)
  : _idx(idx),
    _addr(addr),
    _label(BinSlay::Label(x, y, z)),
    _crc32(crc32),
    _size(size)
{
}


// BinSlay::ANode::ANode(BinSlay::ANode const &)
// {
  
// }


BinSlay::ANode::~ANode()
{
}


// BinSlay::ANode& BinSlay::ANode::operator=(BinSlay::ANode const &o)
// {
// }

void BinSlay::ANode::displayLabel() const
{
  this->_label.displayLabel();
}

