/**
 * \file CG_Core.hh
 * \author Martial Bourquin
 * \version 1.0
 * \date 30/06/2012
 */

#ifndef CG_CORE_HH_
# define CG_CORE_HH_

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
    virtual bool load_graph_left();
    virtual bool load_graph_right();
    virtual void add_Selector(int idSelector);
    virtual bool run_bindiff_algorithm(int level);
    virtual bool compute_ged(int level);

  private:
    void _run_bindiff_at_cfg_level();
  };
}

#endif // !CG_CORE_HH_
