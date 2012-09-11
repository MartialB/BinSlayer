/**
 * \file BbNode.cpp
 * \author Martial Bourquin
 * \version 1.0
 * \date 30/06/2012
 */

#include "BbNode.hh"

BinSlay::BbNode::BbNode()
{
}

BinSlay::BbNode::BbNode(
	      unsigned int idx,
	      unsigned long addr,
	      int x, int y, int z,
	      BinSlay::ReverseAPI::Crc32 crc32,
	      unsigned int size,
	      unsigned int nb_incomming_edges,
	      unsigned int nb_outcomming_edges,
	      unsigned int nb_instrs)
  : BinSlay::ANode::ANode(idx, addr, x, y, z, crc32, size),
    _nb_incomming_edges(nb_incomming_edges),
    _nb_outcomming_edges(nb_outcomming_edges),
    _nb_instrs(nb_instrs)
{
}


// BinSlay::BbNode::BbNode(BinSlay::BbNode const &)
// {
  
// }


BinSlay::BbNode::~BbNode()
{
}


// BinSlay::BbNode& BinSlay::BbNode::operator=(BinSlay::BbNode const &o)
// {
// }

unsigned int BinSlay::BbNode::assign_substitution_cost(BinSlay::ANode const &o) const
{
  unsigned int cost = 0;

  // We do this static cast to allow the use of attributes from the derived class
  // in order to make the deletion cost the most accurate and relevant as possible
  BinSlay::BbNode const &to = static_cast<BinSlay::BbNode const &>(o); 
  // The cost for a substitution between two function nodes is equal to the relabeling
  // cost + the Neighborhood cost which is equal to the Neighborhood_cost_IN
  // and Neighborhood_cost_OUT.

	// relabeling cost
  cost += ABS(static_cast<int>(this->getNbInstrs()) - static_cast<int>(to.getNbInstrs()));

  	// Neighborhood cost IN
  cost += (this->getInEdges() >= to.getInEdges() ?
  	   static_cast<int>(this->getInEdges()) - static_cast<int>(to.getInEdges()) :
	   static_cast<int>(to.getInEdges()) - static_cast<int>(this->getInEdges()));

  	// Neighborhood cost OUT
  cost += (this->getOutEdges() >= to.getOutEdges() ?
	   static_cast<int>(this->getOutEdges()) - static_cast<int>(to.getOutEdges()) :
	   static_cast<int>(to.getOutEdges()) - static_cast<int>(this->getOutEdges()));

  return cost;
}

unsigned int BinSlay::BbNode::assign_insertion_cost() const
{
  return this->getNbInstrs() + this->getInEdges();
}

unsigned int BinSlay::BbNode::assign_deletion_cost() const
{
  return this->getNbInstrs() + this->getOutEdges();
}
