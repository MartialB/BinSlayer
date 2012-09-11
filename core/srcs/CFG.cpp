/**
 * \file CFG.cpp
 * \author Martial Bourquin
 * \version 1.0
 * \date 30/06/2012
 */

#include <fstream>
#include <iostream>
#include "CFG.hh"

/**
 * \brief	Constructor : Initialise the CFG object with a vector
 *		of Nodes.
 *
 * \param[in]	graphDesc File which contains the description of the graph.
 */

BinSlay::CFG::CFG(BinSlay::ReverseAPI::IBinary *bin, BinSlay::ReverseAPI::Address addr)
  : BinSlay::AGraph<BinSlay::BbNode>::AGraph()
{
  BinSlay::ReverseAPI::CFG &cfg = *bin->recover_function_cfg(addr);

  this->_nbNode = cfg.size();
  this->_matrix.resize(cfg.size());
  for (unsigned int i = 0; i < cfg.size(); ++i)
    {
      BinSlay::BbNode *newNode = new BinSlay::BbNode(
			 i,
			 cfg[i]->_addr, // addr
			 cfg[i]->_nb_jump_from_entry,
			 cfg[i]->_nb_jump_to_exit,
			 cfg[i]->_nb_outgoing_calls,
			 cfg[i]->_crc32,
			 0, // size
			 cfg[i]->_nb_edges_in,
			 cfg[i]->_nb_edges_out,
			 cfg[i]->_nb_instrs
	  );
      this->_matrix[i] = newNode;
      newNode->setEdges().resize(cfg.size());      
      for (unsigned int j = 0; j < cfg.size(); j++)
	{
	  bool linked = false;
	  for (std::list<unsigned long>::const_iterator it2 = cfg[i]->_link_to.begin();
	       it2 != cfg[i]->_link_to.end(); ++it2)
	    {
	      if (cfg[j]->_addr == *it2)
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
  delete &cfg;
}

/**
 * \brief	Contructor per copy.
 *
 * \param[in]	o The other instance of CFG object we want to copy.
 */

// BinSlay::CFG::CFG(CFG const &o)
// {

// }

/**
 * \brief	Destructor : delete all allocated data.
 */

BinSlay::CFG::~CFG()
{
}

/**
 * \brief	Contructor per assignation
 */

// BinSlay::CFG& BinSlay::CFG::operator=(BinSlay::CFG const &o)
// {
  
// }

/**
 * \brief	Retrieve a basic block, type 'Node *', in the CFG.
 */

BinSlay::BbNode *BinSlay::CFG::getBasicBlock(BinSlay::ReverseAPI::Address addr) const
{
  for (std::vector<BinSlay::BbNode *>::const_iterator it = this->getMatrix().begin();
       it != this->getMatrix().end(); ++it)
    {
      if ((*it)->getAddr() == addr)
	return *it;
    }
  return NULL;
} 

