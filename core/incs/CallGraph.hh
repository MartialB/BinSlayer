/**
 * \file CallGraph.hh
 * \author Martial Bourquin
 * \version 1.0
 * \date 30/06/2012
 */

#ifndef CALLGRAPH_HH_
# define CALLGRAPH_HH_

# include <string>
# include <vector>
# include <utility>
# include <fstream>
# include "AGraph.hpp"
# include "IBinary.hh"
# include "FctNode.hh"

namespace BinSlay
{
  class CallGraph : public AGraph<BinSlay::FctNode>
  {
  public:
    CallGraph(BinSlay::ReverseAPI::IBinary *bin);
    CallGraph(CallGraph const &o);
    virtual ~CallGraph();
    CallGraph& operator=(CallGraph const &o);

    unsigned int get_total_nb_of_basic_blocks() const;
    unsigned int get_total_nb_of_edges() const;
  };
}

#endif // !CALLGRAPH_HH_
