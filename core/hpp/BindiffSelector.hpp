/**
 * \file BindiffSelector.hh
 * \author Martial Bourquin
 * \version 1.0
 * \date 30/06/2012
 */

#ifndef BINDIFF_SELECTOR_HH_
# define BINDIFF_SELECTOR_HH_

# include <list>
# include "BinSlay.hpp"
# include "Node.hh"
# include "ISelector.hpp"

namespace BinSlay
{
  template<typename NodeType>
  class BindiffSelector : public ISelector<NodeType>
  {
  public:
    BindiffSelector() {}
    BindiffSelector(NodeType *node) : _node(node) {}
    virtual ~BindiffSelector() {}

  public:
    inline NodeType *operator()(typename BinSlay::bind_node<NodeType>::NODES_LIST const &setOfNodes)
    {
      int count = 0;
      NodeType *find = NULL;
      _idx = -1;

      int i = 0;
      for (typename BinSlay::bind_node<NodeType>::NODES_LIST::const_iterator it =
	     setOfNodes.begin(); it != setOfNodes.end(); ++it)
	{
	  if ((*it)->getLabel() == this->_node->getLabel())
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

    inline void setNode(NodeType *node)
    {
      this->_node = node;      
    }

    inline int getIdx() const
    {
      return this->_idx;
    }

  private:
    NodeType	*_node;
    int		_idx;
  };
}

#endif // !BINDIFF_SELECTOR_HH_
