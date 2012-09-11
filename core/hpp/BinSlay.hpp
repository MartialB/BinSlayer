/**
 * \file BinSlay.hh
 * \author Martial Bourquin
 * \version 1.0
 * \date 30/06/2012
 */

#ifndef BINSLAY_HH_
# define BINSLAY_HH_

# include <deque>
# include <iostream>
# include <list>
# include <string>
# include <vector>

#define NODES_LIST(NodeType)	BinSlay::bind_node<NodeType>::NODES_LIST
#define ISOMORPHES_LIST(NodeType)	BinSlay::bind_node<NodeType>::ISOMORPHES_LIST
#define MAPPING(NodeType)		BinSlay::bind_node<NodeType>::MAPPING
#define SELECTORS(NodeType)		BinSlay::bind_node<NodeType>::SELECTORS
#define PROPERTIES(NodeType)       	BinSlay::bind_node<NodeType>::PROPERTIES

namespace BinSlay
{
  template <typename NodeType> class Isomorphes;
  template <typename NodeType> class ISelector;
  template <typename NodeType> class IProperty;

  template <typename NodeType>
  struct bind_node
  {
    typedef std::vector<NodeType *>				NODES_LIST;
    typedef std::list<Isomorphes<NodeType> *>			ISOMORPHES_LIST;
    typedef std::list<std::list<Isomorphes<NodeType> *> *>	MAPPING;
    typedef std::deque<ISelector<NodeType> *>			SELECTORS;
    typedef std::deque<IProperty<NodeType> *>			PROPERTIES;
  };
}

// template<typename NodeType>
// std::ostream& operator<<(std::ostream &os, NodeType const &node)
// {
//   return os << "addr=0x" << std::hex << node.getAddr() << std::dec << ", label=" << node.getLabel();
// }

template<typename NodeType>
std::ostream& operator<<(std::ostream &os,
			 typename BinSlay::bind_node<NodeType>::NODES_LIST const &nodes)
{
  for (typename BinSlay::bind_node<NodeType>::NODES_LIST::const_iterator it = nodes.begin();
       it != nodes.end(); ++it)
    {
      os << **it << ", ";
    }
  return os;
}

#endif // !BINSLAY_HH
