/**
 * \file SymNameSelector.hh
 * \author Martial Bourquin
 * \version 1.0
 * \date 30/06/2012
 */

#ifndef SYMNAME_SELECTOR_HH_
# define SYMNAME_SELECTOR_HH_

# include <list>
# include "BinSlay.hpp"
# include "FctNode.hh"
# include "ISelector.hpp"

namespace BinSlay
{
  //  template<typename NodeType>
  class SymNameSelector : public ISelector<BinSlay::FctNode>
  {
  public:
    SymNameSelector() {}
    SymNameSelector(BinSlay::FctNode *node) : _node(node) {}
    virtual ~SymNameSelector() {}

  public:
    virtual BinSlay::FctNode *operator()(
	    typename BinSlay::bind_node<BinSlay::FctNode>::NODES_LIST const &setOfNodes)
    {
      int count = 0;
      BinSlay::FctNode *find = NULL;
      _idx = -1;
      int i = 0;

      if (this->_node->getName() == "")
	return NULL;

      for (typename BinSlay::bind_node<BinSlay::FctNode>::NODES_LIST::const_iterator it =
	     setOfNodes.begin(); it != setOfNodes.end(); ++it)
	{
	  if ((*it)->getName() != "" &&
	      this->_node->getName() != "" &&
	      (*it)->getName() == this->_node->getName())
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
    
    inline void setNode(BinSlay::FctNode *node)
    {
      this->_node = node;
    }

    inline int getIdx() const
    {
      return this->_idx;
    }

  private:
    BinSlay::FctNode	*_node;
    int			_idx;
  };
}

#endif // !SYMNAME_SELECTOR_HH_
