/**
 * \file Crc32Selector.hh
 * \author Martial Bourquin
 * \version 1.0
 * \date 30/06/2012
 */

#ifndef CRC32_SELECTOR_HH_
# define CRC32_SELECTOR_HH_

# include <list>
# include "BinSlay.hpp"
# include "Node.hh"
# include "ISelector.hpp"

namespace BinSlay
{
  template<typename NodeType>
  class Crc32Selector : public ISelector<NodeType>
  {
  public:
    Crc32Selector() {}
    Crc32Selector(NodeType *node) : _node(node) {}
    virtual ~Crc32Selector() {}

  public:
    virtual NodeType *operator()(typename BinSlay::bind_node<NodeType>::NODES_LIST const &setOfNodes)
    {
      int count = 0, i = 0;
      NodeType *find = NULL;
      _idx = -1;
      
      for (typename BinSlay::bind_node<NodeType>::NODES_LIST::const_iterator it =
	     setOfNodes.begin(); it != setOfNodes.end(); ++it)
	{
	  if ((*it)->getCrc32() == this->_node->getCrc32())
	    {
	      ++count;
	      find = *it;
	      _idx = i;
	      if (count > 1)
		return NULL;
	    }
	  ++i;
	}
      return count == 1 ? find : NULL;
    }

    virtual void setNode(NodeType *node)
    {
      this->_node = node;
    }

    inline int getIdx() const
    {
      return this->_idx;
    }

  private:
    NodeType   	*_node;
    int		_idx;
  };
}

#endif // !CRC32_SELECTOR_HH_
