/**
 * \file Node.cpp
 * \author Martial Bourquin
 * \version 1.0
 * \date 30/06/2012
 */

#include "Node.hh"

BinSlay::Node::Node()
{
}

BinSlay::Node::Node(
	      unsigned int idx,
	      unsigned long addr,
	      int x, int y, int z,
	      BinSlay::ReverseAPI::Crc32 crc32,
	      unsigned int v1,
	      unsigned int v2,
	      unsigned int v3,
	      unsigned int size,
	      std::string const &name)
  : _idx(idx),
    _addr(addr),
    _label(BinSlay::Label(x, y, z)),
    _crc32(crc32),
    _nb_incomming_edges(v1),
    _nb_outcomming_edges(v2),
    _nb_instrs(v3),
    _size(size),
    _name(name)
{
  // this->_label.setLabel(x, y, z);
  // this->_nb_incomming_edges = v;
  // this->_nb_outcomming_edges = z;
  // this->_addr = addr;
  // this->_name = name;
  // this->_crc32 = crc32;
  // this->_size = size;
}


// BinSlay::Node::Node(BinSlay::Node const &)
// {
  
// }


BinSlay::Node::~Node()
{
}


// BinSlay::Node& BinSlay::Node::operator=(BinSlay::Node const &o)
// {
// }


// std::vector<int>& BinSlay::Node::setEdges()
// {
//   return this->_edges;
// }


// std::vector<int> const & BinSlay::Node::getEdges() const
// {
//   return this->_edges;
// }

void BinSlay::Node::displayLabel() const
{
  this->_label.displayLabel();
}

// BinSlay::Label const & BinSlay::Node::getLabel() const
// {
//   return this->_label;
// }


// int BinSlay::Node::getIdx() const
// {
//   return this->_idx;
// }

unsigned long BinSlay::Node::getAddr() const
{
  return this->_addr;
}


unsigned int BinSlay::Node::getSize() const
{
  return this->_addr;
}


std::string const &BinSlay::Node::getName() const
{
  return this->_name;
}


BinSlay::ReverseAPI::Crc32 BinSlay::Node::getCrc32() const
{
  return this->_crc32;
}


unsigned int BinSlay::Node::getInEdges() const
{
  return this->_nb_incomming_edges;
}


unsigned int BinSlay::Node::getOutEdges() const
{
  return this->_nb_outcomming_edges;
}
