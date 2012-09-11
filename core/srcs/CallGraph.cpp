/**
 * \file CallGraph.cpp
 * \author Martial Bourquin
 * \version 1.0
 * \date 30/06/2012
 */

#include <fstream>
#include <iostream>
#include "CallGraph.hh"

/**
 * \brief	Constructor : Initialise the CallGraph object with a vector
 *		of Nodes.
 *
 * \param[in]	graphDesc File which contains the description of the graph.
 */

BinSlay::CallGraph::CallGraph(BinSlay::ReverseAPI::IBinary *bin)
  : BinSlay::AGraph<BinSlay::FctNode>::AGraph()
{
  BinSlay::ReverseAPI::CG &cg = *bin->recover_call_graph();

  this->_nbNode = cg.size();
  this->_matrix.resize(cg.size());
  for (unsigned int i = 0; i < cg.size(); ++i)
    {
      BinSlay::FctNode *newNode = new BinSlay::FctNode(
		      i,
		      cg[i]->_addr, // addr
		      cg[i]->_nb_basic_blocks,
		      cg[i]->_nb_internal_edges,
		      cg[i]->_nb_outgoing_calls,
		      bin->get_function_crc32(cg[i]->_addr), // Crc32
		      bin->get_function_size(cg[i]->_addr), // Size
		      cg[i]->_nb_incomming_calls,
		      cg[i]->_nb_outgoing_calls,
		      bin->get_function_symname(cg[i]->_addr) // SymName
      );
      this->_matrix[i] = newNode;
      newNode->setEdges().resize(cg.size());      
      for (unsigned int j = 0; j < cg.size(); j++)
	{
	  bool linked = false;
	  for (std::list<unsigned long>::const_iterator it2 = cg[i]->_link_to.begin();
	       it2 != cg[i]->_link_to.end(); ++it2)
	    {
	      if (cg[j]->_addr == *it2)
		{
		  linked = true;
		  newNode->setEdges()[j] = 1;
		  break;
		}
	    }
	  if (linked == false)
	    newNode->setEdges()[j] = 0;
	}
    }
  delete &cg;
}

// BinSlay::CallGraph::CallGraph(CallGraph const &o)
//   : BinSlay::AGraph::AGraph()
// {
  
// }


BinSlay::CallGraph::~CallGraph()
{
}

// BinSlay::CallGraph& BinSlay::CallGraph::operator=(BinSlay::CallGraph const &o)
// {
//   return *this;
// }

unsigned int BinSlay::CallGraph::get_total_nb_of_basic_blocks() const
{
  unsigned int total_nb_of_basic_blocks = 0;

  for (unsigned int i = 0; i < this->_nbNode; ++i)
    {
      total_nb_of_basic_blocks += this->_matrix[i]->getLabel().getX();
    }
  return total_nb_of_basic_blocks;
}

unsigned int BinSlay::CallGraph::get_total_nb_of_edges() const
{
  unsigned int total_nb_of_edges = 0;

  for (unsigned int i = 0; i < this->_nbNode; ++i)
    {
      total_nb_of_edges += this->_matrix[i]->getLabel().getY();
    }
  return total_nb_of_edges;
}
