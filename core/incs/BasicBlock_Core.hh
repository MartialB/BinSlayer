/**
 * \file BasicBlock_Core.hh
 * \author Martial Bourquin
 * \version 1.0
 * \date 30/06/2012
 */

#ifndef BASIC_BLOCK_CORE_HH_
# define BASIC_BLOCK_CORE_HH_

# include "BasicBlock.hh"

namespace BinSlay
{
  class BasicBlock_Core
  {
  public:
    BasicBlock_Core(
		    BinSlay::BasicBlock &_basic_block_left,
		    BinSlay::BasicBlock &_basic_block_right
    );
    ~BasicBlock_Core();
    BasicBlock_Core(BasicBlock_Core const &o);
    BasicBlock_Core& operator=(BasicBlock_Core const &o);

  public:
    BinSlay::BasicBlock &getLeft();
    BinSlay::BasicBlock &getRight();

  private:
    BinSlay::BasicBlock &_basic_block_left;
    BinSlay::BasicBlock &_basic_block_right;
  };
}

#endif // !BASIC_BLOCK_CORE_HH_
