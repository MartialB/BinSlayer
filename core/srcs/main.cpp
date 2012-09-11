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
  if (!core.init())
    {
      std::cerr << core.getErrorBuffer().str() << std::endl;
      return 1;
    }

  // Create a instance of the CG_Core object
  BinSlay::CG_Core *cg_core = core.createCG_Core(argv[1], argv[2]);

  cg_core->add_Property(0);
  cg_core->add_Property(1);
  cg_core->add_Selector(0); // name
  cg_core->add_Selector(1); // opcode
  cg_core->add_Selector(2); // bindiff
  cg_core->run_bindiff_algorithm(1);
  // cg_core->re_run_bindiff();
  // cg_core->re_run_bindiff();

  std::cout << "Nb isomorphism found: " << std::dec << cg_core->get_nb_isomorphism_found() << std::endl;
  std::cout << "Nb nodes in left: " << cg_core->get_graph_left().getnbNode() << std::endl;
  std::cout << "Nb nodes in right: " << cg_core->get_graph_right().getnbNode() << std::endl;
  std::cout << std::endl;

  std::cout << "Ged: " << cg_core->get_ged() << std::endl;
  BinSlay::bind_node<BinSlay::FctNode>::ISOMORPHES_LIST *final_ep = cg_core->get_edit_path();

  for (typename BinSlay::bind_node<BinSlay::FctNode>::ISOMORPHES_LIST::iterator it_iso =
  	 final_ep->begin(); it_iso != final_ep->end(); ++it_iso)
    {
      if ((*it_iso)->getLeft())
  	std::cout << "addr left: " << std::hex << (*it_iso)->getLeft()->getAddr();
      if ((*it_iso)->getRight())
  	std::cout << " - addr right: " << std::hex << (*it_iso)->getRight()->getAddr();
      std::cout << std::endl;
      // std::cout << "(*it_iso)->getLeft(): " << (*it_iso)->getLeft()
      // 		<< " - (*it_iso)->getRight(): " << (*it_iso)->getRight() << std::endl;
    }

  BinSlay::CallGraph const &left =
    static_cast<BinSlay::CallGraph const &>(cg_core->get_graph_left());
  BinSlay::CallGraph const &right =
    static_cast<BinSlay::CallGraph const &>(cg_core->get_graph_right());

  std::cout << "Nb basic blocks in left: " << std::dec << left.get_total_nb_of_basic_blocks()
	    << std::endl;
  std::cout << "Nb edges in left: " << left.get_total_nb_of_edges() << std::endl;

  std::cout << "Nb basic blocks in right: " << right.get_total_nb_of_basic_blocks() << std::endl;
  std::cout << "Nb edges in right: " << right.get_total_nb_of_edges() << std::endl;


  	       std::cout << "Dissimilarity: "
  	       << ((double)cg_core->get_ged() /
  		   (double)(left.get_total_nb_of_basic_blocks() + left.get_total_nb_of_edges() +
  			right.get_total_nb_of_basic_blocks() + right.get_total_nb_of_edges()))
  	       << std::endl;

	       
  return 0;
}
