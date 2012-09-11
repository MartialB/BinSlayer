/**
 * \file Label.hh
 * \author Martial Bourquin
 * \version 1.0
 * \date 30/06/2012
 */

#ifndef LABEL_HH_
# define LABEL_HH_

# include <iostream>

# define ABS(x) ((x) >= 0 ? (x) : (-(x)))

namespace BinSlay
{
  class Label
  {
  public:
    Label();
    Label(int x, int y, int z);
    Label(Label const &o);
    ~Label();
    Label& operator=(Label const &o);

    void displayLabel() const;
    void setLabel(int x, int y, int z);
    bool isEqual(Label const &compareTo) const;
    inline bool operator==(Label const &compareTo) const
    {
      if (this->_x == compareTo._x &&
	  this->_y == compareTo._y &&
	  this->_z == compareTo._z)
	return true;
      return false;
    }
    unsigned sum() const;
    unsigned int get_relabeling_cost(Label const &compareTo) const;
    inline int getX() const
    {
      return _x;
    }
    inline int getY() const
    {
      return _y;
    }
    inline int getZ() const
    {
      return _z;
    }

  private:
    int	_x;
    int _y;
    int _z;
  };
}

std::ostream& operator<<(std::ostream &os, BinSlay::Label const &label);

#endif // !LABEL_HH_
