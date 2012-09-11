#include "Function.hh"

BinSlay::Function::Function(
			    BinSlay::ReverseAPI::IBinary *bin,
			    BinSlay::FctNode const &fct_node,
			    BinSlay::ReverseAPI::Address addr)
  : BinSlay::CFG::CFG(bin, addr),
    _binary(bin),
    _node(fct_node)
{
}

// BinSlay::Function::Function(Function const &)
// {
// }

BinSlay::Function::~Function()
{
}

// BinSlay::Function& operator=(Function const &)
// {
// }

BinSlay::FctNode const &BinSlay::Function::getNode() const
{
  return this->_node;
}

BinSlay::ReverseAPI::IBinary *BinSlay::Function::getBinary()
{
  return this->_binary;
}
