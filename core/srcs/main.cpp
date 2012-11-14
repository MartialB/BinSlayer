/**
 * \file main.cpp
 * \author Martial Bourquin
 * \version 1.0
 * \date 30/06/2012
 */

#include <iostream>
#include <string>

#include "InternalsCore.hh"

static inline int usage(void)
{
  std::cerr << "Usage: ./bindiff_on_steroids file1 file2" << std::endl;
  return 1;
}

int main(int argc, char *argv[])
{
  if (argc < 3)
    return usage();

  // Create a instance of the InternalsCore object
  BinSlay::InternalsCore core;

  // Load internal libraries (BinaryHelper)
  if (!core.init()) {
    std::cerr << core.msg() << std::endl;
    return 1;
  }

  // Create a instance of the CG_Core object
  BinSlay::CG_Core *cg_core = core.createCG_Core(argv[1], argv[2]);
  if (core.status() != BinSlay::InternalsCore::Status::OK) {
    std::cerr << core.msg() << std::endl;
    return 1;
  }

  ////////////////////////////////////////////////////////////
  // BinDiff Algorithm

  // 1- Configuration of the Buindiff algorithm
  cg_core->add_Property(BinSlay::idProperties::UP);
  cg_core->add_Property(BinSlay::idProperties::DOWN);
  //cg_core->add_Selector(BinSlay::idSelectors::NAME);
  cg_core->add_Selector(BinSlay::idSelectors::CRC32);
  cg_core->add_Selector(BinSlay::idSelectors::CFG);
  // 2- Run Bindiff algorithm
  //cg_core->run_bindiff_algorithm(BinSlay::DiffingLevel::FUNCTION);
  cg_core->run_bindiff_algorithm(BinSlay::DiffingLevel::CALL_GRAPH);

  std::cout << "NB_ISO: " << std::dec << cg_core->get_nb_isomorphism_found()
  	    << " - NB_NODE_LEFT:" << cg_core->get_graph_left().getnbNode()
  	    << " - NB_NODE_RIGHT:" << cg_core->get_graph_right().getnbNode()
  	    << std::endl;

  ////////////////////////////////////////////////////////////
  // Ged Computation
  //cg_core->compute_ged(BinSlay::gedProperties::WITH_VALIDATOR);
  cg_core->compute_ged(BinSlay::gedProperties::NO_OPTIONS);
  std::cout << "Ged: " << cg_core->get_ged() << std::endl;

  // We want to count the number of isomorphisms found which have correctly matched
  // identical functions: we consider that two functions have successfully been matched
  // if they have the same name.
  unsigned int matched = 0;
  unsigned int cost = 0;
  unsigned int nb_substitutions = 0;
  unsigned int nb_insertions = 0;
  unsigned int nb_deletions = 0;
  auto *final_ep = cg_core->get_edit_path();
  for (auto it_iso = final_ep->begin(); it_iso != final_ep->end(); ++it_iso) {
    // if both are not null
    if ((*it_iso)->getLeft() && (*it_iso)->getRight())
      ++nb_substitutions;
    else if (!(*it_iso)->getLeft() && (*it_iso)->getRight())
      ++nb_insertions;
    if ((*it_iso)->getLeft() && !(*it_iso)->getRight())
      ++nb_deletions;
    cost += (*it_iso)->getLevel();
    if ((*it_iso)->getLeft() && (*it_iso)->getRight()) {
      if ((*it_iso)->getLeft()->getName() == (*it_iso)->getRight()->getName()) {
  	// Reference
  	++matched;
  	if ((*it_iso)->getLevel()) {
  	  // std::cout << (*it_iso)->getLeft()->getName()  << " - "
  	  // 	    << (*it_iso)->getRight()->getName()
  	  // 	    << " - COST = " << std::dec << (*it_iso)->getLevel()
  	  // 	    << std::endl;
  	}
      }	
    }
  }
  std::cout << "Number of elem in get iso list: " << std::dec << final_ep->size() << std::endl;
  std::cout << "Number of matched functions: " << std::dec << matched << std::endl;
  std::cout << "Cost: " << std::dec << cost << std::endl;

  std::cout << std::dec << "swap: " << nb_substitutions
  	    << " - add: " << nb_insertions
  	    << " - del:" << nb_deletions
  	    << std::endl;

  // std::cout << "Incorrect matches: " << std::dec
  // 	    << (final_ep->size() - matched - nb_insertions - nb_deletions)
  // 	    << std::endl;
  // {
  //   unsigned int matched = 0;
  //   auto const &final_mapping = cg_core->get_mapping();
  //   for (auto it_map = final_mapping.begin(); it_map != final_mapping.end(); ++it_map) {
  //     for (auto it_iso = (*it_map)->begin(); it_iso != (*it_map)->end(); ++it_iso) {
  // 	if ((*it_iso)->getLeft() && (*it_iso)->getRight()) {
  // 	  if ((*it_iso)->getLeft()->getName() == (*it_iso)->getRight()->getName()) {
  // 	    // Reference
  // 	  ++matched;
  // 	  // std::cout << (*it_iso)->getLeft()->getName()  << " - "
  // 	  // 	    << (*it_iso)->getRight()->getName()  << std::endl;
  // 	  }
  // 	}
  //     }
  //   }
  //   std::cout << "FINAL Number of elem in mapping: " << std::dec
  // 	      << cg_core->get_nb_isomorphism_found() << std::endl;
  //   std::cout << "FINAL Number of matched functions: " << std::dec << matched << std::endl;
  // }


  // /////////////////////////////////////////////////////////////////////////////////////////////////

  // unsigned int matched = 0;
  // unsigned int res = 0;
  // double treshold = 100.0;

  // while (1) {
  //   matched = 0;
  //   res = 0;
  //   unsigned int ret = 1;
  //   while ((ret = cg_core->re_run_bindiff()));
  //   // Print some information
  //   std::cout << "NB_ISO: " << std::dec << cg_core->get_nb_isomorphism_found()
  // 	      << " - NB_NODE_LEFT:" << cg_core->get_graph_left().getnbNode()
  // 	      << " - NB_NODE_RIGHT:" << cg_core->get_graph_right().getnbNode()
  // 	      << std::endl;

  //   ////////////////////////////////////////////////////////////
  //   // Ged Computation
  //   cg_core->compute_ged(BinSlay::gedProperties::WITH_VALIDATOR);
  //   //cg_core->compute_ged(BinSlay::gedProperties::NO_OPTIONS);
  //   std::cout << "Ged: " << cg_core->get_ged() << std::endl;

  //   // We want to count the number of isomorphisms found which have correctly matched
  //   // identical functions: we consider that two functions have successfully been matched
  //   // if they have the same name.
  //   unsigned int nb_substitutions = 0;
  //   unsigned int nb_insertions = 0;
  //   unsigned int nb_deletions = 0;
  //   auto *final_ep = cg_core->get_edit_path();
  //   for (auto it_iso = final_ep->begin(); it_iso != final_ep->end(); ++it_iso) {
  //     // if both are not null
  //     if (!( !(*it_iso)->getLeft() && !(*it_iso)->getRight() )) {
  // 	if ((*it_iso)->getLeft() && (*it_iso)->getRight())
  // 	  ++nb_substitutions;
  // 	else if (!(*it_iso)->getLeft() && (*it_iso)->getRight())
  // 	  ++nb_insertions;
  // 	if ((*it_iso)->getLeft() && !(*it_iso)->getRight())
  // 	  ++nb_deletions;
  //     }
  //     if ((*it_iso)->getLeft() && (*it_iso)->getRight()) {

  // 	// std::cout << (*it_iso)->getLeft()->getName()  << " - "
  // 	// 	  << (*it_iso)->getRight()->getName()  << std::endl;
  // 	// if ((*it_iso)->getLeft()->getName() == (*it_iso)->getRight()->getName()) {
  // 	//   // Reference
  // 	//   ++matched;
  // 	//   std::cout << (*it_iso)->getLeft()->getName()  << " - "
  // 	//   	    << (*it_iso)->getRight()->getName()  << std::endl;
  // 	//   // With symname => maximum possible result
  // 	//   cg_core->add_iso_couple_to_mapping(
  // 	//   	(*it_iso)->getLeft()->getAddr(),
  // 	//   	(*it_iso)->getRight()->getAddr()
  // 	//   );
  // 	// }	
  // 	// With BinDiff at the CFG level
  // 	double percentage = cg_core->run_bindiff_at_cfg_level(
  // 		(*it_iso)->getLeft()->getAddr(),
  // 		(*it_iso)->getRight()->getAddr()
  // 	);
  // 	if (percentage >= treshold) {//&&
  // 	    //	    ((*it_iso)->getLeft()->getName() == (*it_iso)->getRight()->getName())) {
  // 	  ++res;
  // 	  // std::cout << (*it_iso)->getLeft()->getName()  << " - " << std::hex
  // 	  // 	    << (*it_iso)->getLeft()->getAddr() << " : "
  // 	  // 	    << (*it_iso)->getRight()->getName() << " - "
  // 	  // 	    << (*it_iso)->getRight()->getAddr()
  // 	  // 	    << std::endl;
  // 	  cg_core->add_iso_couple_to_mapping(
  // 		(*it_iso)->getLeft()->getAddr(),
  // 		(*it_iso)->getRight()->getAddr()
  // 	    );
  // 	}
  // 	//	std::cout << std::dec << percentage << "%" << std::endl;
  //     }
  //   }
  //   std::cout << "Number of elem in get iso list: " << std::dec << final_ep->size() << std::endl;
  //   //    std::cout << "Maximum Number of matched functions: " << std::dec << matched << std::endl;
  //   std::cout << "Number of matched functions: " << std::dec << res << std::endl;
  //   treshold -= 5.0;
  //   if (res == 0)
  //     break;

  // }

  // std::cout << std::endl << std::endl << "================================================"
  // 	    << std::endl << std::endl;

  // {
  //   unsigned int matched = 0;
  //   auto *final_ep = cg_core->get_edit_path();
  //   for (auto it_iso = final_ep->begin(); it_iso != final_ep->end(); ++it_iso) {
  //     if ((*it_iso)->getLeft() && (*it_iso)->getRight()) {
  // 	if ((*it_iso)->getLeft()->getName() == (*it_iso)->getRight()->getName()) {
  // 	  ++matched;
  // 	  // std::cout << (*it_iso)->getLeft()->getName()  << " - "
  // 	  // 	    << (*it_iso)->getRight()->getName()  << std::endl;
  // 	}
  //     }
  //   }
  //   std::cout << "FINAL Number of elem in get iso list: " << std::dec << final_ep->size() << std::endl;
  //   std::cout << "FINAL Number of matched functions: " << std::dec << matched << std::endl;
  // }

  // {
  //   unsigned int matched = 0;
  //   auto const &final_mapping = cg_core->get_mapping();
  //   for (auto it_map = final_mapping.begin(); it_map != final_mapping.end(); ++it_map) {
  //     for (auto it_iso = (*it_map)->begin(); it_iso != (*it_map)->end(); ++it_iso) {
  // 	if ((*it_iso)->getLeft() && (*it_iso)->getRight()) {
  // 	  if ((*it_iso)->getLeft()->getName() == (*it_iso)->getRight()->getName()) {
  // 	    // Reference
  // 	  ++matched;
  // 	  // std::cout << (*it_iso)->getLeft()->getName()  << " - "
  // 	  // 	    << (*it_iso)->getRight()->getName()  << std::endl;
  // 	  }
  // 	}
  //     }
  //   }
  //   std::cout << "FINAL Number of elem in mapping: " << std::dec
  // 	      << cg_core->get_nb_isomorphism_found() << std::endl;
  //   std::cout << "FINAL Number of matched functions: " << std::dec << matched << std::endl;
  // }

  ////////////////////////////////////////////////////////////////////////////////////////////////////

  // BinSlay::CallGraph const &left =
  //   static_cast<BinSlay::CallGraph const &>(cg_core->get_graph_left());
  // BinSlay::CallGraph const &right =
  //   static_cast<BinSlay::CallGraph const &>(cg_core->get_graph_right());

  // // std::cout << "Nb basic blocks in left: " << std::dec << left.get_total_nb_of_basic_blocks()
  // // 	    << std::endl;
  // // std::cout << "Nb edges in left: " << left.get_total_nb_of_edges() << std::endl;

  // // std::cout << "Nb basic blocks in right: " << right.get_total_nb_of_basic_blocks() << std::endl;
  // // std::cout << "Nb edges in right: " << right.get_total_nb_of_edges() << std::endl;

  // std::cout << "Dissimilarity: "
  // 	    << ((double)cg_core->get_ged() /
  // 		(double)(left.get_total_nb_of_basic_blocks() + left.get_total_nb_of_edges() +
  // 			 right.get_total_nb_of_basic_blocks() + right.get_total_nb_of_edges()))
  // 	    << std::endl;

  // ret = 1;
  // while ((ret = cg_core->re_run_bindiff()));

  // // Print some information
  // std::cout << "NB_ISO: " << std::dec << cg_core->get_nb_isomorphism_found()
  // 	    << " - NB_NODE_LEFT:" << cg_core->get_graph_left().getnbNode()
  // 	    << " - NB_NODE_RIGHT:" << cg_core->get_graph_right().getnbNode()
  // 	    << std::endl;

  // ////////////////////////////////////////////////////////////
  // // Ged Computation
  // cg_core->compute_ged(BinSlay::gedProperties::WITH_VALIDATOR);
  // //cg_core->compute_ged(BinSlay::gedProperties::NO_OPTIONS);
  // std::cout << "Ged: " << cg_core->get_ged() << std::endl;

  // // We want to count the number of isomorphisms found which have correctly matched
  // // identical functions: we consider that two functions have successfully been matched
  // // if they have the same name.
  // matched = 0;
  // nb_substitutions = 0;
  // nb_insertions = 0;
  // nb_deletions = 0;
  // final_ep = cg_core->get_edit_path();
  // for (auto it_iso = final_ep->begin(); it_iso != final_ep->end(); ++it_iso) {
  //   // if both are not null
  //   if (!( !(*it_iso)->getLeft() && !(*it_iso)->getRight() )) {
  //     if ((*it_iso)->getLeft() && (*it_iso)->getRight())
  // 	++nb_substitutions;
  //     else if (!(*it_iso)->getLeft() && (*it_iso)->getRight())
  // 	++nb_insertions;
  //     if ((*it_iso)->getLeft() && !(*it_iso)->getRight())
  // 	++nb_deletions;
  //   }

  //   if ((*it_iso)->getLeft() && (*it_iso)->getRight()) {
  //     if ((*it_iso)->getLeft()->getName() == (*it_iso)->getRight()->getName()) {
  // 	++matched;
  // 	std::cout << (*it_iso)->getLeft()->getName()  << " - "
  // 		  << (*it_iso)->getRight()->getName()  << std::endl;
  // 	cg_core->add_iso_couple_to_mapping(
  // 		(*it_iso)->getLeft()->getAddr(),
  // 		(*it_iso)->getRight()->getAddr()
  // 	);
  //     }
  //   }
  // }

  // std::cerr << std::dec << "swap: " << nb_substitutions
  // 	    << " - add: " << nb_insertions
  // 	    << " - del:" << nb_deletions
  // 	    << std::endl;

  // std::cout << "Number of elem in get iso list: " << std::dec << final_ep->size() << std::endl;
  // std::cout << "Number of mactched functions: " << std::dec << matched << std::endl;
  return 0;
}
