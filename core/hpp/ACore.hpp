/**
 * \file ACore.hh
 * \author Martial Bourquin
 * \version 1.0
 * \date 30/06/2012
 */

#ifndef __ACORE_HHP__
# define __ACORE_HHP__

# include <time.h>
# include <unistd.h>

# include "Core_ErrorHandler.hh"
# include "BinDiff.hh"
# include "BinSlay.hpp"
# include "AGraph.hpp"
# include "GraphED.hpp"
# include "BindiffSelector.hpp"
# include "Crc32Selector.hpp"
# include "SymNameSelector.hpp"
# include "UpProperty.hpp"
# include "DownProperty.hpp"
# include "CFG.hh"

namespace BinSlay
{
  template<typename NodeType>
  class ACore : public Core_ErrorHandler
  {
  public:
    ACore(BinSlay::ReverseAPI::IBinary &_bin_left,
	  BinSlay::ReverseAPI::IBinary &_bin_right)
      : _bin_left(_bin_left),
	_bin_right(_bin_right),
	_graph_left(nullptr),
	_graph_right(nullptr),
	_l_left(nullptr),
	_l_right(nullptr),
	_mapping(nullptr),
	_bindiff(nullptr),
	_ged(nullptr)
    {
      // Init '_selectors'
      _selectors.resize(3);
      _selectors[0] = nullptr;
      _selectors[1] = nullptr;
      _selectors[2] = nullptr;
      
      // Init '_properties'
      _properties.resize(2);
      _properties[0] = nullptr;
      _properties[1] = nullptr;
    }

    // TODO: copy, '='
    //    ACore(ACore const &o);
    //ACore& operator=(ACore const &o);

  public:
    virtual ~ACore()
    {
      // Clean the '_cg_mapping'
      if (_mapping) {
	for (auto it_map = _mapping->begin(); it_map != _mapping->end(); ++it_map) {
	  for (auto it_iso = (*it_map)->begin(); it_iso != (*it_map)->end(); ++it_iso)
	    delete *it_iso;
	  delete *it_map;
	}
	delete _mapping;
      }
      // Delete selectors
      for (auto it_selec = _selectors.begin(); it_selec != _selectors.end(); ++it_selec)
	delete *it_selec;
      // Delete properties
      for (auto it_prop = _properties.begin(); it_prop != _properties.end(); ++it_prop)
	delete *it_prop;      
      // Free allocated memory
      if (_l_left) delete _l_left;
      if (_l_right) delete _l_right;
      if (_ged) delete _ged;
    }
    
    virtual bool load_graphs() = 0;
    virtual bool load_graph_left() = 0;
    virtual bool load_graph_right() = 0;

  public:
    // To retrieve the string associated with the last error
    std::stringstream const &getErrorBuffer() const
    {
      return this->_buf_error;
    }

    // Function to add/remove a selector
    virtual bool add_Selector(BinSlay::idSelectors::idSelectors_e id_selec)
    {
      if (id_selec == BinSlay::idSelectors::CFG) {
	_selectors[id_selec] = new BinSlay::BindiffSelector<NodeType>;
      } else if (id_selec == BinSlay::idSelectors::CRC32) {
	_selectors[id_selec] = new BinSlay::Crc32Selector<NodeType>;
      }
      return true;
    }

    void remove_Selector(int id_selec)
    {
      delete _selectors[id_selec];
      _selectors[id_selec] = nullptr;
    }

    // Function to add/remove a property
    bool add_Property(BinSlay::idProperties::idProperties_e id_property)
    {
      if (id_property == BinSlay::idProperties::UP) {
	_properties[id_property] = new BinSlay::UpProperty<NodeType>;
      } else if (id_property == BinSlay::idProperties::DOWN) {
	_properties[id_property] = new BinSlay::DownProperty<NodeType>;
      }
      return true;
    }

    void remove_Property(BinSlay::idProperties::idProperties_e id_property)
    {
      delete _properties[id_property];
      _properties[id_property] = nullptr;
    }

    // Run bindiff algorithm at the current level
    virtual bool run_bindiff_algorithm()
    {
      // Delete the previous '_bindiff' object if it already exists
      if (_bindiff) { delete _bindiff; _bindiff = nullptr; }

      // Clean previous used lists of nodes if already exist
      if (_l_left) { delete _l_left; _l_left = nullptr; }
      if (_l_right) { delete _l_right; _l_right = nullptr; }

      // Create the list of nodes for each graph
      _l_left = _graph_left->CreateListOfNodes();
      _l_right = _graph_right->CreateListOfNodes();

      // Clean the previous '_mapping' if exists
      if (_mapping) {
	for (auto it_map = _mapping->begin(); it_map != _mapping->end(); ++it_map) {
	  for (auto it_iso = (*it_map)->begin(); it_iso != (*it_map)->end(); ++it_iso)
	    delete *it_iso;
	  delete *it_map;
	}
	delete _mapping;
	_mapping = nullptr;
      }

      // Create an instance of the Bindiff object
      _bindiff = new BinSlay::Bindiff<NodeType>(*_l_left, *_l_right, _selectors, _properties);

      // Call the run method and retrieve the MAPPING
      _mapping = _bindiff->run();

      std::cout << "Unmatched nodes in left: " << std::dec << _l_left->size() << std::endl;
      std::cout << "Unmatched nodes in right: " << std::dec << _l_right->size() << std::endl;
      _bindiff->drawResults("res_left", _mapping, *_graph_left);
      _bindiff->drawResults("res_right", _mapping, *_graph_right);

      return true;
    }

    unsigned int re_run_bindiff()
    {
      unsigned int ret = 0;

      if (!_bindiff || !_mapping)
	return 0;
      ret = _bindiff->re_run(*_mapping);
      _bindiff->drawResults("res_left", _mapping, *_graph_left);
      _bindiff->drawResults("res_right", _mapping, *_graph_right);
      return ret;
    }

    // Compute the GED and display results
    virtual bool compute_ged()
    {
#ifdef MEASURE_PERFORMANCE
      timespec start, end;
      clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
#endif // !MEASURE_PERFORMANCE

      // Reset '_ged'
      if (_ged) { delete _ged; _ged = nullptr; }

      // Create the list of nodes for each graph if they not exist yet
      if (!_l_left) _l_left = _graph_left->CreateListOfNodes();
      if (!_l_right) _l_right = _graph_right->CreateListOfNodes();

      // // Swap graphs in order that g1 <= g2 (performance optimization)
      // if (_l_left->size() <= _l_right->size()) {
      _ged = new BinSlay::GraphED<NodeType>(*_graph_left, *_graph_right, *_l_left, *_l_right);
      // } else {
      // 	_ged = new BinSlay::GraphED<NodeType>(*_graph_right, *_graph_left, *_l_right, *_l_left);
      // }
      _ged->compute();

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
     
      // TODO: do this in a better way, unsafe code
      // Remove the NULL item from the lists
      if (_l_left->size()) {
	int left_size = _l_left->size() - 1;
	for (int i = left_size; i >= 0; --i) {
	  if (!(*_l_left)[i])
	    _l_left->pop_back();
	  else
	    break;
	}
      }
      if (_l_right->size()) {
	int right_size = _l_right->size() - 1;
	for (int i = right_size; i >= 0; --i) {
	  if (!(*_l_right)[i])
	    _l_right->pop_back();
	  else
	    break;
	}
      }
      // We do not delete the lists of nodes now because we may still use them
      // again
      return true;
    }

    typename BinSlay::bind_node<NodeType>::ISOMORPHES_LIST *
    get_edit_path() const
    {
      return _ged->get_edit_path();
    }

    void
    delete_edit_path_list(
	typename BinSlay::bind_node<NodeType>::ISOMORPHES_LIST *l
    ) const
    {
      for (auto it_iso = l->begin(); it_iso != l->end(); ++it_iso)
	delete *it_iso;
      delete l;
    }

    unsigned int get_ged()
    {
      if (!_ged)
	this->compute_ged();
      return this->_ged->getGED();
    }

    // To create associated .dot file
    bool create_dot_file_left(std::string const &path)
    {
      if (path != "" && !_graph_left)
	return false;
      _graph_left->drawMe(path);
      return true;
    }

    bool create_dot_file_right(std::string const &path)
    {
      if (path != "" && !_graph_right)
	return false;
      _graph_right->drawMe(path);
      return true;
    }

    // // Returns 'true' if the two CFGs are ismorphic (100% match, 0 ged cost)
    // bool are_graphs_isomorphic() const;

    // const getters
    int getNbNodeInGraphLeft() const
    {
      return this->_graph_left->getnbNode();
    }

    int getNbNodeInGraphRight() const
    {
      return this->_graph_right->getnbNode();
    }

    typename BinSlay::bind_node<NodeType>::MAPPING const &
    get_mapping() const
    {
      return *this->_mapping;
    }

    BinSlay::AGraph<NodeType> const &
    get_graph_left() const
    {
      return *this->_graph_left;
    }

    BinSlay::AGraph<NodeType> const &
    get_graph_right() const
    {
      return *this->_graph_right;
    }

    typename BinSlay::bind_node<NodeType>::NODES_LIST const &
    get_list_left() const
    {
      return *this->_l_left;
    }

    typename BinSlay::bind_node<NodeType>::NODES_LIST const &
    get_list_right() const
    {
      return *this->_l_right;
    }

    // Add/Remove items from the matching
    void
    add_iso_couple_to_mapping(
	BinSlay::ReverseAPI::Address addr_left,
	BinSlay::ReverseAPI::Address addr_right
    )
    {
      NodeType *node_left = nullptr;
      NodeType *node_right = nullptr;

      // Get left node
      for (auto it = _l_left->begin(); it != _l_left->end(); ++it)
	if ((*it)->getAddr() == addr_left) {
	  node_left = *it;
	  // remove the node from the left list
	  if (_bindiff)
	    _bindiff->remove_node_from_left_list(node_left->getIdx());
	  break;
	}
      // Get right node
      for (auto it = _l_right->begin(); it != _l_right->end(); ++it)
	if ((*it)->getAddr() == addr_right) {
	  node_right = *it;
	  // remove the node from the right list
	  if (_bindiff)
	    _bindiff->remove_node_from_right_list(node_right->getIdx());
	  break;
	}

      // If the mapping is empty, add a new iso_list in it
      if (!_mapping) {
	_mapping = new typename BinSlay::bind_node<NodeType>::MAPPING;
	_mapping->push_front(new typename BinSlay::bind_node<NodeType>::ISOMORPHES_LIST);
      }

      // Add the new couple of isomorphisms to the mapping
      // level: -1 -> means that it has been matched manually
      (*_mapping->begin())->push_front(new BinSlay::Isomorphes<NodeType>(node_left, node_right, -1));
    }

    void
    remove_iso_couple_from_mapping(
	BinSlay::ReverseAPI::Address,
	BinSlay::ReverseAPI::Address
    )
    {
    }

    unsigned int
    get_nb_isomorphism_found() const
    {
      int nb_isomorphism = 0;

      if (_mapping) {
	for (auto it_map = _mapping->begin(); it_map != _mapping->end(); ++it_map) {
	  for (auto it_iso = (*it_map)->begin(); it_iso != (*it_map)->end(); ++it_iso)
	    ++nb_isomorphism;
	}
      }
      return nb_isomorphism;
    }

  protected:
    // Instrumentation of binaries
    BinSlay::ReverseAPI::IBinary &_bin_left;
    BinSlay::ReverseAPI::IBinary &_bin_right;
    // Graph data
    BinSlay::AGraph<NodeType> *_graph_left;
    BinSlay::AGraph<NodeType> *_graph_right;
    // Lists of nodes
    typename BinSlay::bind_node<NodeType>::NODES_LIST *_l_left;
    typename BinSlay::bind_node<NodeType>::NODES_LIST *_l_right;
    // Couples of isomorphisms found
    typename BinSlay::bind_node<NodeType>::MAPPING *_mapping;
    // Bindiff
    BinSlay::Bindiff<NodeType> *_bindiff;
    // Ged
    BinSlay::GraphED<NodeType> *_ged;
    // Error buffer
    std::stringstream _buf_error;
    // Selectors
    typename BinSlay::bind_node<NodeType>::SELECTORS _selectors;
    // Properties
    typename BinSlay::bind_node<NodeType>::PROPERTIES _properties;
  };
}

#endif // !__ACORE_HHP__
