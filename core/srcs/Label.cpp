/**
 * \file Label.cpp
 * \author Martial Bourquin
 * \version 1.0
 * \date 30/06/2012
 */

#include "Label.hh"

BinSlay::Label::Label()
{ 
}

BinSlay::Label::Label(int x, int y, int z)
  : _x(x), _y(y), _z(z)
{ 
}

BinSlay::Label::Label(Label const &o)
  : _x(o._x), _y(o._y), _z(o._z)
{
}

BinSlay::Label::~Label()
{
}

BinSlay::Label& BinSlay::Label::operator=(BinSlay::Label const &o)
{
  if (&o != this)
    {
      this->_x = o._x;
      this->_y = o._y;
      this->_z = o._z;
    }
  return *this;
}

void BinSlay::Label::setLabel(int x, int y, int z)
{
  this->_x = x;
  this->_y = y;
  this->_z = z;
}

bool BinSlay::Label::isEqual(BinSlay::Label const &compareTo) const
{
  if (this->_x == compareTo._x &&
      this->_y == compareTo._y &&
      this->_z == compareTo._z)
    return true;
  return false;
}

// bool BinSlay::Label::operator==(BinSlay::Label const &compareTo) const
// {
//   if (this->_x == compareTo._x &&
//       this->_y == compareTo._y &&
//       this->_z == compareTo._z)
//     return true;
//   return false;
// }

void BinSlay::Label::displayLabel() const
{
  std::cout << "(" << this->_x << "," << this->_y << ","
	    << this->_z << ")";
}

// int BinSlay::Label::getX() const
// {
//   return this->_x;
// }

// int BinSlay::Label::getY() const
// {
//   return this->_y;  
// }

// int BinSlay::Label::getZ() const
// {
//   return this->_z;  
// }

std::ostream& operator<<(std::ostream &os, BinSlay::Label const &label)
{
  return os << "(" << label.getX() << "," << label.getY() << "," << label.getZ() << ")";
}

unsigned int BinSlay::Label::get_relabeling_cost(BinSlay::Label const &compareTo) const
{
  // In the case of a substitution, the relabeling cost is only computed with the '_x' and '_y'
  // values which both deal with the internals structure of the node in our implementation. But
  // the '_z' value can not be used in the relabeling cost because it deals with edges information
  // and consequently, to improve the accuracy of the graph edit distance approximation, we need
  // to treat edges information separately.
  return ABS(this->_x - compareTo._x) + ABS(this->_y - compareTo._y);
}

unsigned int BinSlay::Label::sum() const
{
  return this->_x + this->_y;
}
