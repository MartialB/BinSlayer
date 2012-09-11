/**
 * \file Function.hh
 * \author Martial Bourquin
 * \version 1.0
 * \date 30/06/2012
 */

#ifndef FUNCTION_HH_
# define FUNCTION_HH_

# include "FctNode.hh"
# include "CFG.hh"
# include "IBinary.hh"

namespace BinSlay
{
  class Function : public BinSlay::CFG
  {
  public:
    Function(
	     BinSlay::ReverseAPI::IBinary *bin,
	     BinSlay::FctNode const &fct_node,
	     BinSlay::ReverseAPI::Address addr
    );
    Function(Function const &o);
    ~Function();
    Function& operator=(Function const &);

    BinSlay::FctNode const &getNode() const;
    BinSlay::ReverseAPI::IBinary *getBinary();

  private:
    ReverseAPI::IBinary		*_binary;
    BinSlay::FctNode const	&_node;
  };
}

#endif // !FUNCTION_HH_
