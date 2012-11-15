/**
 * \file Bindiff.hhp
 * \author Martial Bourquin
 * \version 1.0
 * \date 30/06/2012
 */

#ifndef __BINDIFF_HHP__
# define __BINDIFF_HHP__

# include <vector>
# include "BinSlay.hpp"
# include "AGraph.hpp"
# include "ISelector.hpp"
# include "IProperty.hpp"
# include "Isomorphes.hpp"

namespace BinSlay
{
  template<typename NodeType>
  class Bindiff
  {
  public:
    Bindiff(typename BinSlay::bind_node<NodeType>::NODES_LIST &left,
	    typename BinSlay::bind_node<NodeType>::NODES_LIST &right,
	    typename BinSlay::bind_node<NodeType>::SELECTORS const &selectors,
	    typename BinSlay::bind_node<NodeType>::PROPERTIES const &properties)
      : _left(left),
	_right(right),
	_selectors(selectors),
	_properties(properties)
    {  
      // Init fast lookup table _left
      _left_lookup.resize(_left.size());
      for (size_t i = 0; i < _left.size(); i++)
	_left_lookup[i] = i;
      // Init fast lookup table _right
      _right_lookup.resize(_right.size());
      for (size_t i = 0; i < _right.size(); i++)
	_right_lookup[i] = i;
    }

    ~Bindiff()
    {
      _left_lookup.clear();
      _right_lookup.clear();
    }

    static
    typename BinSlay::bind_node<NodeType>::NODES_LIST *
    GetlistOfAllParents(
	typename BinSlay::bind_node<NodeType>::NODES_LIST const &nodes,
	NodeType const &node
    )
    {
      auto *ret = new typename BinSlay::bind_node<NodeType>::NODES_LIST;
      
      ret->reserve((nodes.size() * sizeof(ret)) / 4);
      for (size_t k = 0; k < nodes.size(); ++k) {
	if (nodes[k]->getEdges()[node.getIdx()])
	  ret->push_back(nodes[k]);
      }
      return ret;
    }
    
    static
    typename BinSlay::bind_node<NodeType>::NODES_LIST *
    GetlistOfAllChildrens(
	typename BinSlay::bind_node<NodeType>::NODES_LIST const &nodes,
	NodeType const &node
    )
    {
      auto *ret = new typename BinSlay::bind_node<NodeType>::NODES_LIST;
      
      ret->reserve((nodes.size() * sizeof(ret)) / 4);
      for (size_t i = 0; i < node.getEdges().size(); ++i) {
	if (node.getEdges()[i])
	  for (size_t k = 0; k < nodes.size(); ++k) {
	    if (nodes[k]->getIdx() == i)
	      ret->push_back(nodes[k]);
	  }
      }
      return ret;
    }

    void run(typename BinSlay::bind_node<NodeType>::MAPPING &mapping)
    {
      unsigned int ret = 0;
      // Get the mapping resulting from the first run of the BinDiff algorithm
      this->_run(mapping);
      // Re-Run the algorithm until no further matches can be found
      while ((ret = this->re_run(mapping)));
    }

    unsigned int
    re_run(
	typename BinSlay::bind_node<NodeType>::MAPPING &current_mapping
    )
    {
      unsigned int ret = 0;
      // Re-run the algorithm with the previous unmatched lists
      typename BinSlay::bind_node<NodeType>::MAPPING new_mapping;
      this->_run(new_mapping);
      // Update the current mapping with the new one, if any
      // and get the number of new isomorphism found
      if (new_mapping.size()) {
	for (auto it_map = new_mapping.begin(); it_map != new_mapping.end(); ++it_map) {
	  for (auto it_iso = (*it_map)->begin(); it_iso != (*it_map)->end(); ++it_iso) {
	    ++ret;
	  }
	}
	current_mapping.insert(current_mapping.end(), new_mapping.begin(), new_mapping.end());
      }
      return ret;
    }

    // TODO: detail explanation of how to handle those list of nodes
    void
    remove_node_from_left_list(int node_idx)
    {
      if (!_left.size())
	return;
      
      int tmp = _left[_left.size() - 1]->getIdx();
      int ivl = _left_lookup[node_idx];		// get ivl
      _left_lookup[node_idx] = -1;
      _left[ivl] = _left[_left.size() - 1];    	// swap node
      _left[_left.size() - 1] = nullptr;
      _left_lookup[tmp] = ivl;			// update ivl
      _left.pop_back();				// discard: O(1) complexity
    }
    
    void
    remove_node_from_right_list(int node_idx)
    {
      if (!_right.size())
	return;
      
      int tmp = _right[_right.size() - 1]->getIdx();
      int ivl = _right_lookup[node_idx];       	// get ivl
      _right_lookup[node_idx] = -1;
      _right[ivl] = _right[_right.size() - 1];	// swap
      _right[_right.size() - 1] = NULL;
      _right_lookup[tmp] = ivl;		       	// update ivl
      _right.pop_back();	       	       	// discard: O(1) complexity
    }
    
    void
    drawResults(
	std::string const &path,
	typename BinSlay::bind_node<NodeType>::MAPPING *mapping,
	AGraph<NodeType> const &graph
    )
    {
      std::fstream fs((path + ".dot").c_str(),
		      std::fstream::in |
		      std::fstream::out |
		      std::fstream::trunc
		     );

      fs << "digraph " << path.c_str() << " {" << std::endl
      	 << "\tnode [style=filled, color=lightgrey, fontsize=8,"
      	 << " fixedsize=true, width=1.5, height=0.75];" << std::endl << std::endl;

      for (auto it = graph.getMatrix().begin(); it != graph.getMatrix().end(); ++it) {
      	  BinSlay::Isomorphes<NodeType> *iso = nullptr;
      	  int idx = 0;

	  // TODO: need something more generic
      	  if (path == "res_left")
      	    iso = _GetIsomorphes_left(mapping, *it);
      	  else
      	    iso = _GetIsomorphes_right(mapping, *it);
      	  fs << "\t" << (*it)->getIdx() << " -> {";
      	  for (size_t x = 0; x < (*it)->getEdges().size(); ++x) {
	    if ((*it)->getEdges()[x] == 1)
	      fs << idx << "; ";
	    ++idx;
	  }
      	  fs << "}" << std::endl << "\t\t" << (*it)->getIdx() << " [label=\"" << std::hex
      	     << (*it)->getAddr() << std::dec << ":("
      	 << (*it)->getLabel().getX() << "," << (*it)->getLabel().getY()
      	     << "," << (*it)->getLabel().getZ() << ")";
      	  if (iso)
      	    fs << "Lvl=" << iso->getLevel();
      	  fs << "\"";
      	  if (iso)
      	    fs << ", color=red";
      	  fs << "];" << std::endl;
      	}
      fs << "}";
      fs.close();  
    }

  private:
    void _run(typename BinSlay::bind_node<NodeType>::MAPPING &mapping)
    {
      typename BinSlay::bind_node<NodeType>::ISOMORPHES_LIST *IsoList = nullptr;
      int level = 0;
      unsigned int max_nb_nodes = _left.size() < _right.size() ? _right.size() : _left.size();
      
      IsoList = _GetIsoList(_left, _right, level);
      // Add the first isomorphisms found to the mapping
      if (IsoList->size())
  	mapping.push_back(IsoList);
      // Recursion
      if (IsoList->size() < max_nb_nodes)
      	_recFindIso(mapping, ++level); 
    }

    typename BinSlay::bind_node<NodeType>::ISOMORPHES_LIST *
    _GetIsoList(
	typename BinSlay::bind_node<NodeType>::NODES_LIST &setA,
	typename BinSlay::bind_node<NodeType>::NODES_LIST &setB,
	int level
    )
    {
      typename BinSlay::bind_node<NodeType>::ISOMORPHES_LIST *fIso = nullptr;
      NodeType *isomorphe_left = nullptr;
      NodeType *isomorphe_right = nullptr;
      int idx_left = -1;
      int idx_right = -1;
      
      fIso = new typename BinSlay::bind_node<NodeType>::ISOMORPHES_LIST;
      for (size_t l = 0; l < setA.size(); /*no incrementation here*/) {

	for (auto it_selec = _selectors.begin(); it_selec != _selectors.end(); ++it_selec) {
	  isomorphe_left = nullptr;
	  isomorphe_right = nullptr;
	  idx_left = -1;
	  idx_right = -1;

	  // No selectors available
	  if (!*it_selec)
	    continue;
	  // Set the node we want to compare in the selector and call the comparison function:
	  // check if the current node in unique in its set
	  (*it_selec)->setNode(setA[l]);
	  isomorphe_left = (**it_selec)(setA);
	  idx_left = (*it_selec)->getIdx();
	  // Set the node we want to compare in the selector and call the comparison function:
	  // check if the current node exist and is unique in the other set
	  (*it_selec)->setNode(setA[l]);
	  isomorphe_right = (**it_selec)(setB);
	  idx_right = (*it_selec)->getIdx();
	  // If we found an couple of isomorphisms, no need to try an other selector
	  if (isomorphe_left && isomorphe_right)
	    break;
	}

	// If we have found a couple of isomorphisms, we add it to the iso list and we remove
	// the nodes which are isomorphic from their respective set to avoid matching them again
	// and again...
	if (isomorphe_left && isomorphe_right) {
	  fIso->push_back(new BinSlay::Isomorphes<NodeType>(
			       	      isomorphe_left,
				      isomorphe_right,
				      level
			      ));
	  // Remove these nodes from their input list, so that we not loop again and again
	  // with the same couple of isomorphism
	  if (!level) {
	    // TODO: why special case for level == 0 ? Need explanation
	    remove_node_from_left_list(isomorphe_left->getIdx());
	    remove_node_from_right_list(isomorphe_right->getIdx());
	  } else {
	    setA[idx_left] = setA[setA.size() - 1];		// swap
	    setA.pop_back();					// discard: O(1) complexity
	    setB[idx_right] = setB[setB.size() - 1];		// swap
	    setB.pop_back();					// discard: O(1) complexity
	  }
	} else {
	  // Incrementation
	  ++l;
	}
      }
      return fIso;
    }

    unsigned int
    _recFindIso(
	typename BinSlay::bind_node<NodeType>::MAPPING &mapping,
	int &recLevel
    )
    {
      //
      // We need to go deeper in the comparison: for each couple of isomorphisms
      // found, we retry all the neighbours of each two nodes in the couple and then we
      // redo the previous step again until any further isomorphisms can be found or until
      // we reach the leafes.
      //
      unsigned int nbIso = 0;
      // If the mapping is emtpy, we do nothing
      if (!mapping.size()) return nbIso;

      for (auto it_map = mapping.begin(); it_map != mapping.end(); ++it_map) {
	for (auto it_iso = (*it_map)->begin(); it_iso != (*it_map)->end(); ++it_iso) {
	  //
	  // We need to define as many as possible properties here in order
	  // to reduce the sets and increase the chance that the seclector returns
	  // a non empty set.
	  //
	  // We currently have the property "CHILDRENS" and the property "PARENTS".
	  //
	  for (auto it_property = _properties.begin(); it_property != _properties.end();
	       ++it_property) {
	    // No property available - nullptr
	    if (!*it_property)
	      continue;

	    typename BinSlay::bind_node<NodeType>::NODES_LIST *subset01 = nullptr;
	    typename BinSlay::bind_node<NodeType>::NODES_LIST *subset02 = nullptr;

	    // Set the node on which the property will be applied
	    (*it_property)->setNode((*it_iso)->getLeft());
	    subset01 = (**it_property)(_left);
	    // Set the node on which the property will be applied
	    (*it_property)->setNode((*it_iso)->getRight());
	    subset02 = (**it_property)(_right);

	    // Handle subsets
	    if (subset01->size() && subset02->size()) {
	      // This list holds all couples of isomorphisms found
	      auto *fIso = this->_GetIsoList(*subset01, *subset02, recLevel);
	      // Remove already found nodes from the two being compared graphs
	      for (auto it2 = fIso->begin(); it2 != fIso->end(); ++it2) {
		remove_node_from_left_list((*it2)->getLeft()->getIdx());
		remove_node_from_right_list((*it2)->getRight()->getIdx());
	      }
	      // Update nbIso
	      nbIso += fIso->size();
	      if (fIso->size())
		mapping.push_back(fIso);

	    }
	    delete subset01;
	    delete subset02;
	  }
	}
      }      
      return nbIso;
    }
    
    Isomorphes<NodeType> *
    _GetIsomorphes_left(
	typename BinSlay::bind_node<NodeType>::MAPPING &mapping,
	NodeType *node
    )
    {
      for (auto it_map = mapping.begin(); it_map != mapping.end(); ++it_map) {
	for (auto it_iso = (*it_map)->begin(); it_iso != (*it_map)->end(); ++it_iso) {
	  if ((*it_iso)->getLeft()->getIdx() == node->getIdx())
	    return *it_iso;
	}
      }
      return nullptr;
    }
    
    Isomorphes<NodeType> *
    _GetIsomorphes_right(
       	typename BinSlay::bind_node<NodeType>::MAPPING &mapping,
	NodeType *node
    )
    {
      for (auto it_map = mapping.begin(); it_map != mapping.end(); ++it_map) {
	for (auto it_iso = (*it_map)->begin(); it_iso != (*it_map)->end(); ++it_iso) {
	  if ((*it_iso)->getRight()->getIdx() == node->getIdx())
	    return *it_iso;
	}
      }
      return nullptr;
    }

  private:
    typename BinSlay::bind_node<NodeType>::NODES_LIST		&_left;
    typename BinSlay::bind_node<NodeType>::NODES_LIST		&_right;
    typename BinSlay::bind_node<NodeType>::SELECTORS	       	const &_selectors;
    typename BinSlay::bind_node<NodeType>::PROPERTIES	       	const &_properties;

    // Need Explanation or even better -> encapsulate this special container in
    // a true object
    std::vector<int>   	_left_lookup;
    std::vector<int>   	_right_lookup;
  };
}

#endif // !__BINDIFF_HHP__
