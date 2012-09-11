/**
 * \file CFG_Core.hh
 * \author Martial Bourquin
 * \version 1.0
 * \date 30/06/2012
 */

#ifndef CFG_CORE_HH_
# define CFG_CORE_HH_

# include "ACore.hpp"
# include "CFG.hh"
# include "BbNode.hh"

namespace BinSlay
{
  class CFG_Core : public ACore<BinSlay::BbNode>
  {
  public:
    CFG_Core(
	BinSlay::ReverseAPI::IBinary &bin_left,
	BinSlay::ReverseAPI::IBinary &bin_right,
	BinSlay::ReverseAPI::Address const fct_left,
	BinSlay::ReverseAPI::Address const fct_right
    );
    CFG_Core(CFG_Core const &o);
    CFG_Core& operator=(CFG_Core const &o);

  public:
    virtual ~CFG_Core();
    virtual bool load_graphs();
    virtual bool load_graph_left();
    virtual bool load_graph_right();

  private:
    BinSlay::ReverseAPI::Address	const _fct_left;
    BinSlay::ReverseAPI::Address	const _fct_right;
  };
}

#endif // !CFG_CORE_HH_
