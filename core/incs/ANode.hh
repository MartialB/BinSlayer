/**
 * \file ANode.hh
 * \author Martial Bourquin
 * \version 1.0
 * \date 30/06/2012
 */

#ifndef ANODE_HH_
# define ANODE_HH_

# include <deque>
# include <string>
# include <valarray>
# include <vector>
# include "Label.hh"
# include "IBinary.hh"

namespace BinSlay
{
  typedef std::valarray<char>	EDGES;

  class ANode
  {
  public:
    ANode();
    ANode(unsigned int idx,    			// index in the matrix representing the graph
	 unsigned long addr,			// addr: identifier in the lib used for parsing
	 int = 0, int = 0, int = 0,		// label three-tuple
	 BinSlay::ReverseAPI::Crc32 crc32 = 0,	// crc32 based on opcodes
	 unsigned int size = 0			// size of function, block, ...
    );
    ANode(ANode const &o);
    ~ANode();
    ANode& operator=(ANode const &);

  public:
    virtual unsigned int assign_substitution_cost(ANode const &) const = 0;
    virtual unsigned int assign_insertion_cost() const = 0;
    virtual unsigned int assign_deletion_cost() const = 0;

    // Inlining functions
  public:
    inline unsigned int getIdx() const
    {
      return this->_idx;
    }
    inline EDGES const & getEdges() const
    {
      return this->_edges;
    }
    inline BinSlay::Label const & getLabel() const
    {
      return this->_label;
    }
    inline unsigned long getAddr() const
    {
      return this->_addr;
    }
    inline BinSlay::ReverseAPI::Crc32 getCrc32() const
    {
      return this->_crc32;
    }
    inline unsigned int getSize() const
    {
      return this->_size;
    }

    inline EDGES& setEdges()
    {
      return this->_edges;
    }

  public:
    void displayLabel() const;

  private:
    unsigned int	       	_idx;
    unsigned long		_addr;
    Label			_label;
    BinSlay::ReverseAPI::Crc32	_crc32;
    unsigned int		_size;

    // This one could but a bits field
    EDGES			_edges;
  };
}

#endif // !ANODE_HH_
