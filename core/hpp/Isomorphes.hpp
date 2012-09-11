/**
 * \file Isomorphes.hh
 * \author Martial Bourquin
 * \version 1.0
 * \date 30/06/2012
 */

#ifndef ISOMORPHES_HH_
# define ISOMORPHES_HH_

# include "Node.hh"
# include "BinSlay.hpp"

namespace BinSlay
{
  template<typename NodeType>
  class Isomorphes
  {
  public:
    Isomorphes(NodeType *left, NodeType *right, int level = 0)
      : _left(left), _right(right), _level(level)
    {
    }
    Isomorphes(Isomorphes const &o)
      : _left(o._left), _right(o._right), _level(o._level)
    {
    }
    ~Isomorphes() {}
    //    Isomorphes &operator=(Isomorphes const &);

    inline NodeType *getLeft() const
    {
      return this->_left;
    }
    inline NodeType *getRight() const
    {
      return this->_right;
    }
    inline int getLevel() const
    {
      return this->_level;
    }

  private:
    NodeType	*_left;
    NodeType	*_right;
    int const	_level;
  };
}

#endif // !ISOMORPHES_HH_
