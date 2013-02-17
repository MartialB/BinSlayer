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
    virtual bool load_graphs() = 0;
    virtual double run_bindiff_on_two_nodes(
	BinSlay::ReverseAPI::Address,
	BinSlay::ReverseAPI::Address
    ) = 0;

  public:
    ACore(BinSlay::ReverseAPI::IBinary &_bin_left,
	  BinSlay::ReverseAPI::IBinary &_bin_right
    );
    ACore(ACore const &o);
    ACore& operator=(ACore const &o);

  public:
    virtual ~ACore();
    virtual bool add_Selector(BinSlay::idSelectors::idSelectors_e id_selec);
    virtual void reset_bindiff();
    virtual bool run_bindiff_algorithm();
    virtual bool compute_ged();

  public:
    void reset_mapping();
    void delete_selectors();
    void delete_properties();
    template<typename Stype>
    void delete_selectors(typename BinSlay::bind_node<Stype>::SELECTORS &);
    template<typename Ptype>
    void delete_properties(typename BinSlay::bind_node<Ptype>::PROPERTIES &);
    void remove_Selector(int id_selec);
    bool add_Property(BinSlay::idProperties::idProperties_e id_property);
    void remove_Property(BinSlay::idProperties::idProperties_e id_property);
    unsigned int re_run_bindiff();
    void delete_edit_path_list(typename BinSlay::bind_node<NodeType>::ISOMORPHES_LIST *ep) const;
    unsigned int get_ged();
    bool create_dot_file_left(std::string const &path);
    bool create_dot_file_right(std::string const &path);
    int get_nb_node_gleft() const;
    int get_nb_node_gright() const;
    BinSlay::AGraph<NodeType> const &get_graph_left() const;
    BinSlay::AGraph<NodeType> const &get_graph_right() const;
    typename BinSlay::bind_node<NodeType>::MAPPING const &get_mapping() const;
    typename BinSlay::bind_node<NodeType>::ISOMORPHES_LIST *get_edit_path() const;
    typename BinSlay::bind_node<NodeType>::NODES_LIST const &get_list_left() const;
    typename BinSlay::bind_node<NodeType>::NODES_LIST const &get_list_right() const;
    void add_iso_couple_to_mapping(
	BinSlay::ReverseAPI::Address addr_left,
	BinSlay::ReverseAPI::Address addr_right
    );
    void remove_iso_couple_from_mapping(
	BinSlay::ReverseAPI::Address,
	BinSlay::ReverseAPI::Address
    );
    unsigned int get_nb_isomorphism_found() const;

  protected:
    // Instrumentation of binaries
    BinSlay::ReverseAPI::IBinary	&_bin_left;
    BinSlay::ReverseAPI::IBinary	&_bin_right;
    BinSlay::AGraph<NodeType>		*_graph_left;
    BinSlay::AGraph<NodeType>		*_graph_right;
    BinSlay::Bindiff<NodeType>		*_bindiff;
    BinSlay::GraphED<NodeType>	       	*_ged;
    typename BinSlay::bind_node<NodeType>::MAPPING	_mapping;
    typename BinSlay::bind_node<NodeType>::NODES_LIST	_l_left;
    typename BinSlay::bind_node<NodeType>::NODES_LIST	_l_right;
    typename BinSlay::bind_node<NodeType>::SELECTORS	_selectors;
    typename BinSlay::bind_node<NodeType>::PROPERTIES	_properties;
  };

  template<typename NodeType>
  BinSlay::ACore<NodeType>::ACore(
	BinSlay::ReverseAPI::IBinary &_bin_left,
	BinSlay::ReverseAPI::IBinary &_bin_right
  )
    : _bin_left(_bin_left),
      _bin_right(_bin_right),
      _graph_left(nullptr),
      _graph_right(nullptr),
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
  
  template<typename NodeType>
  BinSlay::ACore<NodeType>::~ACore()
  {
    this->reset_mapping();
    this->delete_selectors();
    this->delete_properties();
    if (_bindiff) delete _bindiff;
    if (_ged) delete _ged;
  }

  template<typename NodeType>
  void BinSlay::ACore<NodeType>::reset_mapping()
  {
    for (auto it_map = this->_mapping.begin(); it_map != this->_mapping.end(); ++it_map) {
      for (auto it_iso = (*it_map)->begin(); it_iso != (*it_map)->end(); ++it_iso)
	delete *it_iso;
      delete *it_map;
    }
    this->_mapping.clear();
  }

  template<typename NodeType>
  void BinSlay::ACore<NodeType>::delete_selectors()
  {
    for (auto it_selec = this->_selectors.begin(); it_selec != this->_selectors.end(); ++it_selec)
      delete *it_selec;
  }

  template<typename NodeType>
  void BinSlay::ACore<NodeType>::delete_properties()
  {
    for (auto it_prop = this->_properties.begin(); it_prop != this->_properties.end(); ++it_prop)
      delete *it_prop;
  }

  template<typename NodeType>
  template<typename Stype>
  void BinSlay::ACore<NodeType>::delete_selectors(
  	typename BinSlay::bind_node<Stype>::SELECTORS &selectors
  )
  {
    for (auto it_selec = selectors.begin(); it_selec != selectors.end(); ++it_selec)
      delete *it_selec;
  }

  template<typename NodeType>
  template<typename Ptype>
  void BinSlay::ACore<NodeType>::delete_properties(
  	typename BinSlay::bind_node<Ptype>::PROPERTIES &properties
  )
  {
    for (auto it_prop = properties.begin(); it_prop != properties.end(); ++it_prop)
      delete *it_prop;
  }

  template<typename NodeType>
  bool BinSlay::ACore<NodeType>::add_Selector(BinSlay::idSelectors::idSelectors_e id_selec)
  {
    if (id_selec == BinSlay::idSelectors::CFG) {
      _selectors[id_selec] = new BinSlay::BindiffSelector<NodeType>;
    } else if (id_selec == BinSlay::idSelectors::CRC32) {
      _selectors[id_selec] = new BinSlay::Crc32Selector<NodeType>;
    }
    return true;
  }

  template<typename NodeType>
  void BinSlay::ACore<NodeType>::remove_Selector(int id_selec)
  {
    delete _selectors[id_selec];
    _selectors[id_selec] = nullptr;
  }

  template<typename NodeType>
  bool BinSlay::ACore<NodeType>::add_Property(BinSlay::idProperties::idProperties_e id_property)
  {
    if (id_property == BinSlay::idProperties::UP) {
      _properties[id_property] = new BinSlay::UpProperty<NodeType>;
    } else if (id_property == BinSlay::idProperties::DOWN) {
      _properties[id_property] = new BinSlay::DownProperty<NodeType>;
    }
    return true;
  }

  template<typename NodeType>
  void BinSlay::ACore<NodeType>::remove_Property(BinSlay::idProperties::idProperties_e id_property)
  {
    delete _properties[id_property];
    _properties[id_property] = nullptr;
  }

  // Run bindiff algorithm at the current level
  template<typename NodeType>
  void BinSlay::ACore<NodeType>::reset_bindiff()
  {
    // Delete the previous '_bindiff' object if it already exists
    if (_bindiff) { delete _bindiff; _bindiff = nullptr; }
    // Clean previous used lists of nodes if already exist
    this->_l_left.clear();
    this->_l_right.clear();
    // Create the list of nodes for each graph
    _graph_left->CreateListOfNodes(_l_left);
    _graph_right->CreateListOfNodes(_l_right);
    // Clean the previous '_mapping' if exists
    this->reset_mapping();
    _bindiff = new BinSlay::Bindiff<NodeType>(_l_left, _l_right, _selectors, _properties);
  }

  // Run bindiff algorithm at the current level
  template<typename NodeType>
  bool BinSlay::ACore<NodeType>::run_bindiff_algorithm()
  {
    // Create an instance of the Bindiff object
    if (!_bindiff) this->reset_bindiff();
    // Call the run method and retrieve the MAPPING
    this->_bindiff->run(this->_mapping);
    return true;
  }

  template<typename NodeType>
  unsigned int BinSlay::ACore<NodeType>::re_run_bindiff()
  {
    unsigned int ret = 0;

    if (!_bindiff || !_mapping.size())
      return 0;
    ret = this->_bindiff->re_run(this->_mapping);
    return ret;
  }

  // Compute the GED and display results
  template<typename NodeType>
  bool BinSlay::ACore<NodeType>::compute_ged()
  {
#ifdef MEASURE_PERFORMANCE
    timespec start, end;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
#endif // !MEASURE_PERFORMANCE

    // Reset '_ged'
    if (_ged) { delete _ged; _ged = nullptr; }

    // Create the list of nodes for each graph if they not exist yet
    if (!this->_l_left.size())
      this->_graph_left->CreateListOfNodes(this->_l_left);
    if (!this->_l_right.size())
      this->_graph_right->CreateListOfNodes(this->_l_right);

    // // Swap graphs in order that g1 <= g2 (performance optimization)
    // if (_l_left->size() <= _l_right->size()) {
    _ged = new BinSlay::GraphED<NodeType>(*this, *_graph_left, *_graph_right, _l_left, _l_right);
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
    if (_l_left.size()) {
      int left_size = _l_left.size() - 1;
      for (int i = left_size; i >= 0; --i) {
	if (!_l_left[i])
	  _l_left.pop_back();
	else
	  break;
      }
    }
    if (_l_right.size()) {
      int right_size = _l_right.size() - 1;
      for (int i = right_size; i >= 0; --i) {
	if (!_l_right[i])
	  _l_right.pop_back();
	else
	  break;
      }
    }
    // We do not delete the lists of nodes now because we may still use them
    // again
    return true;
  }

  template<typename NodeType> typename BinSlay::bind_node<NodeType>::ISOMORPHES_LIST *
  BinSlay::ACore<NodeType>::get_edit_path() const
  {
    return _ged->get_edit_path();
  }

  template<typename NodeType> void
  BinSlay::ACore<NodeType>::delete_edit_path_list(
	typename BinSlay::bind_node<NodeType>::ISOMORPHES_LIST *l
  ) const
  {
    for (auto it_iso = l->begin(); it_iso != l->end(); ++it_iso)
      delete *it_iso;
    delete l;
  }

  template<typename NodeType>
  unsigned int BinSlay::ACore<NodeType>::get_ged()
  {
    if (!_ged)
      this->compute_ged();
    return this->_ged->getGED();
  }

  template<typename NodeType>
  bool BinSlay::ACore<NodeType>::create_dot_file_left(std::string const &path)
  {
    if (path != "" && !_graph_left)
      return false;
    _graph_left->drawMe(path);
    return true;
  }

  template<typename NodeType>
  bool BinSlay::ACore<NodeType>::create_dot_file_right(std::string const &path)
  {
    if (path != "" && !_graph_right)
      return false;
    _graph_right->drawMe(path);
    return true;
  }

  // TODO!!!
  // // Returns 'true' if the two CFGs are ismorphic (100% match, 0 ged cost)
  // bool are_graphs_isomorphic() const;
  
  template<typename NodeType>
  int BinSlay::ACore<NodeType>::get_nb_node_gleft() const
  {
    return this->_graph_left->getnbNode();
  }

  template<typename NodeType>
  int BinSlay::ACore<NodeType>::get_nb_node_gright() const
  {
    return this->_graph_right->getnbNode();
  }

  template<typename NodeType> typename BinSlay::bind_node<NodeType>::MAPPING const &
  BinSlay::ACore<NodeType>::get_mapping() const
  {
    return this->_mapping;
  }

  template<typename NodeType> BinSlay::AGraph<NodeType> const &
  BinSlay::ACore<NodeType>::get_graph_left() const
  {
    return *this->_graph_left;
  }

  template<typename NodeType> BinSlay::AGraph<NodeType> const &
  BinSlay::ACore<NodeType>::get_graph_right() const
  {
    return *this->_graph_right;
  }

  template<typename NodeType> typename BinSlay::bind_node<NodeType>::NODES_LIST const &
  BinSlay::ACore<NodeType>::get_list_left() const
  {
    return this->_l_left;
  }

  template<typename NodeType> typename BinSlay::bind_node<NodeType>::NODES_LIST const &
  BinSlay::ACore<NodeType>::get_list_right() const
  {
    return this->_l_right;
  }

  template<typename NodeType> void
  BinSlay::ACore<NodeType>::add_iso_couple_to_mapping(
	BinSlay::ReverseAPI::Address addr_left,
	BinSlay::ReverseAPI::Address addr_right
  )
  {
    NodeType *node_left = nullptr;
    NodeType *node_right = nullptr;

    // Get left node
    for (auto it = this->_l_left.begin(); it != this->_l_left.end(); ++it)
      if ((*it)->getAddr() == addr_left) {
	node_left = *it;
	// remove the node from the left list
	if (_bindiff)
	  _bindiff->remove_node_from_left_list(node_left->getIdx());
	break;
      }
    // Get right node
    for (auto it = this->_l_right.begin(); it != this->_l_right.end(); ++it)
      if ((*it)->getAddr() == addr_right) {
	node_right = *it;
	// remove the node from the right list
	if (_bindiff)
	  _bindiff->remove_node_from_right_list(node_right->getIdx());
	break;
      }
    // If the mapping is empty, add a new iso_list in it
    if (!this->_mapping.size()) {
      _mapping.push_front(new typename BinSlay::bind_node<NodeType>::ISOMORPHES_LIST);
    }
    // Add the new couple of isomorphisms to the mapping
    // level: -1 -> means that it has been matched manually
    (*_mapping.begin())->push_front(new BinSlay::Isomorphes<NodeType>(node_left, node_right, -1));
  }

  template<typename NodeType> void
  BinSlay::ACore<NodeType>::remove_iso_couple_from_mapping(
	BinSlay::ReverseAPI::Address,
	BinSlay::ReverseAPI::Address
  )
  {
  }

  template<typename NodeType> unsigned int
  BinSlay::ACore<NodeType>::get_nb_isomorphism_found() const
  {
    int nb_isomorphism = 0;

    for (auto it_map = this->_mapping.begin(); it_map != this->_mapping.end(); ++it_map)
      for (auto it_iso = (*it_map)->begin(); it_iso != (*it_map)->end(); ++it_iso)
	++nb_isomorphism;
    return nb_isomorphism;
  }
}

#endif // !__ACORE_HHP__
