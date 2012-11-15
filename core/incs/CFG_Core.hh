/**
 * \file CFG_Core.hh
 * \author Martial Bourquin
 * \version 1.0
 * \date 30/06/2012
 */

#ifndef __CFG_CORE_HH__
# define __CFG_CORE_HH__

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
    virtual double run_bindiff_on_two_nodes(
	BinSlay::ReverseAPI::Address,
	BinSlay::ReverseAPI::Address
    );

  public:
    bool run_bindiff_algorithm(BinSlay::DiffingLevel level);
    double run_bindiff_on_two_basic_block(
	unsigned long addr_left,
	unsigned long addr_right
	// typename BinSlay::bind_node<BinSlay::InstrNode>::SELECTORS const &cfg_selectors,
	// typename BinSlay::bind_node<BinSlay::InstrNode>::PROPERTIES const &cfg_properties
    );
    bool diff();


  private:
    bool _run_bindiff_at_basic_block_level();
    bool _load_graph_left();
    bool _load_graph_right();

  private:
    BinSlay::ReverseAPI::Address const _fct_left;
    BinSlay::ReverseAPI::Address const _fct_right;
    CFG *_left;
    CFG *_right;
  };
}

#endif // !__CFG_CORE_HH__
