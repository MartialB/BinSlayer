#include "CG_Core.hh"
#include <stdlib.h>

BinSlay::CG_Core::CG_Core(BinSlay::ReverseAPI::IBinary &_bin_left,
		    BinSlay::ReverseAPI::IBinary &_bin_right)
  : BinSlay::ACore<BinSlay::FctNode>::ACore(_bin_left, _bin_right)
{
}

BinSlay::CG_Core::~CG_Core()
{
  delete _left;
  delete _right;
}

bool
BinSlay::CG_Core::load_graphs()
{
  if (!load_graph_left()) return false;
  if (!load_graph_right()) return false;
  return true;
}

bool
BinSlay::CG_Core::load_graph_left()
{
  if (!_graph_left) {
    _left = new (std::nothrow) BinSlay::CallGraph(&_bin_left);
    _graph_left = _left;
  }
  return _left ? true : false;
}

bool
BinSlay::CG_Core::load_graph_right()
{
  if (!_graph_right) {
    _right = new (std::nothrow) BinSlay::CallGraph(&_bin_right);
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
    _selectors[id_selec] = new (std::nothrow) BinSlay::SymNameSelector;
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

// TODO; a function to run bindiff at the cfg level
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
    cfg_properties[BinSlay::idProperties::UP] = new BinSlay::UpProperty<BinSlay::BbNode>;
    cfg_properties[BinSlay::idProperties::DOWN] = new BinSlay::DownProperty<BinSlay::BbNode>;
    //cfg_selectors[BinSlay::idSelectors::CFG] = new BinSlay::BindiffSelector<BinSlay::BbNode>;
    cfg_selectors[BinSlay::idSelectors::CRC32] = new BinSlay::Crc32Selector<BinSlay::BbNode>;

    // Set the initial threshold
    double threshold = 70.0;

    //  std::cout << ">>>>>>>>>>>>>>>>>>> Start the validator <<<<<<<<<<<<<<<" << std::endl;
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

	// Create the CFG objects
	BinSlay::CFG *cfg_left =
	  new BinSlay::CFG(&_bin_left, (*it_iso)->getLeft()->getAddr());
	BinSlay::CFG *cfg_right =
	  new BinSlay::CFG(&_bin_right, (*it_iso)->getRight()->getAddr());

	// Create the list of nodes for each graph
	auto *cfg_l_left = cfg_left->CreateListOfNodes();
	auto *cfg_l_right = cfg_right->CreateListOfNodes();

	// Create an instance of the Bindiff object
	auto *cfg_bindiff = new BinSlay::Bindiff<BinSlay::BbNode>(
			       	*cfg_l_left,
				*cfg_l_right,
				cfg_selectors,
				cfg_properties
			      );

	// Call the run method and retrieve the MAPPING
	auto *cfg_mapping = cfg_bindiff->run();

	// Get the number of isomorphims found
	double cfg_nb_isomorphims = 0;
	for (auto it_map_cfg = cfg_mapping->begin(); it_map_cfg != cfg_mapping->end();
	     ++it_map_cfg)
	  for (auto it_iso = (*it_map_cfg)->begin(); it_iso != (*it_map_cfg)->end(); ++it_iso)
	    ++cfg_nb_isomorphims;

	// Clean the cfg_mapping
	for (auto it_map_cfg = cfg_mapping->begin(); it_map_cfg != cfg_mapping->end();
	     ++it_map_cfg) {
	  for (auto it_iso = (*it_map_cfg)->begin(); it_iso != (*it_map_cfg)->end(); ++it_iso) {
	    delete *it_iso;
	  }
	  delete *it_map_cfg;
	}

	// Get the percentage of similarty between the two compared graphs
	unsigned int max = cfg_left->getnbNode() <= cfg_right->getnbNode() ?
	  cfg_right->getnbNode() : cfg_left->getnbNode();
	  
	//	  std::cout << "before deleting/inserting...." << std::endl;
	if (((double)cfg_nb_isomorphims / (double)max * 100.0) <= threshold) {


	  // We also want make the deletion+insertion or insertion+deletion possible
	  // so we should decrease the cost to insert/delete while increasing the cost
	  // of substituting this two functions

	  // Then we add node left and right to their new unmatched lists respectively
	  tmp_u1->push_back((*it_iso)->getLeft());
	  tmp_u2->push_back((*it_iso)->getRight());
	  unsigned int cost = 0; //(100 - ((double)cfg_nb_isomorphims / (double)max * 100.0)) / 10;
	  if (((double)cfg_nb_isomorphims / (double)max * 100.0) < 10.0)
	    cost += 2000;
	  else if (((double)cfg_nb_isomorphims / (double)max * 100.0) < 20.0)
	    cost += 1000;
	  else if (((double)cfg_nb_isomorphims / (double)max * 100.0) < 30.0)
	    cost += 500;
	  else if (((double)cfg_nb_isomorphims / (double)max * 100.0) < 40.0)
	    cost += 250;
	  else if (((double)cfg_nb_isomorphims / (double)max * 100.0) < 50.0)
	    cost += 100;
	  else if (((double)cfg_nb_isomorphims / (double)max * 100.0) < 60.0)
	    cost += 50;
	  // if (((double)cfg_nb_isomorphims / (double)max * 100.0) < 60.0)
	  // 	cost += 5;
	  cost_to_update.push_front(std::make_pair(std::make_pair(idx_left, idx_right), cost));

	  ++idx_left;
	  ++idx_right;
	  // if they are not enough similar, based on the treshold, we remove
	  // this couple from the edit path because these two functions are not
	  // isomorphic at the CFG level.
	  delete *it_iso;
	  it_iso = tmp_ep->erase(it_iso);
	} else {
	  // Then we add node left and right to their new unmatched lists respectively
	  // We add this couple to the final edit_path list
	  final_ep->push_front(*it_iso);
	  ++it_iso;
	}
	delete cfg_mapping;
	delete cfg_l_left;
	delete cfg_l_right;
	delete cfg_left;
	delete cfg_right;
	delete cfg_bindiff;
      }
      // Decrease the threshold
      threshold -= 5.0;
      // Delete prev edit path
      if (threshold >= 5.0) {
	delete tmp_ep;
      }

      // std::cout << std::dec << "unmatched left list size = " << tmp_u1->size() << std::endl;
      // std::cout << std::dec << "unmatched right list size = " << tmp_u2->size() << std::endl;

      // for (typename BinSlay::bind_node<BinSlay::FctNode>::NODES_LIST::const_iterator it =
      // 	     tmp_u1->begin(); it != tmp_u1->end(); ++it)
      // 	{
      // 	  std::cout << std::hex << (*it)->getAddr() << " - ";
      // 	}
      // std::cout << std::endl;

      // for (typename BinSlay::bind_node<BinSlay::FctNode>::NODES_LIST::const_iterator it =
      // 	     tmp_u2->begin(); it != tmp_u2->end(); ++it)
      // 	{
      // 	  std::cout << std::hex << (*it)->getAddr() << " - ";
      // 	}
      // std::cout << std::endl;

      // Create a new GraphED object with tmp_u1 and tmp_u2 lists of nodes
      BinSlay::GraphED<FctNode> *tmp_ged =
      	new BinSlay::GraphED<FctNode>(*_graph_left, *_graph_right, *tmp_u1, *tmp_u2);
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
    for (typename BinSlay::bind_node<BinSlay::BbNode>::SELECTORS::const_iterator it =
	   cfg_selectors.begin(); it != cfg_selectors.end(); ++it)
      delete *it;

    // Delete properties
    for (typename BinSlay::bind_node<BinSlay::BbNode>::PROPERTIES::const_iterator it =
	   cfg_properties.begin(); it != cfg_properties.end(); ++it)
      delete *it;

    for (typename BinSlay::bind_node<BinSlay::FctNode>::ISOMORPHES_LIST::iterator it_iso =
	   tmp_ep->begin(); it_iso != tmp_ep->end(); ++it_iso) {
      final_ep->push_front(*it_iso);
    }
    delete tmp_ep;

    _ged->setEditPath(final_ep);
    unsigned int final_ged = 0;
    for (typename BinSlay::bind_node<BinSlay::FctNode>::ISOMORPHES_LIST::iterator it_iso =
	   final_ep->begin(); it_iso != final_ep->end(); ++it_iso) {
      // if ((*it_iso)->getLeft())
      // 	std::cout << "0x" << std::hex << (*it_iso)->getLeft()->getAddr();
      // if ((*it_iso)->getRight())
      // 	std::cout << " & 0x" << std::hex << (*it_iso)->getRight()->getAddr();
      // std::cout << " & " << std::dec << (*it_iso)->getLevel() << "\\\\\\hline";
      // std::cout << std::endl;
      final_ged += (*it_iso)->getLevel();
      // std::cout << "(*it_iso)->getLeft(): " << (*it_iso)->getLeft()
      // 		<< " - (*it_iso)->getRight(): " << (*it_iso)->getRight() << std::endl;
    }
    std::cout << std::dec << "Final ged is: " << final_ged << std::endl;
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

void BinSlay::CG_Core::_run_bindiff_at_cfg_level()
{
  std::cout << "Level 2 !!!!" << std::endl;
  ////////////////////////////////////
  //    Control Flow graph level    //
  ////////////////////////////////////

  typename BinSlay::bind_node<BinSlay::BbNode>::SELECTORS cfg_selectors;
  typename BinSlay::bind_node<BinSlay::BbNode>::PROPERTIES cfg_properties;
  cfg_selectors.resize(3);
  cfg_properties.resize(2);

  cfg_selectors[BinSlay::idSelectors::NAME] = nullptr;
  cfg_properties[BinSlay::idProperties::UP] = new BinSlay::UpProperty<BinSlay::BbNode>;
  cfg_properties[BinSlay::idProperties::DOWN] = new BinSlay::DownProperty<BinSlay::BbNode>;
  //cfg_selectors[BinSlay::idSelectors::CFG] = new BinSlay::BindiffSelector<BinSlay::BbNode>;
  cfg_selectors[BinSlay::idSelectors::CFG] = nullptr;
  cfg_selectors[BinSlay::idSelectors::CRC32] = new BinSlay::Crc32Selector<BinSlay::BbNode>;

  for (auto it_map = _mapping->begin(); it_map != _mapping->end(); ++it_map) {
    for (auto it_iso = (*it_map)->begin(); it_iso != (*it_map)->end();/* no incrementation here */) {
      // Create the CFG objects
      BinSlay::CFG *cfg_left =
	new BinSlay::CFG(&_bin_left, (*it_iso)->getLeft()->getAddr());
      BinSlay::CFG *cfg_right =
	new BinSlay::CFG(&_bin_right, (*it_iso)->getRight()->getAddr());

      // Create the list of nodes for each graph
      auto *cfg_l_left = cfg_left->CreateListOfNodes();
      auto *cfg_l_right = cfg_right->CreateListOfNodes();

      // Create an instance of the Bindiff object
      BinSlay::Bindiff<BinSlay::BbNode> *cfg_bindiff =
	new BinSlay::Bindiff<BinSlay::BbNode>(*cfg_l_left, *cfg_l_right,
	       	       	  cfg_selectors, cfg_properties);

      // Call the run method and retrieve the MAPPING
      auto *cfg_mapping = cfg_bindiff->run();

      // Get the number of isomorphims found
      double cfg_nb_isomorphims = 0;
      for (typename BinSlay::bind_node<BinSlay::BbNode>::MAPPING::const_iterator it_map_cfg =
	     cfg_mapping->begin(); it_map_cfg != cfg_mapping->end(); ++it_map_cfg)
	for (typename BinSlay::bind_node<BinSlay::BbNode>::ISOMORPHES_LIST::const_iterator it_iso =
	       (*it_map_cfg)->begin(); it_iso != (*it_map_cfg)->end(); ++it_iso)
	  ++cfg_nb_isomorphims;

      // Clean the cfg_mapping
      for (auto it_map_cfg = cfg_mapping->begin(); it_map_cfg != cfg_mapping->end(); ++it_map_cfg) {
	for (auto it_iso = (*it_map_cfg)->begin(); it_iso != (*it_map_cfg)->end(); ++it_iso)
	  delete *it_iso;
	delete *it_map_cfg;
      }

      // Two cfg are identical if they have the same number of nodes and if
      // all their nodes are isomorphic.
      if (!(cfg_left->getnbNode() + cfg_right->getnbNode() ==
	    cfg_nb_isomorphims * 2)) {
	// if they are not, we remove this couple from the isoList because these
	// two functions are not isomorphic at the CFG level.
	delete *it_iso;
	it_iso = (*it_map)->erase(it_iso);
	// TODO: Need to remove them from the '_l_left' and '_l_right' lists too !
      } else {
	++it_iso;
      }
      delete cfg_mapping;
      delete cfg_l_left;
      delete cfg_l_right;
      delete cfg_left;
      delete cfg_right;
      delete cfg_bindiff;
    }
  }

  // Delete selectors
  for (auto it = cfg_selectors.begin(); it != cfg_selectors.end(); ++it)
    delete *it;
  
  // Delete properties
  for (auto it = cfg_properties.begin(); it != cfg_properties.end(); ++it)
    delete *it;
}
  
double
BinSlay::CG_Core::run_bindiff_at_cfg_level(
	unsigned long addr_left,
	unsigned long addr_right
)
{
  typename BinSlay::bind_node<BinSlay::BbNode>::SELECTORS cfg_selectors;
  typename BinSlay::bind_node<BinSlay::BbNode>::PROPERTIES cfg_properties;
  cfg_selectors.resize(3);
  cfg_properties.resize(2);

  cfg_selectors[BinSlay::idSelectors::NAME] = nullptr;
  cfg_properties[BinSlay::idProperties::UP] = new BinSlay::UpProperty<BinSlay::BbNode>;
  cfg_properties[BinSlay::idProperties::DOWN] = new BinSlay::DownProperty<BinSlay::BbNode>;
  cfg_selectors[BinSlay::idSelectors::CRC32] = new BinSlay::Crc32Selector<BinSlay::BbNode>;
  cfg_selectors[BinSlay::idSelectors::CFG] = nullptr;

  // Create the CFG objects
  BinSlay::CFG *cfg_left = new BinSlay::CFG(&_bin_left, addr_left);
  BinSlay::CFG *cfg_right = new BinSlay::CFG(&_bin_right, addr_right);

  // Create the list of nodes for each graph
  auto *cfg_l_left = cfg_left->CreateListOfNodes();
  auto *cfg_l_right = cfg_right->CreateListOfNodes();

  // Create an instance of the Bindiff object
  BinSlay::Bindiff<BinSlay::BbNode> *cfg_bindiff =
    new BinSlay::Bindiff<BinSlay::BbNode>(*cfg_l_left, *cfg_l_right,
		cfg_selectors, cfg_properties);

  // Call the run method and retrieve the MAPPING
  auto *cfg_mapping = cfg_bindiff->run();

  // Get the number of isomorphims found
  double cfg_nb_isomorphims = 0;
  for (typename BinSlay::bind_node<BinSlay::BbNode>::MAPPING::const_iterator it_map_cfg =
	 cfg_mapping->begin(); it_map_cfg != cfg_mapping->end(); ++it_map_cfg)
    for (typename BinSlay::bind_node<BinSlay::BbNode>::ISOMORPHES_LIST::const_iterator it_iso =
	   (*it_map_cfg)->begin(); it_iso != (*it_map_cfg)->end(); ++it_iso)
      ++cfg_nb_isomorphims;

  // Get the percentage of similarty between the two compared graphs
  unsigned int max = cfg_left->getnbNode() <= cfg_right->getnbNode() ?
    cfg_right->getnbNode() : cfg_left->getnbNode();
  //  std::cout << std::dec << "nb iso: " << cfg_nb_isomorphims << " - ";
  double res = (double)cfg_nb_isomorphims / (double)max * 100.0;
  //std::cout << res << "%" << std::endl;

  // Clean the cfg_mapping
  for (auto it_map_cfg = cfg_mapping->begin(); it_map_cfg != cfg_mapping->end(); ++it_map_cfg) {
    for (auto it_iso = (*it_map_cfg)->begin(); it_iso != (*it_map_cfg)->end(); ++it_iso)
      delete *it_iso;
    delete *it_map_cfg;
  }

  delete cfg_mapping;
  delete cfg_l_left;
  delete cfg_l_right;
  delete cfg_left;
  delete cfg_right;
  delete cfg_bindiff;

  // Delete selectors
  for (auto it = cfg_selectors.begin(); it != cfg_selectors.end(); ++it)
    delete *it;
  
  // Delete properties
  for (auto it = cfg_properties.begin(); it != cfg_properties.end(); ++it)
    delete *it;

  return res;
}
























// static inline int usage(void)
// {
//   std::cerr << "Usage: ./bindiff_on_steroids file1 file2" << std::endl;
//   return 1;
// }

// int main(int argc, char *argv[])
// {
//   if (argc < 3)
//     return usage();

//   // Retrieve the symbol 'get_instance_of_binary' from the 'BinaryHelper' dynamic library
//   DllManager loader;
//   BinSlay::ReverseAPI::IBinary *(*ptr_bin_ctor)(std::string const &) = NULL;

//   if (loader.OpenLibrary("BinaryHelper") == false)
//     std::cerr << "Fail to open dll 'BinaryHelper'." << std::endl;
//   ptr_bin_ctor = loader.GetSymboleFromLibrary<BinSlay::ReverseAPI::IBinary *(*)(std::string const &)>(
//       	  "BinaryHelper", "get_instance_of_binary");
//   if (!ptr_bin_ctor)
//     std::cerr << "Fail to load symbol 'get_instance_of_binary'." << std::endl;

//   // Get an instance a BinaryHelper to recover needed data from binaries to compare
//   BinSlay::ReverseAPI::IBinary *bin_left = (*ptr_bin_ctor)(argv[1]);
//   BinSlay::ReverseAPI::IBinary *bin_right = (*ptr_bin_ctor)(argv[2]);

//   //////////////////////////////////////////
//   ///////// Level 1: CallGraph level ///////
//   //////////////////////////////////////////

//   std::cout << std::endl << "### Comparison at the Call-Graph level" << std::endl << std::endl;

//   // Create call-graph from file graph's descriptor
//   BinSlay::CallGraph *graph01 = new BinSlay::CallGraph(bin_left);
//   BinSlay::CallGraph *graph02 = new BinSlay::CallGraph(bin_right);

//   // Create the selectors for the Call-Graph level
//   BinSlay::SELECTORS cg_selectors;
//   cg_selectors.push_front(new BinSlay::BindiffSelector);	// 3
//   cg_selectors.push_front(new BinSlay::Crc32Selector);		// 2
//   cg_selectors.push_front(new BinSlay::SymNameSelector);	// 1

//   // Create the .dot file for each graph
//   graph01->drawMe("graph01.dot");
//   graph02->drawMe("graph02.dot");

//   // Create the list of nodes for each graph
//   BinSlay::NODES_LIST *l_graph01 = graph01->CreateListOfNodes();
//   BinSlay::NODES_LIST *l_graph02 = graph02->CreateListOfNodes();

//   // Call the find isomorphism function and retrieve the MAPPING
//   BinSlay::MAPPING *mapping = BinSlay::findIsomorphism(cg_selectors, *l_graph01, *l_graph02);

//   double nb_isomorphims = 0;
//   // Display the results in the console
//   for (BinSlay::MAPPING::const_iterator it_map = mapping->begin(); it_map != mapping->end(); ++it_map)
//     {
//       for (BinSlay::ISOMORPHES_LIST::const_iterator it_iso = (*it_map)->begin();
//   	   it_iso != (*it_map)->end(); ++it_iso)
//   	{
//   	  // Display couples of isomorphism
//   	  std::cout << "[Level " << (*it_iso)->getLevel() << "] Node " << *(*it_iso)->getLeft()
//   		    << " ISO with Node " << *(*it_iso)->getRight() << " -> NAME: "
//   		    << (*it_iso)->getLeft()->getName() << std::endl;
//   	  ++nb_isomorphims;
//   	}
//     }

//   // Display the percentage of similarty between the two compared graphs
//   int max = graph01->getnbNode() < graph02->getnbNode() ? graph02->getnbNode() : graph01->getnbNode();
//   std::cout << "Percentage of similarity = '" << (nb_isomorphims / max * 100.0) << "%'" << std::endl;

//   // Draw the results with graphviz in .dot file for each graph
//   BinSlay::drawResults("res_graph01", mapping, *graph01);
//   BinSlay::drawResults("res_graph02", mapping, *graph02);

//   // Delete selectors for the Call-Graph level
//   for (BinSlay::SELECTORS::const_iterator it = cg_selectors.begin(); it != cg_selectors.end(); ++it)
//     delete *it;

//   // Compute an approximation (suboptimal) of the Graph Edit Distance (GED)
//   std::cout << std::endl << "Unmatching nodes in graph01: " << l_graph01->size() << std::endl;
//   // for (BinSlay::NODES_LIST::const_iterator it = l_graph01->begin();
//   //      it != l_graph01->end(); ++it)
//   //   {
//   //     std::cout << **it << std::endl;
//   //   }

//   std::cout << std::endl << "Unmatching nodes in graph02: " << l_graph02->size() << std::endl;
//   // for (BinSlay::NODES_LIST::const_iterator it = l_graph02->begin();
//   //      it != l_graph02->end(); ++it)
//   //   {
//   //     std::cout << **it << std::endl;
//   //   }

//   BinSlay::GraphED *ged = new BinSlay::GraphED(
// 		 *graph01, *graph01, *l_graph01, *l_graph02);
//   std::cout << "Graph Edit Distance is equal to '" << ged->compute() << "'." << std::endl;
//   //hungarian->display_upper_left_submatrix();
//   //std::cout << std::endl;
//   //hungarian->display_upper_right_submatrix();
//   //std::cout << std::endl;
//   //hungarian->display_bottom_left_submatrix();
//   //std::cout << std::endl;
//   //hungarian->display_bottom_right_submatrix();
//   //std::cout << std::endl;
//   //hungarian->display();
  
//   //  hungarian->solve();

//   // ////////////////////////////////////////
//   // ///////    Level 2: CFG level    ///////
//   // ////////////////////////////////////////
  
//   // std::cout << std::endl << "### Comparison at the Control Flow Graph level" << std::endl << std::endl;

//    // for (BinSlay::MAPPING::const_iterator it_map = mapping->begin(); it_map != mapping->end(); ++it_map)
//    //   {
//    //     for (BinSlay::ISOMORPHES_LIST::const_iterator it_iso = (*it_map)->begin();
//    // 	    it_iso != (*it_map)->end(); ++it_iso)
//    // 	{
//    // 	  // std::cout << "Function: " << (*it_iso)->getLeft()->getName() << std::endl;

//    // 	  BinSlay::CFG *cfg_left =
//    // 	    new BinSlay::CFG(bin_left, (*it_iso)->getLeft()->getAddr());
//    // 	  BinSlay::CFG *cfg_right =
//    // 	    new BinSlay::CFG(bin_right, (*it_iso)->getRight()->getAddr());

//    // 	  // Create the selectors for the CFG level
//    // 	  BinSlay::SELECTORS cfg_selectors;
//    // 	  cfg_selectors.push_front(new BinSlay::BindiffSelector);

//    // 	  // Create the list of nodes for each graph
//    // 	  BinSlay::NODES_LIST *cfg_l_left = cfg_left->CreateListOfNodes();
//    // 	  BinSlay::NODES_LIST *cfg_l_right = cfg_right->CreateListOfNodes();

//    // 	  // Call the find isomorphism function and retrieve the MAPPING
//    // 	  BinSlay::MAPPING *cfg_mapping =
//    // 	    BinSlay::findIsomorphism(cfg_selectors, *cfg_l_left, *cfg_l_right);
//    // 	  double cfg_nb_isomorphims = 0;

//    // 	  // Display the results in the console
//    // 	  for (BinSlay::MAPPING::const_iterator it_map = cfg_mapping->begin();
//    // 	       it_map != cfg_mapping->end(); ++it_map)
//    // 	    {
//    // 	      for (BinSlay::ISOMORPHES_LIST::const_iterator it_iso = (*it_map)->begin();
//    // 		   it_iso != (*it_map)->end(); ++it_iso)
//    // 		{
//    // 		  // // Display couples of isomorphism
//    // 		  // std::cout << "[Level " << (*it_iso)->getLevel() << "] Node "
//    // 		  // 	    << *(*it_iso)->getLeft()
//    // 		  // 	    << " ISO with Node " << *(*it_iso)->getRight() << std::endl;

//    // 		  // TODO: finish this !
//    // 		  // std::cout << "Disassembling basic block @VA 0x" << std::hex
//    // 		  // 	    <<(*it_iso)->getLeft()->getAddr() << std::dec << std::endl;
//    // 		  // bin_left->recover_basic_block_instr((*it_iso)->getLeft()->getAddr());
//    // 		  // std::cout << std::endl;
//    // 		  ++cfg_nb_isomorphims;
//    // 		  // TODO: finish this !
//    // 		}
//    // 	    }

//    // 	  // Display the percentage of similarty between the two compared graphs
//    // 	  int cfg_max = cfg_left->getnbNode() < cfg_right->getnbNode()
//    // 	    ? cfg_left->getnbNode() : cfg_right->getnbNode();
//    // 	  std::cout << "Percentage of similarity = '" << (cfg_nb_isomorphims / cfg_max * 100.0)
//    // 	  	    << "%'" << std::endl;

//    // 	  // Clean the cfg_mapping
//    // 	  for (BinSlay::MAPPING::const_iterator it_map = cfg_mapping->begin();
//    // 	       it_map != cfg_mapping->end(); ++it_map)
//    // 	    {
//    // 	      for (BinSlay::ISOMORPHES_LIST::const_iterator it_iso = (*it_map)->begin();
//    // 		   it_iso != (*it_map)->end(); ++it_iso)
//    // 		delete *it_iso;
//    // 	      delete *it_map;
//    // 	    }
//    // 	  delete cfg_mapping;

//    // 	  // Delete selectors for the Call-Graph level
//    // 	  for (BinSlay::SELECTORS::const_iterator it = cfg_selectors.begin();
//    // 	       it != cfg_selectors.end(); ++it)
//    // 	    delete *it;
//    // 	  delete cfg_left;
//    // 	  delete cfg_right;
//    // 	}
//    //   }

//   // //
//   // // TODO: Level 3 !
//   // //

//   // Clean the mapping
//   for (BinSlay::MAPPING::const_iterator it_map = mapping->begin(); it_map != mapping->end(); ++it_map)
//     {
//       for (BinSlay::ISOMORPHES_LIST::const_iterator it_iso = (*it_map)->begin();
//   	   it_iso != (*it_map)->end(); ++it_iso)
//   	delete *it_iso;
//       delete *it_map;
//     }
//   delete mapping;

//   // Free allocated memory
//   delete l_graph01;
//   delete l_graph02;
//   delete graph01;
//   delete graph02;
//   // delete bin_left;
//   // delete bin_right; // crash sous windows ???
//   loader.CloseLibrary("BinaryHelper");
//   return 0;
// }
