/**
 * \file DownProperty.hh
 * \author Martial Bourquin
 * \version 1.0
 * \date 30/06/2012
 */

#ifndef DOWN_PROPERTY_HH_
# define DOWN_PROPERTY_HH_

# include <list>
# include "Bindiff.hpp"
# include "Node.hh"
# include "AGraph.hpp"
# include "IProperty.hpp"

namespace BinSlay
{
  template<typename NodeType>
  class DownProperty : public IProperty<NodeType>
  {
  public:
    DownProperty()
    {}
    DownProperty(NodeType *node) : _node(node) {}
    virtual ~DownProperty() {}

  public:
    virtual typename BinSlay::bind_node<NodeType>::NODES_LIST *operator()(
		typename BinSlay::bind_node<NodeType>::NODES_LIST const &setOfNodes)
    {
      return BinSlay::Bindiff<NodeType>::GetlistOfAllChildrens(setOfNodes, *this->_node);
    }

    virtual void setNode(NodeType *node)
    {
      this->_node = node;
    }


  private:
    NodeType *_node;
  };
}

#endif // !DOWN_PROPERTY_HH_
