/**
 * \file Bindiff.hh
 * \author Martial Bourquin
 * \version 1.0
 * \date 06/11/2012
 */

#ifndef __BINDIFF_HH__
# define __BINDIFF_HH__

namespace BinSlay {

  class idSelectors
  {
  public:
    enum idSelectors_e
      {
	NAME = 0,
	CRC32,
	CFG
      };
  };

  class idProperties
  {
  public:
    enum idProperties_e
      {
	UP = 0,
	DOWN
      };
  };

  enum class DiffingLevel
  {
    CALL_GRAPH = 0,
    FUNCTION,
    BASIC_BLOCK
  };

}

#endif // !__BINDIFF_HH__
