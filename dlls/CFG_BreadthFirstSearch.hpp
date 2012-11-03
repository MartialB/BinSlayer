#ifndef __CFG_BREADTH_FIRST_SEARCH_HPP__
# define __CFG_BREADTH_FIRST_SEARCH_HPP__

# include "DyninstBin.hh"

namespace BinSlay {
  namespace ReverseAPI {

    template<class Action>
    class CFG_BreadthFirstSearch
    {
    public:
      enum Order
	{
	  TOP_DOWN = 0,
	  DOWN_TOP
	};

    public:
      CFG_BreadthFirstSearch(
		BinSlay::ReverseAPI::DyninstBin const &bin,
		Dyninst::ParseAPI::Function &f,
		BinSlay::ReverseAPI::CFG &cfg
      );

      ~CFG_BreadthFirstSearch();

      void operator()(
		std::list<Dyninst::ParseAPI::Block *> &blocks,
		Order order
      );

    private:
      DyninstBin			const &_bin;
      Dyninst::ParseAPI::Function      	&_f;
      unsigned int			_level_of_depth;
      Action				*_action;
      std::map<Dyninst::Address, bool> 	_visited;
    };

    template<typename Action>
    CFG_BreadthFirstSearch<Action>::CFG_BreadthFirstSearch(
		BinSlay::ReverseAPI::DyninstBin const &bin,
		Dyninst::ParseAPI::Function &f,
		BinSlay::ReverseAPI::CFG &cfg
    )
      : _bin(bin), 
	_f(f),
	_level_of_depth(0),
	_action(new Action(bin, cfg))
    {
    }

    template<typename Action>
    CFG_BreadthFirstSearch<Action>::~CFG_BreadthFirstSearch()
    {
      delete _action;
    }

    template<typename Action>
    void CFG_BreadthFirstSearch<Action>::operator()(
		std::list<Dyninst::ParseAPI::Block *> &blocks,
		Order order
    )
    {
      std::list<Dyninst::ParseAPI::Block *> next_set;

      for (auto it_block = blocks.begin(); it_block != blocks.end(); ++it_block) {
	// Don’t revisit blocks in shared code (otherwise infinite loop)
	if (_visited.find((*it_block)->start()) != _visited.end())
	  continue;
	_visited[(*it_block)->start()] = true;
	// Get next_set, either parent or chil blocks
	if (order == TOP_DOWN) {
	  for(auto it_edge = (*it_block)->targets().begin(); it_edge !=
		(*it_block)->targets().end(); ++it_edge) {
	    // Children blocks
	    if ((*it_edge)->type() != Dyninst::ParseAPI::CALL &&
		(*it_edge)->type() != Dyninst::ParseAPI::RET) {
	      if (_f.contains((*it_edge)->trg())) {
		next_set.push_front((*it_edge)->trg());
	      }
	    }
	  }
	} else {
	  for(auto it_edge = (*it_block)->sources().begin(); it_edge !=
		(*it_block)->sources().end(); ++it_edge) {
	    // Parent blocks
	    if ((*it_edge)->type() != Dyninst::ParseAPI::CALL &&
		(*it_edge)->type() != Dyninst::ParseAPI::RET) {
	      if (_f.contains((*it_edge)->src())) {
		next_set.push_front((*it_edge)->src());
	      }
	    }
	  }
	}
	// Functor : the action we want to do on each basic block
	(*_action)(**it_block, this->_level_of_depth);
      }
      // If the children list is empty, we have reached the end of the function
      if (!next_set.size())
	return;
      // Recursion with the next_set of basic block in the given order
      ++this->_level_of_depth;
      (*this)(next_set, order);
    }
 
  }
}

#endif // !__CFG_BREADTH_FIRST_SEARCH_HPP__
