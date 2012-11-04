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
    std::cerr << core.getErrorBuffer().str() << std::endl;
    return 1;
  }

  // Create a instance of the CG_Core object
  BinSlay::CG_Core *cg_core = core.createCG_Core(argv[1], argv[2]);

  // Configuration of the Buindiff algorithm
  cg_core->add_Property(0);
  cg_core->add_Property(1);
  //  cg_core->add_Selector(0); // name
  cg_core->add_Selector(1); // opcode
  cg_core->add_Selector(2); // bindiff
  // Run Bindiff algorithm
  cg_core->run_bindiff_algorithm(1);

  // Re-Run it until no more isormorphism are found
  unsigned int ret = 1;
  while ((ret = cg_core->re_run_bindiff()));

  // Print some information
  std::cout << "NB_ISO: " << std::dec << cg_core->get_nb_isomorphism_found()
  	    << " - NB_LEFT:" << cg_core->get_graph_left().getnbNode()
  	    << " - NB_RIGHT:" << cg_core->get_graph_right().getnbNode()
  	    << std::endl;

  // Ged Computation
  cg_core->compute_ged(0);
  std::cout << "Ged: " << cg_core->get_ged() << std::endl;

  // We want to count the number of isomorphisms found which have correctly matched
  // identical functions: we consider that two functions have successfully been matched
  // if they have the same name.
  unsigned int matched = 0;
  auto *final_ep = cg_core->get_edit_path();
  for (auto it_iso = final_ep->begin(); it_iso != final_ep->end(); ++it_iso) {
    if ((*it_iso)->getLeft() && (*it_iso)->getRight()) {
      if ((*it_iso)->getLeft()->getName() == (*it_iso)->getRight()->getName()) {
	++matched;
	std::cout << (*it_iso)->getLeft()->getName()  << std::endl;
      }
    }
  }
  std::cout << "Number of elem in get iso list: " << std::dec << final_ep->size() << std::endl;
  std::cout << "Number of mactched functions: " << std::dec << matched << std::endl;

  BinSlay::CallGraph const &left =
    static_cast<BinSlay::CallGraph const &>(cg_core->get_graph_left());
  BinSlay::CallGraph const &right =
    static_cast<BinSlay::CallGraph const &>(cg_core->get_graph_right());

  // std::cout << "Nb basic blocks in left: " << std::dec << left.get_total_nb_of_basic_blocks()
  // 	    << std::endl;
  // std::cout << "Nb edges in left: " << left.get_total_nb_of_edges() << std::endl;

  // std::cout << "Nb basic blocks in right: " << right.get_total_nb_of_basic_blocks() << std::endl;
  // std::cout << "Nb edges in right: " << right.get_total_nb_of_edges() << std::endl;

  std::cout << "Dissimilarity: "
  	    << ((double)cg_core->get_ged() /
  		(double)(left.get_total_nb_of_basic_blocks() + left.get_total_nb_of_edges() +
  			 right.get_total_nb_of_basic_blocks() + right.get_total_nb_of_edges()))
  	    << std::endl;

	       
  return 0;
}
