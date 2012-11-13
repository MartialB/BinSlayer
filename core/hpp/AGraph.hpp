/**
 * \file AGraph.hh
 * \author Martial Bourquin
 * \version 1.0
 * \date 30/06/2012
 */

#ifndef AGRAPH_HH_
# define AGRAPH_HH_

# include <string>
# include <vector>
# include <utility>
# include <fstream>
# include "Node.hh"
# include "BinSlay.hpp"

namespace BinSlay
{
  template<typename NodeType>
  class AGraph
  {
  protected:
    unsigned int	_nbNode;
    typename BinSlay::bind_node<NodeType>::NODES_LIST _matrix;

  public:
    AGraph() {}

    void drawMe(std::string const &path) const
    {
      std::fstream fs((path + ".dot").c_str(),
      		      std::fstream::in | std::fstream::out | std::fstream::trunc);
  
      fs << "digraph G {" << std::endl /*<< "rankdir=LR; ranksep=0.5; size=\"1,3\"; ratio=fill;"*/
      	 << std::endl << "node [shape=box, style=filled, color=lightgrey, fontsize=22,"
      	 << " fixedsize=true, width=2.5, height=1];" << std::endl;
      for (typename std::vector<NodeType *>::const_iterator it = this->_matrix.begin();
      	   it != this->_matrix.end(); ++it)
      	{
      	  int idx = 0;
      	  fs << (*it)->getIdx() << " -> {";
      	  for (unsigned int x = 0; x < (*it)->getEdges().size(); ++x)
      	    {
      	      if ((*it)->getEdges()[x] == 1)
      		fs << idx << "; ";
      	      ++idx;
      	    }
      	  fs << "}" << std::endl;
      	}
      for (typename std::vector<NodeType *>::const_iterator it = this->_matrix.begin();
      	   it != this->_matrix.end(); ++it)
      	fs << (*it)->getIdx() << " [label=\"" << (*it)->getIdx() << ":("
      	   << (*it)->getLabel().getX() << ","
      	   << (*it)->getLabel().getY() << "," << (*it)->getLabel().getZ() << ")\"];";
      fs << "}";
    }
    
    void setnbNode(unsigned int nbNode)
    {
      this->_nbNode = nbNode;
    }
    
    unsigned int getnbNode() const
    {
      return this->_nbNode;
    }
    
    typename BinSlay::bind_node<NodeType>::NODES_LIST *CreateListOfNodes() const
    {
      typename BinSlay::bind_node<NodeType>::NODES_LIST *ret =
	new typename BinSlay::bind_node<NodeType>::NODES_LIST;
      
      for (typename BinSlay::bind_node<NodeType>::NODES_LIST::const_iterator it = this->_matrix.begin();
	   it != this->_matrix.end(); ++it)
	{
	  ret->push_back(*it);
	}
      return ret;
    }
    
    void displayMatrix() const
    {
      for (int i = 0; i < this->getnbNode(); i++)
	{
	  std::cout << std::dec << this->_matrix[i]->getIdx() << " ";
	  this->_matrix[i]->displayLabel();
	  for (int j = 0; j < this->getnbNode(); j++)
	    {
	      std::cout << std::dec << this->_matrix[i]->getEdges()[j] << " ";
	    }
	  std::cout << std::endl;
	}
      
    }
    
    ~AGraph()
    {
      for (typename BinSlay::bind_node<NodeType>::NODES_LIST::iterator it = this->_matrix.begin();
	   it != this->_matrix.end(); ++it)
	delete *it;
    }
    
    typename BinSlay::bind_node<NodeType>::NODES_LIST const &getMatrix() const
    {
      return this->_matrix;
    }

    // void setnbNode(int);
    // int getnbNode() const;
    // void displayMatrix() const;
    // std::vector<NodeType *> const & getMatrix() const;
    // NODES_LIST *CreateListOfNodes() const;
    // void drawMe(std::string const &path) const;
  };
}

#endif // !AGRAPH_HH_
