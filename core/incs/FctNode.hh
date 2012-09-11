/**
 * \file FctNode.hh
 * \author Martial Bourquin
 * \version 1.0
 * \date 30/06/2012
 */

#ifndef FCT_NODE_HH_
# define FCT_NODE_HH_

# include "ANode.hh"

namespace BinSlay
{
  class FctNode : public ANode
  {
  public:
    FctNode();
    FctNode(unsigned int idx,	       		// index in the matrix representing the graph
	    unsigned long addr,			// addr: identifier in the lib used for parsing
	    int, int, int,		       	// label three-tuple
	    BinSlay::ReverseAPI::Crc32 crc32,	// crc32 based on opcodes
	    unsigned int size,			// size of function, block, ...
	    unsigned int nb_incomming_edges,
	    unsigned int nb_outcomming_edges,
	    std::string const &name
    );
    FctNode(FctNode const &o);
    ~FctNode();
    FctNode& operator=(FctNode const &);

    // Inlining functions
  public:
    inline std::string const &getName() const
    {
      return this->_name;
    }
    inline unsigned int getInEdges() const
    {
      return this->_nb_incomming_edges;
    }
    inline unsigned int getOutEdges() const
    {
      return this->_nb_outcomming_edges;
    }

  public:
    virtual unsigned int assign_substitution_cost(BinSlay::ANode const &) const;
    virtual unsigned int assign_insertion_cost() const;
    virtual unsigned int assign_deletion_cost() const;

  private:
    unsigned int		_nb_incomming_edges;
    unsigned int		_nb_outcomming_edges;
    std::string			_name;
  };
}

#endif // !FCT_NODE_HH_
