/**
 * \file ISelector.hh
 * \author Martial Bourquin
 * \version 1.0
 * \date 30/06/2012
 */

#ifndef ISELECTOR_HH_
# define ISELECTOR_HH_

# include <list>
# include <deque>
# include "Node.hh"
# include "BinSlay.hpp"

namespace BinSlay
{
  template<typename NodeType>
  class ISelector
  {
  public:
    virtual ~ISelector() {}
    virtual NodeType *operator()(typename BinSlay::bind_node<NodeType>::NODES_LIST const &setOfNodes) = 0;
    virtual void setNode(NodeType *_node) = 0;
    virtual int getIdx() const = 0;
  };
}

#endif // !ISELECTOR_HH_
