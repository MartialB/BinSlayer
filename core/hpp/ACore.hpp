/**
 * \file ACore.hh
 * \author Martial Bourquin
 * \version 1.0
 * \date 30/06/2012
 */

#ifndef ACORE_HH_
# define ACORE_HH_

# include <time.h>
# include <unistd.h>
# include <sstream>

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
  class ACore
  {
  public:
    enum idSelectors { name = 0, crc32, cfg };
    enum idProperties { up = 0, down };

  public:
    ACore(BinSlay::ReverseAPI::IBinary &_bin_left,
	  BinSlay::ReverseAPI::IBinary &_bin_right)
      : _bin_left(_bin_left),
	_bin_right(_bin_right),
	_graph_left(NULL),
	_graph_right(NULL),
	_l_left(NULL),
	_l_right(NULL),
	_mapping(NULL),
	_bindiff(NULL),
	_ged(NULL)
    {
      // Init '_selectors'
      _selectors.resize(3);
      _selectors[0] = NULL;
      _selectors[1] = NULL;
      _selectors[2] = NULL;
      
      // Init '_properties'
      _properties.resize(2);
      _properties[0] = NULL;
      _properties[1] = NULL;
    }

    //    ACore(ACore const &o);
    //ACore& operator=(ACore const &o);

  public:
    virtual ~ACore()
    {
      // Clean the '_cg_mapping'
      if (_mapping)
	{
	  for (typename BinSlay::bind_node<NodeType>::MAPPING::const_iterator it_map =
		 _mapping->begin(); it_map != _mapping->end(); ++it_map)
	    {
	      for (typename BinSlay::bind_node<NodeType>::ISOMORPHES_LIST::const_iterator it_iso =
		     (*it_map)->begin(); it_iso != (*it_map)->end(); ++it_iso)
		delete *it_iso;
	      delete *it_map;
	    }
	  delete _mapping;
	}
      
      // Delete selectors
      for (typename BinSlay::bind_node<NodeType>::SELECTORS::const_iterator it = _selectors.begin();
	   it != _selectors.end(); ++it)
	delete *it;
      
      // Delete properties
      for (typename BinSlay::bind_node<NodeType>::PROPERTIES::const_iterator it = _properties.begin();
	   it != _properties.end(); ++it)
	delete *it;
      
      // Free allocated memory
      if (_l_left) delete _l_left;
      if (_l_right) delete _l_right;
      if (_graph_left) delete _graph_left;
      if (_graph_right) delete _graph_right;
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
    virtual void add_Selector(int idSelector)
    {
      if (idSelector == cfg)
	{
	  _selectors[idSelector] = new BinSlay::BindiffSelector<NodeType>;
	}
      else if (idSelector == crc32)
	_selectors[idSelector] = new BinSlay::Crc32Selector<NodeType>;
    }

    void remove_Selector(int idSelector)
    {
      delete _selectors[idSelector];
      _selectors[idSelector] = NULL;
    }

    // Function to add/remove a property
    void add_Property(int idProperty)
    {
      if (idProperty == up)
	{
	  _properties[idProperty] = new BinSlay::UpProperty<NodeType>;
	}
      else if (idProperty == down)
	{
	  _properties[idProperty] = new BinSlay::DownProperty<NodeType>;
	}
    }

    void remove_Property(int idProperty)
    {
      delete _properties[idProperty];
      _properties[idProperty] = NULL;  
    }

    // Run bindiff algorithm at the current level
    virtual bool run_bindiff_algorithm(int)
    {
      ///////////////////////////////////
      timespec start, end;
      clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
      ///////////////////////////////////

      // Delete the previous '_bindiff' object
      if (_bindiff) { delete _bindiff; _bindiff = NULL; }

      // Clean previous used lists of nodes
      if (_l_left) { delete _l_left; _l_left = NULL; }
      if (_l_right) { delete _l_right; _l_right = NULL; }

      // Create the list of nodes for each graph
      _l_left = _graph_left->CreateListOfNodes();
      _l_right = _graph_right->CreateListOfNodes();

      // Clean the previous '_mapping'
      if (_mapping)
	{
	  for (typename BinSlay::bind_node<NodeType>::MAPPING::const_iterator it_map =
		 _mapping->begin(); it_map != _mapping->end(); ++it_map)
	    {
	      for (typename BinSlay::bind_node<NodeType>::ISOMORPHES_LIST::const_iterator it_iso =
		     (*it_map)->begin(); it_iso != (*it_map)->end(); ++it_iso)
		delete *it_iso;
	      delete *it_map;
	    }
	  delete _mapping;
	  _mapping = NULL;
	}

      // Create an instance of the Bindiff object
      _bindiff = new BinSlay::Bindiff<NodeType>(*_l_left, *_l_right, _selectors, _properties);

      // Call the run method and retrieve the MAPPING
      _mapping = _bindiff->run();

      //////////////////////////////////
      clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
      std::cout << "Bindiff time: ";
      if ((end.tv_nsec-start.tv_nsec) < 0)
      	{
      	  std::cout << end.tv_sec - start.tv_sec - 1 << ":";
      	  std::cout << 1000000000 + end.tv_nsec - start.tv_nsec << std::endl;;
      	}
      else
      	{
      	  std::cout << end.tv_sec - start.tv_sec << ":";
      	  std::cout << end.tv_nsec - start.tv_nsec << std::endl;
      	}
      //////////////////////////////////

      // std::cout << "Unmatched nodes in left: " << _l_left->size() << std::endl;
      // std::cout << "Unmatched nodes in right: " << _l_right->size() << std::endl;
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

      // std::cout << "Unmatched nodes in left: " << _l_left->size() << std::endl;
      // std::cout << "Unmatched nodes in right: " << _l_right->size() << std::endl;
      _bindiff->drawResults("res_left", _mapping, *_graph_left);
      _bindiff->drawResults("res_right", _mapping, *_graph_right);

      //            std::cout << std::dec << "ret: " << ret << std::endl;
      return ret;
    }

    // Compute the GED and display results
    virtual bool compute_ged()
    {
      ///////////////////////////////////
      timespec start, end;
      clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
      ///////////////////////////////////

      // Reset '_ged'
      if (_ged) { delete _ged; _ged = NULL; }

      // Create the list of nodes for each graph if they not exist yet
      if (!_l_left) _l_left = _graph_left->CreateListOfNodes();
      if (!_l_right) _l_right = _graph_right->CreateListOfNodes();

      _ged = new BinSlay::GraphED<NodeType>(*_graph_left, *_graph_right, *_l_left, *_l_right);
      _ged->compute();

      //////////////////////////////////
      clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
      std::cout << "GED time: ";
      if ((end.tv_nsec-start.tv_nsec) < 0)
      	{
      	  std::cout << end.tv_sec - start.tv_sec - 1 << ":";
      	  std::cout << 1000000000 + end.tv_nsec - start.tv_nsec << std::endl;;
      	}
      else
      	{
      	  std::cout << end.tv_sec - start.tv_sec << ":";
      	  std::cout << end.tv_nsec - start.tv_nsec << std::endl;
      	}
      //////////////////////////////////
     

      // Remove the NULL item from the lists
      if (_l_left->size())
	{
	  int left_size = _l_left->size() - 1;
	  for (int i = left_size; i >= 0; --i)
	    {
	      if (!(*_l_left)[i])
		_l_left->pop_back();
	      else
		break;
	    }
	}
      if (_l_right->size())
	{
	  int right_size = _l_right->size() - 1;
	  for (int i = right_size; i >= 0; --i)
	    {
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

    typename BinSlay::bind_node<NodeType>::ISOMORPHES_LIST *get_edit_path() const
    {
      return _ged->get_edit_path();
    }

    void delete_edit_path_list(typename BinSlay::bind_node<NodeType>::ISOMORPHES_LIST *l) const
    {
      for (typename BinSlay::bind_node<NodeType>::ISOMORPHES_LIST::const_iterator it_iso = l->begin();
	   it_iso != l->end(); ++it_iso)
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

    // Returns 'true' if the two CFGs are ismorphic (100% match, 0 ged cost)
    bool are_graphs_isomorphic() const;

    // const getters
    int getNbNodeInGraphLeft() const
    {
      return this->_graph_left->getnbNode();
    }

    int getNbNodeInGraphRight() const
    {
      return this->_graph_right->getnbNode();
    }

    typename BinSlay::bind_node<NodeType>::MAPPING const &get_mapping() const
    {
      return *this->_mapping;
    }

    BinSlay::AGraph<NodeType> const &get_graph_left() const
    {
      return *this->_graph_left;
    }

    BinSlay::AGraph<NodeType> const &get_graph_right() const
    {
      return *this->_graph_right;
    }

    typename BinSlay::bind_node<NodeType>::NODES_LIST const &get_list_left() const
    {
      return *this->_l_left;
    }

    typename BinSlay::bind_node<NodeType>::NODES_LIST const &get_list_right() const
    {
      return *this->_l_right;
    }

    // Add/Remove items from the matching
    void add_iso_couple_to_mapping(BinSlay::ReverseAPI::Address addr_left,
				   BinSlay::ReverseAPI::Address addr_right)
    {
      NodeType *node_left = NULL;
      NodeType *node_right = NULL;

      // Get left node
      for (typename BinSlay::bind_node<NodeType>::NODES_LIST::const_iterator it =
	     _l_left->begin(); it != _l_left->end(); ++it)
	if ((*it)->getAddr() == addr_left)
	  {
	    node_left = *it;
	    // remove the node from the left list
	    if (_bindiff)
	      _bindiff->remove_node_from_left_list(node_left->getIdx());
	    break;
	  }

      // Get right node
      for (typename BinSlay::bind_node<NodeType>::NODES_LIST::const_iterator it =
	     _l_right->begin(); it != _l_right->end(); ++it)
	if ((*it)->getAddr() == addr_right)
	  {
	    node_right = *it;
	    // remove the node from the right list
	    if (_bindiff)
	      _bindiff->remove_node_from_right_list(node_right->getIdx());
	    break;
	  }

      // If the mapping is empty, add a new iso_list in it
      if (!_mapping)
	{
	  _mapping = new typename BinSlay::bind_node<NodeType>::MAPPING;
	  _mapping->push_front(new typename BinSlay::bind_node<NodeType>::ISOMORPHES_LIST);
	}

      // Add the new couple of isomorphisms to the mapping
      // level: -1 -> means that it has been matched manually
      (*_mapping->begin())->push_front(new BinSlay::Isomorphes<NodeType>(node_left, node_right, -1));
    }

    void remove_iso_couple_from_mapping(BinSlay::ReverseAPI::Address, BinSlay::ReverseAPI::Address)
    {
    }

    unsigned int get_nb_isomorphism_found() const
    {
      int nb_isomorphism = 0;

      if (_mapping)
	{
	  for (typename BinSlay::bind_node<FctNode>::MAPPING::const_iterator it_map =
		 _mapping->begin(); it_map != _mapping->end(); ++it_map)
	    {
	      for (typename BinSlay::bind_node<FctNode>::ISOMORPHES_LIST::const_iterator it_iso =
		     (*it_map)->begin(); it_iso != (*it_map)->end(); ++it_iso)
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

#endif // !ACORE_HH_
