/**
 * \file CFG.hh
 * \author Martial Bourquin
 * \version 1.0
 * \date 30/06/2012
 */

#ifndef CFG_HH_
# define CFG_HH_

# include <string>
# include <vector>
# include <utility>
# include <fstream>
# include "BinSlay.hpp"
# include "BbNode.hh"
# include "Label.hh"
# include "BindiffSelector.hpp"
# include "Isomorphes.hpp"
# include "AGraph.hpp"
# include "IBinary.hh"

namespace BinSlay
{
  class CFG : public BinSlay::AGraph< BinSlay::BbNode >
  {
  public:
    CFG(BinSlay::ReverseAPI::IBinary *bin, BinSlay::ReverseAPI::Address addr);
    CFG(CFG const &o);
    virtual ~CFG();
    CFG& operator=(CFG const &o);

    BinSlay::BbNode *getBasicBlock(BinSlay::ReverseAPI::Address addr) const;
  };
}

#endif // !GFG_HH_
