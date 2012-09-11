/**
 * \file Node.hh
 * \author Martial Bourquin
 * \version 1.0
 * \date 30/06/2012
 */

#ifndef NODE_HH_
# define NODE_HH_

# include <deque>
# include <string>
# include <valarray>
# include <vector>
# include "Label.hh"
# include "IBinary.hh"

namespace BinSlay
{
  typedef std::valarray<char>	EDGES;

  class Node
  {
  public:
    Node();
    Node(unsigned int idx,	       		// index in the matrix representing the graph
	 unsigned long addr,			// addr: identifier in the lib used for parsing
	 int = 0, int = 0, int = 0,		// label three-tuple
	 BinSlay::ReverseAPI::Crc32 crc32 = 0,	// crc32 based on opcodes
	 unsigned int = 0,	       	       	// nb_incomming calls OR nb_edges_in
	 unsigned int = 0,     		       	// nb_edges_out OR Nothing
	 unsigned int = 0,     		       	// nb_instrs OR Nothing
	 unsigned int size = 0,			// size of function, block, ...
	 std::string const &name = ""		// name (for functions only)
    );
    Node(Node const &o);
    ~Node();
    Node& operator=(Node const &);

    inline unsigned int getIdx() const
    {
      return this->_idx;
    }
    inline EDGES const & getEdges() const
    {
      return this->_edges;
    }
    // int getIdx() const;
    // std::vector<int> const & getEdges() const;
    void displayLabel() const;
    inline BinSlay::Label const & getLabel() const
    {
      return this->_label;
    }
    unsigned long getAddr() const;
    std::string const &getName() const;
    BinSlay::ReverseAPI::Crc32 getCrc32() const;
    unsigned int getSize() const;
    unsigned int getInEdges() const;
    unsigned int getOutEdges() const;

    inline EDGES& setEdges()
    {
      return this->_edges;
    }

  private:
    unsigned int	       	_idx;
    unsigned long		_addr;
    Label			_label;
    BinSlay::ReverseAPI::Crc32	_crc32;
    unsigned int		_nb_incomming_edges;
    unsigned int		_nb_outcomming_edges;
    unsigned int		_nb_instrs;
    unsigned int		_size;
    std::string			_name;

    // This one could but a bits field
    EDGES			_edges;
  };

}

#endif // !NODE_HH_
