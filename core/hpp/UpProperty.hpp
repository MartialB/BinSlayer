/**
 * \file UpProperty.hh
 * \author Martial Bourquin
 * \version 1.0
 * \date 30/06/2012
 */

#ifndef UP_PROPERTY_HH_
# define UP_PROPERTY_HH_

# include <list>
# include "Bindiff.hpp"
# include "Node.hh"
# include "IProperty.hpp"

namespace BinSlay
{
  template<typename NodeType>
  class UpProperty : public IProperty<NodeType>
  {
  public:
    UpProperty() {}
    UpProperty(NodeType *node) : _node(node) {}
    virtual ~UpProperty() {}

  public:
    virtual typename BinSlay::bind_node<NodeType>::NODES_LIST *operator()(
	typename BinSlay::bind_node<NodeType>::NODES_LIST const &setOfNodes)
    {
      return BinSlay::Bindiff<NodeType>::GetlistOfAllParents(setOfNodes, *this->_node);
    }

    virtual void setNode(NodeType *node)
    {
      this->_node = node;
    }

  private:
    NodeType	*_node;
  };
}

#endif // !UP_PROPERTY_HH_
