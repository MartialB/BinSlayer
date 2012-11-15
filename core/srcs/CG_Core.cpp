#include "CG_Core.hh"
#include <stdlib.h>

BinSlay::CG_Core::CG_Core(
	BinSlay::ReverseAPI::IBinary &_bin_left,
	BinSlay::ReverseAPI::IBinary &_bin_right
)
  : BinSlay::ACore<BinSlay::FctNode>::ACore(
		_bin_left,
		_bin_right
    )
{
  // BinDiff selectors & properties for the CFG level
  _cfg_selectors.resize(3);
  _cfg_properties.resize(2);
  _cfg_selectors[BinSlay::idSelectors::NAME] = nullptr;
  _cfg_properties[BinSlay::idProperties::UP] = new BinSlay::UpProperty<BinSlay::BbNode>;
  _cfg_properties[BinSlay::idProperties::DOWN] = new BinSlay::DownProperty<BinSlay::BbNode>;
  _cfg_selectors[BinSlay::idSelectors::CFG] = nullptr;
  //_cfg_selectors[BinSlay::idSelectors::CFG] = new BinSlay::BindiffSelector<BinSlay::BbNode>;
  _cfg_selectors[BinSlay::idSelectors::CRC32] = new BinSlay::Crc32Selector<BinSlay::BbNode>;
}

BinSlay::CG_Core::~CG_Core()
{
  delete this->_left;
  delete this->_right;
  this->delete_selectors<BinSlay::BbNode>(this->_cfg_selectors);
  this->delete_properties<BinSlay::BbNode>(this->_cfg_properties);
}

bool
BinSlay::CG_Core::load_graphs()
{
  if (!this->_load_graph_left()) return false;
  if (!this->_load_graph_right()) return false;
  return true;
}

bool
BinSlay::CG_Core::_load_graph_left()
{
  if (!_graph_left) {
    _left = new BinSlay::CallGraph(&_bin_left);
    _graph_left = _left;
  }
  return _left ? true : false;
}

bool
BinSlay::CG_Core::_load_graph_right()
{
  if (!_graph_right) {
    _right = new BinSlay::CallGraph(&_bin_right);
    _graph_right = _right;
  }
  return _right ? true : false;
}

bool
BinSlay::CG_Core::add_Selector(BinSlay::idSelectors::idSelectors_e id_selec)
{
  if (id_selec == BinSlay::idSelectors::NAME) {
    // If allocation of a selector fails, this is not critical, but this means
    // that this selector will not be available.
    _selectors[id_selec] = new BinSlay::SymNameSelector;
    IF_NULLPTR_RETURN_FALSE(_selectors[id_selec], BinSlay::Core_ErrorHandler::Status::BAD_ALLOC,
	"Warning: Bad allocation while trying to allocate a 'SymNameSelector' object.");
  } else {
    BinSlay::ACore<BinSlay::FctNode>::add_Selector(id_selec);
  }
}

bool
BinSlay::CG_Core::run_bindiff_algorithm(
	BinSlay::DiffingLevel level
)
{
#ifdef MEASURE_PERFORMANCE
  timespec start, end;
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
#endif // !MEASURE_PERFORMANCE
  // Run the algorithm at the call-graph level
  BinSlay::ACore<BinSlay::FctNode>::run_bindiff_algorithm();
  // Run it at the CFG level if desired to be more accurate
  if (level == BinSlay::DiffingLevel::FUNCTION) {
    this->_run_bindiff_at_cfg_level();
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
BinSlay::CG_Core::_run_bindiff_at_cfg_level()
{
  for (auto it_map = _mapping.begin(); it_map != _mapping.end(); ++it_map) {
    for (auto it_iso = (*it_map)->begin(); it_iso != (*it_map)->end();/* no incrementation here */) {
      double similarity = this->run_bindiff_on_two_functions(
		(*it_iso)->getLeft()->getAddr(),
		(*it_iso)->getRight()->getAddr()
      );
      // Two cfg are identical if they have the same number of nodes and if
      // all their nodes are isomorphic.
      if (similarity < 100.0) {
  	// if they are not, we remove this couple from the isoList because these
  	// two functions are not isomorphic at the CFG level.
  	delete *it_iso;
  	it_iso = (*it_map)->erase(it_iso);
  	// TODO: Need to add them to the '_l_left' and '_l_right' lists too !
      } else {
  	++it_iso;
      }
    }
  }
  return true;
}

double
BinSlay::CG_Core::run_bindiff_on_two_nodes(
	BinSlay::ReverseAPI::Address addr_left,
	BinSlay::ReverseAPI::Address addr_right
)
{
  return this->run_bindiff_on_two_functions(addr_left, addr_right);
}

// This function should be a functor which encapsulates the running of BinDiff and
// expose results.  
double
BinSlay::CG_Core::run_bindiff_on_two_functions(
	unsigned long addr_left,
	unsigned long addr_right
)
{
  // Create the CFG objects
  BinSlay::CFG *cfg_left = new BinSlay::CFG(&_bin_left, addr_left);
  BinSlay::CFG *cfg_right = new BinSlay::CFG(&_bin_right, addr_right);
  // Create the list of nodes for each graph
  typename BinSlay::bind_node<BinSlay::BbNode>::NODES_LIST cfg_l_left;
  typename BinSlay::bind_node<BinSlay::BbNode>::NODES_LIST cfg_l_right;
  cfg_left->CreateListOfNodes(cfg_l_left);
  cfg_right->CreateListOfNodes(cfg_l_right);
  // Create an instance of the Bindiff object
  auto *cfg_bindiff = new BinSlay::Bindiff<BinSlay::BbNode>(
				cfg_l_left,
				cfg_l_right,
				_cfg_selectors,
				_cfg_properties
		      );
  // Call the run method and retrieve the MAPPING
  auto *cfg_mapping = new typename BinSlay::bind_node<BinSlay::BbNode>::MAPPING;
  cfg_bindiff->run(*cfg_mapping);
  // Get the number of isomorphims found - TODO make function
  double cfg_nb_isomorphims = 0;
  for (auto it_map_cfg = cfg_mapping->begin(); it_map_cfg != cfg_mapping->end(); ++it_map_cfg)
    for (auto it_iso = (*it_map_cfg)->begin(); it_iso != (*it_map_cfg)->end(); ++it_iso)
      ++cfg_nb_isomorphims;
  // Get the percentage of similarty between the two compared graphs
  unsigned int max = cfg_left->getnbNode() <= cfg_right->getnbNode() ?
    cfg_right->getnbNode() : cfg_left->getnbNode();
  double similarity = static_cast<double>(cfg_nb_isomorphims) / static_cast<double>(max) * 100.0;
  // Clean the cfg_mapping - TODO: make function
  for (auto it_map_cfg = cfg_mapping->begin(); it_map_cfg != cfg_mapping->end(); ++it_map_cfg) {
    for (auto it_iso = (*it_map_cfg)->begin(); it_iso != (*it_map_cfg)->end(); ++it_iso)
      delete *it_iso;
    delete *it_map_cfg;
  }
  delete cfg_mapping;
  delete cfg_left;
  delete cfg_right;
  delete cfg_bindiff;
  return similarity;
}

// TODO: Faire une classe template sur le type de node pour le validator
bool
BinSlay::CG_Core::compute_ged(
	BinSlay::gedProperties::gedProperties_e options
)
{
#ifdef MEASURE_PERFORMANCE
  timespec start, end;
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
#endif // !MEASURE_PERFORMANCE

  // Run the GED algorithm
  BinSlay::ACore<BinSlay::FctNode>::compute_ged();
  // Get the Edit Path
  auto *tmp_ep = this->_ged->get_edit_path();

  if (options == BinSlay::gedProperties::NO_OPTIONS) {
    _ged->setEditPath(tmp_ep);
  } else {
    // Allocate the final edit path list
    auto *final_ep = new typename BinSlay::bind_node<FctNode>::ISOMORPHES_LIST;
    // Create Bindiff selectors/properties for the cfg level
    typename BinSlay::bind_node<BinSlay::BbNode>::SELECTORS cfg_selectors;
    typename BinSlay::bind_node<BinSlay::BbNode>::PROPERTIES cfg_properties;
    cfg_selectors.resize(3);
    cfg_properties.resize(2);
    cfg_selectors[BinSlay::idSelectors::NAME] = nullptr;
    cfg_selectors[BinSlay::idSelectors::CFG] = nullptr;
    cfg_properties[BinSlay::idProperties::UP] = new BinSlay::UpProperty<BinSlay::BbNode>;
    cfg_properties[BinSlay::idProperties::DOWN] = new BinSlay::DownProperty<BinSlay::BbNode>;
    cfg_selectors[BinSlay::idSelectors::CRC32] = new BinSlay::Crc32Selector<BinSlay::BbNode>;
    // Set the initial threshold
    double threshold = 70.0;

    while (threshold >= 5.0) {
      // Create tmp unmatched node lists
      auto *tmp_u1 = new typename BinSlay::bind_node<FctNode>::NODES_LIST;
      auto *tmp_u2 = new typename BinSlay::bind_node<FctNode>::NODES_LIST;

      // Record the updated cost we want to apply
      std::list<std::pair<
  	std::pair<unsigned int, unsigned int>,
  	unsigned int >
      > cost_to_update;

      unsigned int idx_left = 0;
      unsigned int idx_right = 0;

      for (auto it_iso = tmp_ep->begin(); it_iso != tmp_ep->end();/* no incrementation here */) {
	if (!(*it_iso)->getLeft()) {
	  tmp_u2->push_back((*it_iso)->getRight());
	  delete *it_iso;
	  it_iso = tmp_ep->erase(it_iso);
	  ++idx_right;
	  continue;
	}
	if (!(*it_iso)->getRight()) {
	  tmp_u1->push_back((*it_iso)->getLeft());
	  delete *it_iso;
	  it_iso = tmp_ep->erase(it_iso);
	  ++idx_left;
	  continue;
	}

	double similarity = this->run_bindiff_on_two_functions(
		(*it_iso)->getLeft()->getAddr(),
		(*it_iso)->getRight()->getAddr()
        );
	if (similarity <= threshold) {
	  // We also want make the deletion+insertion or insertion+deletion possible
	  // so we should decrease the cost to insert/delete while increasing the cost
	  // of substituting this two functions.
	  // Then we add node left and right to their new unmatched lists respectively
	  tmp_u1->push_back((*it_iso)->getLeft());
	  tmp_u2->push_back((*it_iso)->getRight());
	  unsigned int cost = 0;
	  if (similarity < 10.0)       	cost += 2000;
	  else if (similarity < 20.0)	cost += 1000;
	  else if (similarity < 30.0)	cost += 500;
	  else if (similarity < 40.0)	cost += 250;
	  else if (similarity < 50.0)	cost += 100;
	  else if (similarity < 60.0)	cost += 50;

	  cost_to_update.push_front({{idx_left, idx_right}, cost});

	  ++idx_left;
	  ++idx_right;
	  // if they are not enough similar, based on the treshold, we remove
	  // this couple from the edit path because these two functions are not
	  // isomorphic at the CFG level.
	  delete *it_iso;
	  it_iso = tmp_ep->erase(it_iso);
	} else {
	  // We add this couple to the final edit_path list
	  final_ep->push_front(*it_iso);
	  ++it_iso;
	}
      }
      // Decrease the threshold
      threshold -= 5.0;
      // Delete prev edit path
      if (threshold >= 5.0) {
	delete tmp_ep;
      }
      // Create a new GraphED object with tmp_u1 and tmp_u2 lists of nodes
      auto *tmp_ged = new BinSlay::GraphED<FctNode>(*this, *_graph_left, *_graph_right,
						    *tmp_u1, *tmp_u2);
      // Update costs
      for (auto it = cost_to_update.begin(); it != cost_to_update.end(); ++it) {
	tmp_ged->update_cost(it->second, it->first.first, it->first.second);
	//	tmp_ged->reduce_deletion_cost(it->second, it->first.first, it->first.second);
	//tmp_ged->reduce_insertion_cost(it->second, it->first.first, it->first.second);
      }
      // Re-compute GED with tmp_u1 and tmp_u2 and get the resulting edit path
      tmp_ged->compute();
      tmp_ep = tmp_ged->get_edit_path();
      // Clean
      delete tmp_ged;
      delete tmp_u1;
      delete tmp_u2;
    }
    // Delete selectors
    for (auto it = cfg_selectors.begin(); it != cfg_selectors.end(); ++it)
      delete *it;
    // Delete properties
    for (auto it = cfg_properties.begin(); it != cfg_properties.end(); ++it)
      delete *it;
    // All "match" which are in tmp_ep at this point must be added to the final ep list
    for (auto it_iso = tmp_ep->begin(); it_iso != tmp_ep->end(); ++it_iso) {
      final_ep->push_front(*it_iso);
    }
    delete tmp_ep;
    // Set the corrected edit path
    _ged->setEditPath(final_ep);

#ifdef DEBUG
    unsigned int final_ged = 0;
    for (auto it_iso = final_ep->begin(); it_iso != final_ep->end(); ++it_iso) {
      final_ged += (*it_iso)->getLevel();
    }
    std::cerr << std::dec << "Final ged is: " << final_ged << std::endl;
#endif // !DEBUG
  }
#ifdef MEASURE_PERFORMANCE
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
    std::cerr << "GED time: ";
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

// Param: BinDiff options, Ged options
bool
BinSlay::CG_Core::diff()
{
  unsigned int matched = 1;
  double treshold = 100;

  // Run BinDiff algorithm
  this->run_bindiff_algorithm(BinSlay::DiffingLevel::CALL_GRAPH);
  // Main loop
  while (treshold >= 50) {
    // Reset the number of matches
    matched = 0;
    // Re-Run BinDiff algorithm
    this->re_run_bindiff();
    // Run Ged algorithm
    this->compute_ged(BinSlay::gedProperties::WITH_VALIDATOR);
    
    auto *final_ep = this->get_edit_path();
    for (auto it_iso = final_ep->begin(); it_iso != final_ep->end(); ++it_iso) {
      // If it is a substitution edit operation, we want to appreciate the quality
      // of the match
      if ((*it_iso)->getLeft() && (*it_iso)->getRight()) {
  	double match_quality = this->run_bindiff_on_two_functions(
  		(*it_iso)->getLeft()->getAddr(),
  		(*it_iso)->getRight()->getAddr()
  	);
	if (match_quality >= treshold) {
  	  ++matched;
	  // Verbose for each couple:
	  // - Method of matching
	  // - which similarity if BinDiff similarity used
  	  this->add_iso_couple_to_mapping(
  		(*it_iso)->getLeft()->getAddr(),
  		(*it_iso)->getRight()->getAddr()
  	    );
  	}
      }
    }
    std::cerr << std::dec << "Number of matches: " << matched << std::endl;
    treshold -= 5.0;
  }
  return true;
}
