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
  load_graphs();
}

BinSlay::CFG_Core::~CFG_Core()
{
  delete this->_left;
  delete this->_right;
}

bool
BinSlay::CFG_Core::load_graphs()
{
  if (!this->_load_graph_left()) return false;
  if (!this->_load_graph_right()) return false;
  return true;
}

bool
BinSlay::CFG_Core::_load_graph_left()
{
  if (!_graph_left) {
    _left = new BinSlay::CFG(&_bin_left, _fct_left);
    _graph_left = _left;
  }
  return true;
}

bool
BinSlay::CFG_Core::_load_graph_right()
{
  if (!_graph_right) {
    _right = new BinSlay::CFG(&_bin_right, _fct_right);
    _graph_right = _right;
  }
  return true;
}

bool
BinSlay::CFG_Core::run_bindiff_algorithm(
	BinSlay::DiffingLevel level
)
{
#ifdef MEASURE_PERFORMANCE
  timespec start, end;
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
#endif // !MEASURE_PERFORMANCE
  // Run the algorithm at the CFG level
  BinSlay::ACore<BinSlay::BbNode>::run_bindiff_algorithm();
  // Run it at the bb level if desired to be more accurate
  if (level == BinSlay::DiffingLevel::BASIC_BLOCK) {
    this->_run_bindiff_at_basic_block_level();
  }
#ifdef MEASURE_PERFORMANCE
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
  std::cerr << "Bindiff time: ";
  if ((end.tv_nsec-start.tv_nsec) < 0) {
    std::cerr << end.tv_sec - start.tv_sec - 1 << ":";
    std::cerr << 1000000000 + end.tv_nsec - start.tv_nsec << std::endl;;
  } else {
    std::cerr << end.tv_sec - start.tv_sec << ":";
    std::cerr << end.tv_nsec - start.tv_nsec << std::endl;
  }
#endif // !MEASURE_PERFORMANCE
  return true;
}

bool
BinSlay::CFG_Core::_run_bindiff_at_basic_block_level()
{
  // // We should used the same generic properties and selectors as the ones
  // // used at the function level
  // typename BinSlay::bind_node<BinSlay::BbNode>::SELECTORS cfg_selectors;
  // typename BinSlay::bind_node<BinSlay::BbNode>::PROPERTIES cfg_properties;
  // cfg_selectors.resize(3);
  // cfg_properties.resize(2);
  // // No symname selector at the basic block level
  // cfg_selectors[BinSlay::idSelectors::NAME] = nullptr;
  // cfg_properties[BinSlay::idProperties::UP] = new BinSlay::UpProperty<BinSlay::BbNode>;
  // cfg_properties[BinSlay::idProperties::DOWN] = new BinSlay::DownProperty<BinSlay::BbNode>;
  // cfg_selectors[BinSlay::idSelectors::CFG] = new BinSlay::BindiffSelector<BinSlay::BbNode>;
  // cfg_selectors[BinSlay::idSelectors::CRC32] = new BinSlay::Crc32Selector<BinSlay::BbNode>;

  // for (auto it_map = _mapping.begin(); it_map != _mapping.end(); ++it_map) {
  //   for (auto it_iso = (*it_map)->begin(); it_iso != (*it_map)->end();/* no incrementation here */) {
  //     double similarity = this->run_bindiff_on_two_functions(
  // 		(*it_iso)->getLeft()->getAddr(),

  // 		(*it_iso)->getRight()->getAddr(),
  // 		cfg_selectors,
  // 		cfg_properties
  //     );
  //     // Two cfg are identical if they have the same number of nodes and if
  //     // all their nodes are isomorphic.
  //     if (similarity < 100.0) {
  // 	// if they are not, we remove this couple from the isoList because these
  // 	// two functions are not isomorphic at the CFG level.
  // 	delete *it_iso;
  // 	it_iso = (*it_map)->erase(it_iso);
  // 	// TODO: Need to add them to the '_l_left' and '_l_right' lists too !
  //     } else {
  // 	++it_iso;
  //     }
  //   }
  // }
  // // Delete selectors
  // for (auto it = cfg_selectors.begin(); it != cfg_selectors.end(); ++it)
  //   delete *it;  
  // // Delete properties
  // for (auto it = cfg_properties.begin(); it != cfg_properties.end(); ++it)
  //   delete *it;
  // return true;
}

double
BinSlay::CFG_Core::run_bindiff_on_two_nodes(
	BinSlay::ReverseAPI::Address addr_left,
	BinSlay::ReverseAPI::Address addr_right
)
{
  
}
