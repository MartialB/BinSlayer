/**
 * \file BbNode.hh
 * \author Martial Bourquin
 * \version 1.0
 * \date 30/06/2012
 */

#ifndef BB_NODE_HH_
# define BB_NODE_HH_

# include "ANode.hh"

namespace BinSlay
{
  class BbNode : public ANode
  {
  public:
    BbNode();
    BbNode(unsigned int idx,				// index in the matrix representing the graph
	   unsigned long addr,				// addr: identifier in the lib used for parsing
	   int, int, int,				// label three-tuple
	   BinSlay::ReverseAPI::Crc32 crc32,		// crc32 based on opcodes
	   unsigned int size,
	   unsigned int nb_incomming_edges,	       	// nb_incomming calls
	   unsigned int nb_outcomming_edges,     	// nb_edges_out
	   unsigned int nb_instrs     		       	// nb_instrs
    );
    BbNode(BbNode const &o);
    ~BbNode();
    BbNode& operator=(BbNode const &);

    // Inlining functions
  public:
    inline unsigned int getInEdges() const
    {
      return this->_nb_incomming_edges;
    }
    inline unsigned int getOutEdges() const
    {
      return this->_nb_outcomming_edges;
    }
    inline unsigned int getNbInstrs() const
    {
      return this->_nb_instrs;
    }

  public:
    virtual unsigned int assign_substitution_cost(BinSlay::ANode const &) const;
    virtual unsigned int assign_insertion_cost() const;
    virtual unsigned int assign_deletion_cost() const;

  private:
    unsigned int		_nb_incomming_edges;
    unsigned int		_nb_outcomming_edges;
    unsigned int		_nb_instrs;
  };

}

#endif // !BB_NODE_HH_
