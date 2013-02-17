/**
 * \file FctNode.cpp
 * \author Martial Bourquin
 * \version 1.0
 * \date 30/06/2012
 */

#include "FctNode.hh"

BinSlay::FctNode::FctNode()
{
}

BinSlay::FctNode::FctNode(
	      unsigned int idx,
	      unsigned long addr,
	      int x, int y, int z,
	      BinSlay::ReverseAPI::Crc32 crc32,
	      unsigned int size,
	      unsigned int nb_incomming_edges,
	      unsigned int nb_outcomming_edges,
	      std::string const &name)
  : BinSlay::ANode::ANode(idx, addr, x, y, z, crc32, size),
    _nb_incomming_edges(nb_incomming_edges),
    _nb_outcomming_edges(nb_outcomming_edges),
    _name(name)
{
}


// BinSlay::FctNode::FctNode(BinSlay::FctNode const &)
// {
  
// }


BinSlay::FctNode::~FctNode()
{
}


// BinSlay::FctNode& BinSlay::FctNode::operator=(BinSlay::FctNode const &o)
// {
// }

unsigned int BinSlay::FctNode::assign_substitution_cost(BinSlay::ANode const &o) const
{
  unsigned int cost = 0;

  // We do this static cast to allow the use of attributes from the derived class
  // in order to make the substitution cost the most accurate and relevant as possible
  BinSlay::FctNode const &to = static_cast<BinSlay::FctNode const &>(o);

  // The cost for a substitution between two function nodes is equal to the relabeling
  // cost + the Neighborhood cost which is equal to the Neighborhood_cost_IN
  // and Neighborhood_cost_OUT.

  	// relabeling cost
  cost += this->getLabel().get_relabeling_cost(to.getLabel());

   	// Neighborhood cost IN
  cost += this->getInEdges() >= to.getInEdges() ?
    static_cast<int>(this->getInEdges()) - static_cast<int>(to.getInEdges()) :
    static_cast<int>(to.getInEdges()) - static_cast<int>(this->getInEdges());

  	// Neighborhood cost OUT
  cost += this->getOutEdges() >= to.getOutEdges() ?
    static_cast<int>(this->getOutEdges()) - static_cast<int>(to.getOutEdges()) :
    static_cast<int>(to.getOutEdges()) - static_cast<int>(this->getOutEdges());

  // return to.getLabel().getX() + to.getLabel().getY() + to.getLabel().getZ()
  //   + this->getLabel().getX() + this->getLabel().getY() + this->getLabel().getZ();
  return cost;
}

unsigned int BinSlay::FctNode::assign_insertion_cost() const
{
  // The cost for an insertion is equal to the relabeling
  // cost + the Neighborhood cost which is equal to the Neighborhood_cost_IN
  // + the the Neighborhood_cost_OUT
  return this->getLabel().sum() + this->getInEdges() + this->getOutEdges();
}

unsigned int BinSlay::FctNode::assign_deletion_cost() const
{
  // The cost for a deletion is equal to the relabeling
  // cost + the Neighborhood cost which is equal and Neighborhood_cost_OUT
  // + the Neighborhood_cost_IN
  return this->getLabel().sum() + this->getOutEdges() + this->getInEdges();
}
