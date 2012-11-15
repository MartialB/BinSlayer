/**
 * \file CG_Core.hh
 * \author Martial Bourquin
 * \version 1.0
 * \date 30/06/2012
 */

#ifndef __CG_CORE_HH__
# define __CG_CORE_HH__

# include "BinDiff.hh"
# include "Ged.hh"
# include "ACore.hpp"
# include "CallGraph.hh"
# include "FctNode.hh"

namespace BinSlay
{
  class CG_Core : public ACore<BinSlay::FctNode>
  {
  public:
    CG_Core(BinSlay::ReverseAPI::IBinary &_bin_left,
	    BinSlay::ReverseAPI::IBinary &_bin_right);
    CG_Core(CG_Core const &o);
    CG_Core& operator=(CG_Core const &o);

  public:
    virtual ~CG_Core();
    virtual bool load_graphs();
    virtual double run_bindiff_on_two_nodes(
	BinSlay::ReverseAPI::Address,
	BinSlay::ReverseAPI::Address
    );
    virtual bool add_Selector(BinSlay::idSelectors::idSelectors_e idSelector);
    virtual bool run_bindiff_algorithm(BinSlay::DiffingLevel level);
    virtual bool compute_ged(BinSlay::gedProperties::gedProperties_e options);

  public:
    double run_bindiff_on_two_functions(
	unsigned long addr_left,
	unsigned long addr_right
    );
    bool diff();

  private:
    bool _run_bindiff_at_cfg_level();
    bool _load_graph_left();
    bool _load_graph_right();

  private:
    CallGraph	*_left;
    CallGraph	*_right;
    typename BinSlay::bind_node<BinSlay::BbNode>::SELECTORS	_cfg_selectors;
    typename BinSlay::bind_node<BinSlay::BbNode>::PROPERTIES	_cfg_properties;
  };
}

#endif // !__CG_CORE_HH__
