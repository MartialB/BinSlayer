/**
 * \file BasicBlock.hh
 * \author Martial Bourquin
 * \version 1.0
 * \date 30/06/2012
 */

#ifndef BASIC_BLOCK_HH_
# define BASIC_BLOCK_HH_

# include "BbNode.hh"
# include "IBinary.hh"

namespace BinSlay
{
  class BasicBlock
  {
  public:
    BasicBlock(
	     BinSlay::ReverseAPI::IBinary *bin,
	     BinSlay::BbNode const &bb_node,
	     BinSlay::ReverseAPI::Address addr
    );
    BasicBlock(BasicBlock const &o);
    ~BasicBlock();
    BasicBlock& operator=(BasicBlock const &);

    BinSlay::BbNode const &getNode() const;
    BinSlay::ReverseAPI::IBinary *getBinary();

  private:
    ReverseAPI::IBinary		*_binary;
    BinSlay::BbNode const      	&_node;
  };
}

#endif // !BASIC_BLOCK_HH_
