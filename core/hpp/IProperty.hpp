/**
 * \file IProperty.hh
 * \author Martial Bourquin
 * \version 1.0
 * \date 30/06/2012
 */

#ifndef IPROPERTY_HH_
# define IPROPERTY_HH_

# include <list>
# include <set>
# include "Node.hh"
# include "AGraph.hpp"

namespace BinSlay
{
  template<typename NodeType>
  class IProperty
  {
  public:
    virtual ~IProperty() {}
    virtual typename BinSlay::bind_node<NodeType>::NODES_LIST *operator()(
       	 typename BinSlay::bind_node<NodeType>::NODES_LIST const &setOfNodes) = 0;
    virtual void setNode(NodeType *_node) = 0;
  };

}

#endif // !IPROPERTY_HH_
