#include "CFG_Core.hh"

BinSlay::CFG_Core::CFG_Core(
		BinSlay::ReverseAPI::IBinary &bin_left,
		BinSlay::ReverseAPI::IBinary &bin_right,
		BinSlay::ReverseAPI::Address const fct_left,
		BinSlay::ReverseAPI::Address const fct_right
)
  : BinSlay::ACore<BinSlay::BbNode>::ACore(bin_left, bin_right),
    _fct_left(fct_left),
    _fct_right(fct_right)
{
  // Init the CallGraph objects
  load_graphs();
}

// BinSlay::CFG_Core::CFG_Core(BinSlay::CFG_Core const &o)
// {
  
// }

BinSlay::CFG_Core::~CFG_Core()
{
}

bool BinSlay::CFG_Core::load_graphs()
{
  if (!load_graph_left())
    return false;
  if (!load_graph_right())
    return false;
  return true;
}

bool BinSlay::CFG_Core::load_graph_left()
{
  if (!_graph_left)
    _graph_left = new BinSlay::CFG(&_bin_left, _fct_left);
  return true;
}

bool BinSlay::CFG_Core::load_graph_right()
{
  if (!_graph_right)
    _graph_right = new BinSlay::CFG(&_bin_right, _fct_right);
  return true; 
}

// BinSlay::CFG_Core& BinSlay::CFG_Core::operator=(BinSlay::CFG_Core const &o)
// {
  
// }
