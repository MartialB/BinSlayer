/**
 * \file GraphED.hh
 * \author Martial Bourquin
 * \version 1.0
 * \date 30/06/2012
 */

#ifndef GRAPHED_HH_
# define GRAPHED_HH_

# include <stdlib.h>
# include "BinSlay.hpp"
# include "Node.hh"
# include "AGraph.hpp"
# include "Hungarian.hh"

namespace BinSlay
{
  typedef unsigned int GED;

  template<typename NodeType>
  class GraphED
  {
  public:
    GraphED(
	    AGraph<NodeType> const &g1,
	    AGraph<NodeType> const &g2,
	    typename BinSlay::bind_node<NodeType>::NODES_LIST &lg1,
	    typename BinSlay::bind_node<NodeType>::NODES_LIST &lg2)
      : _g1(g1),
	_g2(g2)
    {
      this->_edit_path = NULL;
      typename BinSlay::bind_node<NodeType>::NODES_LIST &setA = lg1;
      typename BinSlay::bind_node<NodeType>::NODES_LIST &setB = lg2;
      
      // Get original size of each graph
      this->_osize_g1 = setA.size();
      this->_osize_g2 = setB.size();
      
      // Set member '_ged' to the maximum
      this->_ged = -1;
      
      //
      // STEP0: Extend the number of nodes in the two graphs if their sizes are not equal.
      // 
      if (setA.size() != setB.size())
	{
	  unsigned nb_dummy_nodes_to_add_in_setA = setB.size();
	  unsigned nb_dummy_nodes_to_add_in_setB = setA.size();
	  
	  while (nb_dummy_nodes_to_add_in_setA--)
	    setA.push_back(NULL); // A 'null' element is a dummy node
	  while (nb_dummy_nodes_to_add_in_setB--)
	    setB.push_back(NULL); // A 'null' element is a dummy node
	  
	  //
	  // STEP1: Compute the size of the cost matrix which is a square matrix and initialise it.
	  //
	  //	Let 's_g1' be the size of the graph 'g1' which nodes are in the 'setA' and 's_g2'
	  // the size of the graph 'g2' which nodes are in the 'setB'. The resulting cost matrix
	  // would be of size: (s_g1 + s_g2) * (s_g2 + s_g1).
	  //      
	  this->_cost_matrix_size = (this->_osize_g1 + this->_osize_g2);
	}
      // Sizes are equal: this means no insertion and no deletion
      else
	{
	  this->_cost_matrix_size = this->_osize_g1;
	}
      
      // this->_cost_matrix.resize(this->_cost_matrix_size);
      // for (BinSlay::Hungarian::COST_MATRIX::iterator it = this->_cost_matrix.begin();
      //      it != this->_cost_matrix.end(); ++it)
      //   {
      //     it->resize(this->_cost_matrix_size);
      //   }
      
      // Fill '_cols' vector with nodes from list setB to cache data
      this->_rows.resize(setA.size());
      this->_cols.resize(setB.size());
      {
	unsigned int col = 0;
	for (typename BinSlay::bind_node<NodeType>::NODES_LIST::const_iterator it =
	       setB.begin(); it != setB.end(); ++it)
	  this->_cols[col++] = *it;
      }
      
      //      std::cout << "_cost_matrix_size: " << _cost_matrix_size << std::endl;
      
      // Allocate the Cost matrix
      m = (BinSlay::Hungarian::Cost*)malloc(sizeof(*m) * _cost_matrix_size * _cost_matrix_size);
      if (!m)
	{
	  std::cout << "MALLOC FAILED!" << std::endl;
	  exit(1);
	}
      // for (int i = 0; i < _cost_matrix_size; ++i)
      //   {
      //     m[i] = (BinSlay::Hungarian::Cost*)malloc(sizeof(**m) * _cost_matrix_size);
      //   }
      
      //
      // STEP2: Fill the cost matrix.
      //
      //	In the upper left corner is a submatrix of size s_g1 * s_g2 which represents the cost of node
      // substitution (e.g. the relabeling cost).
      //	In the upper right corner is a submatrix of size s_g1 * s_g2 which represents the cost of node
      // deletion.
      //	In the bottom right is a submatrix of size s_g1 * s_g2 which represents the cost of node
      // instertion.
      //	In the bottom right is a zero submatrix of size s_g1 * s_g2 which represents a zero-cost
      // substitution (e.g. associated a dummy node with another dummy node).
      //
      //	Note that the diagonal of the right upper corner represents the cost of all possible nodes
      // deletions, the diagonal of the bottom left corner the cost of all possible node insertations
      //
      unsigned int row = 0;  
      for (typename BinSlay::bind_node<NodeType>::NODES_LIST::const_iterator itA =
	     setA.begin(); itA != setA.end(); ++itA)
	{
	  // Fill '_rows' vector with nodes from list setA to cache data
	  this->_rows[row] = *itA;
	  
	  unsigned int col = 0;      
	  for (typename BinSlay::bind_node<NodeType>::NODES_LIST::const_iterator itB =
		 setB.begin(); itB != setB.end(); ++itB)
	    {
	      // upper left: substitution
	      if (*itA && *itB)
		{
		  // The cost for a substitution between two nodes is equal to the relabeling
		  // cost + the Neighborhood cost which is equal to the Neighborhood_cost_IN
		  // and Neighborhood_cost_OUT.
		  this->m[row * _cost_matrix_size + col]._cost =
		    (*itA)->assign_substitution_cost(**itB);
		    // // Relabeling cost
		    // (*itA)->getLabel().get_relabeling_cost((*itB)->getLabel())  +
		    // // Neighborhood cost IN
		    // ((*itA)->getInEdges() >= (*itB)->getInEdges() ? (*itA)->getInEdges() -
		    //  (*itB)->getInEdges() : (*itB)->getInEdges() - (*itA)->getInEdges()) +
		    // // Neighborhood cost OUT
		    // ((*itA)->getOutEdges() >= (*itB)->getOutEdges() ? (*itA)->getOutEdges() -
		    //  (*itB)->getOutEdges() : (*itB)->getOutEdges() - (*itA)->getOutEdges());
		}
	      // upper right: deletion in g1
	      else if (*itA && !*itB)
		{
		  if (col - this->_osize_g2 == row) 
		    this->m[row * _cost_matrix_size + col]._cost =
		      (*itA)->assign_deletion_cost();
		  else
		    this->m[row * _cost_matrix_size + col]._cost = 0xafffffff;
		}
	      // bottom left: insertion in g1
	      else if (!*itA && *itB)
		{
		  if (row - this->_osize_g1 == col)
		    this->m[row * _cost_matrix_size + col]._cost =
		      (*itB)->assign_insertion_cost();
		  else
		    this->m[row * _cost_matrix_size + col]._cost = 0xafffffff;
		}
	      // bottom right: dummy with dummy => 0 cost
	      else
		this->m[row * _cost_matrix_size + col]._cost = 0;
	      
	      // Init the booleans to false
	      this->m[row * _cost_matrix_size + col]._starred = false;
	      this->m[row * _cost_matrix_size + col]._primed = false;
	      this->m[row * _cost_matrix_size + col]._series = false;
	      
	      // Set the '_save_cost' filed
	      this->m[row * _cost_matrix_size + col]._saved_cost =
		this->m[row * _cost_matrix_size + col]._cost;
	      
	      ++col;
	    }
	  ++row;
	}


      // for (unsigned int i = 0; i < this->_rows.size(); ++i)
      // 	if (this->_rows[i]) std::cout << this->_rows[i]->getIdx() << ", ";
      // std::cout << std::endl;

      // for (unsigned int i = 0; i < _cols.size(); ++i)
      // 	if (this->_cols[i]) std::cout << this->_cols[i]->getIdx() << ", ";
      // std::cout << std::endl;

    }

    void update_cost(unsigned int cost, unsigned int row, unsigned int col)
    {
      m[row * _cost_matrix_size + col]._cost += cost;
    }

    void setEditPath(typename BinSlay::bind_node<NodeType>::ISOMORPHES_LIST *ep)
    {
      this->_edit_path = ep;
    }
    
    //    GraphED(GraphED const &);
    ~GraphED() {}
    //    GraphED &operator=(GraphED const &);
    
    GED compute()
    {
      // Solve the assignment problem with the Hungarian's algorithm
      if (_osize_g1 <= _osize_g2)
	BinSlay::Hungarian(this->m, this->_cost_matrix_size, true).solve();
      else
	BinSlay::Hungarian(this->m, this->_cost_matrix_size, false).solve();

      // Compute the approximation of the GED.
      this->_ged = 0;
      for (unsigned int row = 0; row < this->_cost_matrix_size; row++)
	for (unsigned int col = 0; col < this->_cost_matrix_size; col++)
	  if (this->m[row * _cost_matrix_size + col]._starred)
	    {
	      //	  std::cout << "Cost: " << _cost_matrix[row][col]._saved_cost << std::endl;
	      this->_ged += this->m[row * _cost_matrix_size + col]._saved_cost;
	    }
      //      std::cout << "ged: " << _ged << std::endl;


      typename BinSlay::bind_node<NodeType>::ISOMORPHES_LIST *ret =
	new typename BinSlay::bind_node<NodeType>::ISOMORPHES_LIST;
      
      // std::cout << "_cost_matrix_size: " << _cost_matrix_size << std::endl;
      // std::cout << "_rows_size: " << _rows.size() << std::endl;
      // std::cout << "_cols_size: " << _cols.size() << std::endl;
      unsigned int nb_substitutions = 0;
      unsigned int nb_insertions = 0;
      unsigned int nb_deletions = 0;
      for (unsigned int row = 0; row < this->_cost_matrix_size; ++row)
	for (unsigned int col = 0; col < this->_cost_matrix_size; ++col)
	  if (this->m[row * _cost_matrix_size + col]._starred)
	    {
	      if (!(!_rows[row] && !_cols[col]))
		{
		  ret->push_front(new BinSlay::Isomorphes<NodeType>(
		    _rows[row], _cols[col], this->m[row * _cost_matrix_size + col]._saved_cost));
		  // std::cout << "edit cost: " << this->m[row * _cost_matrix_size + col]._saved_cost
		  // 	    << std::endl;
		  if (_rows[row] && _cols[col])
		    ++nb_substitutions;
		  else if (!_rows[row] && _cols[col])
		    ++nb_insertions;
		  if (_rows[row] && !_cols[col])
		    ++nb_deletions;
		}
	    }
      // std::cout << "Nb substitutions: " << nb_substitutions << std::endl;
      // std::cout << "Nb insertions: " << nb_insertions << std::endl;
      // std::cout << "Nb deletions: " << nb_deletions << std::endl;
      this->_edit_path = ret;
      return this->_ged;
    }
    
    GED getGED()
    {
      if (this->_ged == 0xffffffff)
	this->compute();
      return this->_ged;
    }

    typename BinSlay::bind_node<NodeType>::ISOMORPHES_LIST *get_edit_path() const
    {
      return this->_edit_path;
    }

  private:
    AGraph<NodeType> const &_g1;
    AGraph<NodeType> const &_g2;
    unsigned int _osize_g1;
    unsigned int _osize_g2;
    unsigned int _cost_matrix_size;
    BinSlay::Hungarian::Cost *m;
    typename BinSlay::bind_node<NodeType>::ISOMORPHES_LIST *_edit_path;

    // Use to ease the mapping of nodes between the two sets at
    // the end of the algorithm: create the IsoList.
    std::vector<NodeType *> _rows;
    std::vector<NodeType *> _cols;

    GED	_ged;
  };
}

#endif // !GRAPHED_HH_
