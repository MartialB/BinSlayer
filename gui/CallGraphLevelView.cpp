#include "CallGraphLevelView.h"
#include "CFGLevelView.h"
#include "ShowFunctionView.h"
#include "BinDiff.hh"

typedef BinSlay::FctNode NodeType;

BinSlay::Gui::CallGraphLevelView::CallGraphLevelView(
      BinSlay::InternalsCore &int_core, BinSlay::CG_Core *cg_core)
  : _internals_core(int_core),
    _core(cg_core)
{
  // Use of a vertical layout
  QVBoxLayout *mainLayout = new QVBoxLayout;

  // // Open files
  // createOpenGroupBox();
  // mainLayout->addWidget(openGroupBox);

  // Selectors choice
  createConfigGroupBox();
  mainLayout->addWidget(configGroupBox);

  // // Euclidian distance selection
  // createEuclidianDistanceGroupBox();
  // mainLayout->addWidget(euclidianDistanceGroupBox);

  // Run buttons
  createBindiffGroupBox();
  mainLayout->addWidget(bindiffGroupBox);

  // nbFct1Label = new QLabel(tr("Number of functions found in graph left: "));
  // nbFct2Label = new QLabel(tr("Number of functions found in graph right: "));;
  // nbIsoFoundLabel = new QLabel(tr("Number of couples of isomorphism found: "));
  // percentageLabel = new QLabel(tr("Percentage of matching nodes: "));
  // mainLayout->addWidget(nbFct1Label);
  // mainLayout->addWidget(nbFct2Label);
  // mainLayout->addWidget(nbIsoFoundLabel);
  // mainLayout->addWidget(percentageLabel);

  // Ged
  createGedGroupBox();
  mainLayout->addWidget(gedGroupBox);

  // Results
  createResGroupBox();
  mainLayout->addWidget(resGroupBox);

  setLayout(mainLayout);
}

void BinSlay::Gui::CallGraphLevelView::set_CG_Core(BinSlay::CG_Core *core)
{
  this->_core = core;
}

void BinSlay::Gui::CallGraphLevelView::createBindiffGroupBox()
{
  bindiffGroupBox = new QGroupBox(tr("Bindiff algorithm"));
  QHBoxLayout *hbox = new QHBoxLayout;

  runButton = new QPushButton("Run");
  resetButton = new QPushButton("Reset");
  reRunButton = new QPushButton("Re-Run");

  connect(runButton, SIGNAL(clicked()), this, SLOT(run_bindiff()));
  connect(resetButton, SIGNAL(clicked()), this, SLOT(reset_bindiff()));
  connect(reRunButton, SIGNAL(clicked()), this, SLOT(re_run_bindiff()));

  bindiffLineEdit = new QLineEdit;
  bindiffLineEdit->setReadOnly(true);

  hbox->addWidget(runButton);
  hbox->addWidget(reRunButton);
  hbox->addWidget(resetButton);
  hbox->addWidget(bindiffLineEdit);
  bindiffGroupBox->setLayout(hbox);
}

void BinSlay::Gui::CallGraphLevelView::createResGroupBox()
{
  resGroupBox = new QGroupBox(tr("Results"));
  QVBoxLayout *vbox = new QVBoxLayout;

  tabResults = new QTabWidget;

  createIsoListView();
  createEditPathView();
  createMatchedFunctionsView();
  createUnMatchedFunctionsView();

  tabResults->addTab(isoListView, tr("Matched functions"));
  tabResults->addTab(editPathView, tr("Edit path"));
  tabResults->addTab(matchedFunctionsView, tr("UnMatched functions bin1"));
  tabResults->addTab(unmatchedFunctionsView, tr("Unmatched functions bin2"));
  vbox->addWidget(tabResults);
  resGroupBox->setLayout(vbox);
}

void BinSlay::Gui::CallGraphLevelView::createIsoListView()
{
  QStringList labels;
  labels << tr("RecLevel") << tr("Address 1") << tr("Name") << tr("Address 2") << tr("Name");
  isoListView = new QTableWidget(0, 5);

  isoListView->setHorizontalHeaderLabels(labels);
  isoListView->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
  isoListView->horizontalHeader()->setCascadingSectionResizes(true);
  isoListView->horizontalHeader()->setStretchLastSection(true);
  isoListView->verticalHeader()->hide();
  isoListView->setShowGrid(false);

  // Context menu
  isoListView->setContextMenuPolicy(Qt::ActionsContextMenu);
  QAction *showFunctionNode  = new QAction(tr("&Show function node"), isoListView);
  QAction *diffFunctions  = new QAction(tr("&Diff functions"), isoListView);
  connect(showFunctionNode, SIGNAL(triggered()), this, SLOT(doShowFunction_fromIsoList()));
  connect(diffFunctions, SIGNAL(triggered()), this, SLOT(doDiffFunctions_fromIsoList()));
  isoListView->addAction(showFunctionNode);
  isoListView->addAction(diffFunctions);
}

void BinSlay::Gui::CallGraphLevelView::createEditPathView()
{
  QStringList labels;
  labels << tr("Address 1") << tr("Address 2") << tr("Transformation")
	 << tr("Cost");
  editPathView = new QTableWidget(0, 4);

  editPathView->setHorizontalHeaderLabels(labels);
  editPathView->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
  editPathView->horizontalHeader()->setCascadingSectionResizes(true);
  editPathView->horizontalHeader()->setStretchLastSection(true);
  editPathView->verticalHeader()->hide();
  editPathView->setShowGrid(false);

  // Context menu
  editPathView->setContextMenuPolicy(Qt::ActionsContextMenu);
  QAction *showFunctionNode  = new QAction(tr("&Show function node"), editPathView);
  QAction *diffFunctions  = new QAction(tr("&Diff functions"), editPathView);
  QAction *addMatchedFunctions  = new QAction(tr("&Add to matched list"), editPathView);
  connect(showFunctionNode, SIGNAL(triggered()), this, SLOT(doShowFunction_fromEditPath()));
  connect(diffFunctions, SIGNAL(triggered()), this, SLOT(doDiffFunctions_fromEditPath()));
  connect(addMatchedFunctions, SIGNAL(triggered()), this, SLOT(doAddToMatchedFunctions()));
  editPathView->addAction(showFunctionNode);
  editPathView->addAction(diffFunctions);
  editPathView->addAction(addMatchedFunctions);
}

void BinSlay::Gui::CallGraphLevelView::createMatchedFunctionsView()
{
  QStringList labels;
  labels << tr("Address 1") << tr("Name");
  matchedFunctionsView = new QTableWidget(0, 2);

  matchedFunctionsView->setHorizontalHeaderLabels(labels);
  matchedFunctionsView->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
  matchedFunctionsView->horizontalHeader()->setCascadingSectionResizes(true);
  matchedFunctionsView->horizontalHeader()->setStretchLastSection(true);
  matchedFunctionsView->verticalHeader()->hide();
  matchedFunctionsView->setShowGrid(false);

  // Context menu
  matchedFunctionsView->setContextMenuPolicy(Qt::ActionsContextMenu);
  QAction *showFunctionNode  = new QAction(tr("&Show function node"), matchedFunctionsView);
  connect(showFunctionNode, SIGNAL(triggered()), this, SLOT(doShowFunction_fromMatched()));
  matchedFunctionsView->addAction(showFunctionNode);
}

void BinSlay::Gui::CallGraphLevelView::createUnMatchedFunctionsView()
{
  QStringList labels;
  labels << tr("Address 2") << tr("Name");
  unmatchedFunctionsView = new QTableWidget(0, 2);

  unmatchedFunctionsView->setHorizontalHeaderLabels(labels);
  unmatchedFunctionsView->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
  unmatchedFunctionsView->horizontalHeader()->setCascadingSectionResizes(true);
  unmatchedFunctionsView->horizontalHeader()->setStretchLastSection(true);
  unmatchedFunctionsView->verticalHeader()->hide();
  unmatchedFunctionsView->setShowGrid(false);

  // Context menu
  unmatchedFunctionsView->setContextMenuPolicy(Qt::ActionsContextMenu);
  QAction *showFunctionNode  = new QAction(tr("&Show function node"), matchedFunctionsView);
  connect(showFunctionNode, SIGNAL(triggered()), this, SLOT(doShowFunction_fromUnMatched()));
  unmatchedFunctionsView->addAction(showFunctionNode);
}

void BinSlay::Gui::CallGraphLevelView::createConfigGroupBox()
{
  configGroupBox = new QGroupBox(tr("Configuration"));
  QHBoxLayout *hbox = new QHBoxLayout;

  // Selectors
  QGroupBox *selectorsGroupBox = new QGroupBox(tr("Selectors"));
  QVBoxLayout *vbox = new QVBoxLayout;

  checkBoxSelectors[cfg] = new QCheckBox(tr("&Control Flow-Graph"));
  checkBoxSelectors[crc32] = new QCheckBox(tr("&Opcode CRC32"));
  checkBoxSelectors[name] = new QCheckBox(tr("&Symbolic name"));
  checkBoxSelectors[cfg]->setChecked(false);
  checkBoxSelectors[crc32]->setChecked(false);
  checkBoxSelectors[name]->setChecked(false);

  connect(checkBoxSelectors[cfg], SIGNAL(toggled(bool)), this, SLOT(doEditBindiffSelector(bool)));
  connect(checkBoxSelectors[crc32], SIGNAL(toggled(bool)), this, SLOT(doEditCrc32Selectors(bool)));
  connect(checkBoxSelectors[name], SIGNAL(toggled(bool)), this, SLOT(doEditSymNameSelectors(bool)));

  vbox->addWidget(checkBoxSelectors[cfg]);
  vbox->addWidget(checkBoxSelectors[crc32]);
  vbox->addWidget(checkBoxSelectors[name]);
  selectorsGroupBox->setLayout(vbox);

  // Properties
  QGroupBox *propertiesGroupBox = new QGroupBox(tr("Properties"));
  QVBoxLayout *vbox2 = new QVBoxLayout;

  checkBoxProperties[up] = new QCheckBox(tr("&Up"));
  checkBoxProperties[down] = new QCheckBox(tr("&Down"));
  checkBoxProperties[up]->setChecked(false);
  checkBoxProperties[down]->setChecked(false);

  connect(checkBoxProperties[up], SIGNAL(toggled(bool)), this, SLOT(doEditUpProperty(bool)));
  connect(checkBoxProperties[down], SIGNAL(toggled(bool)), this, SLOT(doEditDownProperty(bool)));

  vbox2->addWidget(checkBoxProperties[up]);
  vbox2->addWidget(checkBoxProperties[down]);
  propertiesGroupBox->setLayout(vbox2);

  // Levels
  QGroupBox *levelsGroupBox = new QGroupBox(tr("Levels"));
  QVBoxLayout *vbox3 = new QVBoxLayout;

  checkBoxLevels[fct] = new QCheckBox(tr("&call-graph"));
  checkBoxLevels[fcfg] = new QCheckBox(tr("&control flow graph"));
  //  checkBoxLevels[bb] = new QCheckBox(tr("&basic block"));
  checkBoxLevels[fct]->setChecked(false);
  checkBoxLevels[fcfg]->setChecked(false);
  //  checkBoxLevels[bb]->setChecked(false);

  connect(checkBoxLevels[fct], SIGNAL(toggled(bool)), this, SLOT(doEditCgLevel(bool)));
  connect(checkBoxLevels[fcfg], SIGNAL(toggled(bool)), this, SLOT(doEditCfgLevel(bool)));
  //  connect(checkBoxLevels[bb], SIGNAL(toggled(bool)), this, SLOT(doEditBbLevel(bool)));

  vbox3->addWidget(checkBoxLevels[fct]);
  vbox3->addWidget(checkBoxLevels[fcfg]);
  //  vbox3->addWidget(checkBoxLevels[bb]);
  levelsGroupBox->setLayout(vbox3);

  hbox->addWidget(selectorsGroupBox);
  hbox->addWidget(propertiesGroupBox);
  hbox->addWidget(levelsGroupBox);
  configGroupBox->setLayout(hbox);
}

void BinSlay::Gui::CallGraphLevelView::createEuclidianDistanceGroupBox()
{
  euclidianDistanceGroupBox = new QGroupBox(tr("Eucliadian distance"));
  QGridLayout *layout = new QGridLayout;

  eucDistLabel = new QLabel(tr("Choose a distance:"));
  eucDistSpinBox = new QSpinBox;
  eucDistSpinBox->setRange(0, 4);
  eucDistSpinBox->setSingleStep(1);  
  layout->addWidget(eucDistLabel, 0, 0);
  layout->addWidget(eucDistSpinBox, 0, 1);
  euclidianDistanceGroupBox->setLayout(layout);
}

void BinSlay::Gui::CallGraphLevelView::createGedGroupBox()
{
  gedGroupBox = new QGroupBox(tr("Graph Edit Distance computation"));
  QHBoxLayout *hbox = new QHBoxLayout;

  gedRunButton = new QPushButton("Compute");
  gedResetButton = new QPushButton("Reset");

  gedLineEdit = new QLineEdit;
  gedLineEdit->setReadOnly(true);
  hbox->addWidget(gedRunButton);
  hbox->addWidget(gedResetButton);
  hbox->addWidget(gedLineEdit);

  connect(gedRunButton, SIGNAL(clicked()), this, SLOT(compute_ged()));
  connect(gedResetButton, SIGNAL(clicked()), this, SLOT(reset_ged()));

  gedGroupBox->setLayout(hbox);
}

void BinSlay::Gui::CallGraphLevelView::reset()
{
  // Uncheck all QCheckBoxs
  checkBoxSelectors[cfg]->setChecked(false);
  checkBoxSelectors[crc32]->setChecked(false);
  checkBoxSelectors[name]->setChecked(false);
  checkBoxProperties[up]->setChecked(false);
  checkBoxProperties[down]->setChecked(false);
  checkBoxLevels[fct]->setChecked(false);
  checkBoxLevels[fcfg]->setChecked(false);
  //checkBoxLevels[bb]->setChecked(false);

  // Clear all results tabs
  reset_bindiff();
  reset_ged();

  // Reset all QLineEdits
  gedLineEdit->setText(tr(""));
  bindiffLineEdit->setText(tr(""));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>

void BinSlay::Gui::CallGraphLevelView::doEditUpProperty(bool state)
{
  if (!_core)
    return;
  if (state)
    _core->add_Property(BinSlay::idProperties::UP);
  else
    _core->remove_Property(BinSlay::idProperties::UP);
}

void BinSlay::Gui::CallGraphLevelView::doEditDownProperty(bool state)
{
  if (!_core)
    return;
  if (state)
    _core->add_Property(BinSlay::idProperties::DOWN);
  else
    _core->remove_Property(BinSlay::idProperties::DOWN);
}

void BinSlay::Gui::CallGraphLevelView::doEditCgLevel(bool)
{
}

void BinSlay::Gui::CallGraphLevelView::doEditCfgLevel(bool)
{ 
}

// void BinSlay::Gui::CallGraphLevelView::doEditBbLevel(bool state)
// {
  
// }

void BinSlay::Gui::CallGraphLevelView::cg_bin1_to_dot_format()
{
  if (_core && lineEdits[0]->text().toStdString() != "")
    _core->create_dot_file_left(lineEdits[0]->text().toStdString());
}

void BinSlay::Gui::CallGraphLevelView::cg_bin2_to_dot_format()
{
  if (_core && lineEdits[1]->text().toStdString() != "")
  _core->create_dot_file_right(lineEdits[1]->text().toStdString());
}

void BinSlay::Gui::CallGraphLevelView::doEditBindiffSelector(bool state)
{
  if (!_core)
    return ;
  if (state)
    _core->add_Selector(BinSlay::idSelectors::CFG);
  else
    _core->remove_Selector(BinSlay::idSelectors::CFG);
}

void BinSlay::Gui::CallGraphLevelView::doEditCrc32Selectors(bool state)
{
  if (!_core)
    return ;
  if (state)
    _core->add_Selector(BinSlay::idSelectors::CRC32);
  else
    _core->remove_Selector(BinSlay::idSelectors::CRC32);
}

void BinSlay::Gui::CallGraphLevelView::doEditSymNameSelectors(bool state)
{
  if (!_core)
    return ;
  if (state)
    _core->add_Selector(BinSlay::idSelectors::NAME);
  else
    _core->remove_Selector(BinSlay::idSelectors::NAME);
}

void BinSlay::Gui::CallGraphLevelView::doDiffFunctions_fromIsoList()
{
  diffFunctions(ISO_LIST_VIEW);
}

void BinSlay::Gui::CallGraphLevelView::doDiffFunctions_fromEditPath()
{
  diffFunctions(EDIT_PATH_VIEW);
}

void BinSlay::Gui::CallGraphLevelView::doDiffFunctions_fromMatched()
{
  diffFunctions(MATCHED_VIEW);
}

void BinSlay::Gui::CallGraphLevelView::doDiffFunctions_fromUnMatched()
{
  diffFunctions(UNMATCHED_VIEW);
}

void BinSlay::Gui::CallGraphLevelView::diffFunctions(view_type which_one)
{
  // Check if a cg_core is loaded into the view
  if (!_core)
    return ;

  // Find the QTableWidget view which emitted the trigger signal
  QTableWidget *table_view = NULL;
  switch (which_one)
    {
    case ISO_LIST_VIEW:
      table_view = isoListView;
      break;

    case EDIT_PATH_VIEW:
      table_view = editPathView;
      break;

    case MATCHED_VIEW:
      table_view = matchedFunctionsView;
      break;

    case UNMATCHED_VIEW:
      table_view = unmatchedFunctionsView;
      break;

    default:
      return;
    }

  // Check if at least one item is selected
  QList<QTableWidgetItem *> selected_items = table_view->selectedItems();
  if (!selected_items.size())
    return ;

  std::stringstream convertor;
  BinSlay::ReverseAPI::Address fct1 = 0;
  BinSlay::ReverseAPI::Address fct2 = 0;

  // Check if number of selected items equal number of coluns in the view
  if (selected_items.size() != table_view->columnCount())
    return ;

  // Check if all the row has been selected and if it is the case, retrieve the addr of
  // the two functions to compare
  int column = 0;
  int row = (*selected_items.begin())->row();
  for (QList<QTableWidgetItem *>::iterator it = selected_items.begin();
       it != selected_items.end(); ++it)
    {
      // check if all selected items are on the same line
      if ((*it)->column() != column || (*it)->row() != row)
	return ;

      // Get the widget associated with the header in the column where we found
      // the current selected widget
      QTableWidgetItem *col_header = NULL;
      col_header = table_view->horizontalHeaderItem((*it)->column());

      // Check if it is an "Address" column
      if (col_header->text() == "Address 1" || col_header->text() == "Address 2")
	{
	  convertor << (*it)->text().toStdString();
	  if (!fct1) convertor >> std::hex >> fct1;
	  else if (!fct2) convertor >> std::hex >> fct2;
	  convertor.str(std::string());
	  convertor.clear();
	}
      ++column;
    }

  // Create an new instance of a cfg_core
  BinSlay::CFG_Core *cfg_core = _internals_core.createCFG_Core(_core, fct1, fct2);

  // Create a new instance of a CFGLevelView with the new cerated cfg_core
  BinSlay::Gui::CFGLevelView *cfg_diff_view =
    new BinSlay::Gui::CFGLevelView(_internals_core, *cfg_core);

  // Make title of this subwindow
  convertor << "Diff functions: @" << std::hex << fct1 << " VS @" << std::hex << fct2;
  cfg_diff_view->setWindowTitle(convertor.str().c_str());

  // Show the window
  cfg_diff_view->show();
}

void BinSlay::Gui::CallGraphLevelView::doShowFunction_fromIsoList()
{
  showFunction(ISO_LIST_VIEW);
}

void BinSlay::Gui::CallGraphLevelView::doShowFunction_fromEditPath()
{
  showFunction(EDIT_PATH_VIEW);
}

void BinSlay::Gui::CallGraphLevelView::doShowFunction_fromMatched()
{
  showFunction(MATCHED_VIEW);
}

void BinSlay::Gui::CallGraphLevelView::doShowFunction_fromUnMatched()
{
  showFunction(UNMATCHED_VIEW);
}

void BinSlay::Gui::CallGraphLevelView::showFunction(view_type which_one)
{
  if (!_core)
    return ;

  std::stringstream convertor;

  // Find the QTableWidget view which emitted the trigger signal
  QTableWidget *table_view = NULL;
  switch (which_one)
    {
    case ISO_LIST_VIEW:
      table_view = isoListView;
      break;

    case EDIT_PATH_VIEW:
      table_view = editPathView;
      break;

    case MATCHED_VIEW:
      table_view = matchedFunctionsView;
      break;

    case UNMATCHED_VIEW:
      table_view = unmatchedFunctionsView;
      break;

    default:
      return;
    }

  // Retrieve the address of the desired function and determine in which binary this function
  // resides
  BinSlay::ReverseAPI::Address fctAddr = 0;
  int which_binary = 0;
  QList<QTableWidgetItem *> selected_items = table_view->selectedItems();

  // Check if the number of selected items is equal to one or two
  if (selected_items.size() < 1 && selected_items.size() > 2)
    return ;

  // Check if a function has been selected:
  //	- either one cell, addr OR name
  //	- either both addr AND name cells
  QTableWidgetItem *addr = *selected_items.begin();

  // Get the widget associated with the header in the column where we found
  // the current selected widget
  QTableWidgetItem *col_header = NULL;
  col_header = table_view->horizontalHeaderItem(addr->column());

  // Check if it is an "Address" column and find in which binary this funciton resides
  if (col_header->text() == "Address 1")
    {
      which_binary = 0;
      convertor << addr->text().toStdString();
      convertor >> std::hex >> fctAddr;
      convertor.str(std::string());
      convertor.clear(); 
    }
  else if (col_header->text() == "Address 2")
    {
      which_binary = 1;
      convertor << addr->text().toStdString();
      convertor >> std::hex >> fctAddr;
      convertor.str(std::string());
      convertor.clear();
    }
  else
    // This is not a valid address !
    return ;

  //  std::cout << "Addr: " << std::hex << fctAddr << " - which_binary: " << which_binary << std::endl;

  // Get a const reference on the function
  BinSlay::Function *function = _internals_core.create_function(_core, fctAddr, which_binary);
  //  std::cout << function << std::endl;

  // Create a new instance of a ShowFunctionView with the selected function node
  BinSlay::Gui::ShowFunctionView *cfg_show_view =
    new BinSlay::Gui::ShowFunctionView(_internals_core, function);

  // Set the title
  convertor << "Show function: @" << std::hex << fctAddr;
  cfg_show_view->setWindowTitle(convertor.str().c_str());

  // Show the window
  cfg_show_view->show();
}

void BinSlay::Gui::CallGraphLevelView::doAddToMatchedFunctions()
{
  if (!_core)
    return ;

  // Check if at least one item is selected
  QList<QTableWidgetItem *> selected_items = editPathView->selectedItems();
  if (!selected_items.size())
    return ;

  std::stringstream convertor;
  BinSlay::ReverseAPI::Address addr_fct1 = 0;
  BinSlay::ReverseAPI::Address addr_fct2 = 0;

  // Check if number of selected items equal number of coluns in the view
  if (selected_items.size() != editPathView->columnCount())
    return ;

  // Check if all the cells in the row has been selected and if it is the case, retrieve the addr of
  // the two functions to compare
  int column = 0;
  int row = (*selected_items.begin())->row();
  for (QList<QTableWidgetItem *>::iterator it = selected_items.begin();
       it != selected_items.end(); ++it)
    {
      // check if all selected items are on the same line
      if ((*it)->column() != column || (*it)->row() != row)
	return ;

      // Get the widget associated with the header in the column where we found
      // the current selected widget
      QTableWidgetItem *col_header = NULL;
      col_header = editPathView->horizontalHeaderItem((*it)->column());

      // Check if it is an "Address" column
      if (col_header->text() == "Address 1" || col_header->text() == "Address 2")
	{
	  convertor << (*it)->text().toStdString();
	  if (!addr_fct1) convertor >> std::hex >> addr_fct1;
	  else if (!addr_fct2) convertor >> std::hex >> addr_fct2;
	  convertor.str(std::string());
	  convertor.clear();
	}
      ++column;
    }

  // Delete row from the editPathView
  editPathView->removeRow(row);

  // Add row in the matchedBasicBlocksView
  row = isoListView->rowCount();
  isoListView->insertRow(row);

  // RecLevel 
  QTableWidgetItem *reclevel = new QTableWidgetItem("Manual");

  // Addr left
  convertor << std::hex << addr_fct1;
  QTableWidgetItem *addrleft = new QTableWidgetItem(convertor.str().c_str());
  convertor.str(std::string());
  convertor.clear();

  // Addr right
  convertor << std::hex << addr_fct2;
  QTableWidgetItem *addright = new QTableWidgetItem(convertor.str().c_str());
  convertor.str(std::string());
  convertor.clear();

  isoListView->setItem(row, 0, reclevel);
  isoListView->setItem(row, 1, addrleft);
  isoListView->setItem(row, 3, addright);

  // Delete node from 'UnMatched functions bin1' and 'UnMatched functions bin1'
  // => todo
  
  // Update the internals lists of the cg_core
  _core->add_iso_couple_to_mapping(addr_fct1, addr_fct2);

  std::cout << std::hex << addr_fct1 << " - " << addr_fct2 << std::endl;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////

void BinSlay::Gui::CallGraphLevelView::re_run_bindiff()
{
  // Sanity check
  if (!_core)
    return;

  // Run the algorithm
  if (!_core->re_run_bindiff())
    return;

  // Update results
  _display_bindiff_results();
}

void BinSlay::Gui::CallGraphLevelView::_display_bindiff_results()
{
  // Reset the view
  reset_bindiff();

  // Display the results of the bindiff algorithm in the 'isoListView'
  double nb_isomorphims = 0;
  int row = 0;
  std::stringstream convertor;

  // Fill the matched functions view for the call Graph level
  for (typename BinSlay::bind_node<NodeType>::MAPPING::const_iterator it_map =
	 _core->get_mapping().begin(); it_map != _core->get_mapping().end(); ++it_map)
    {
      for (typename BinSlay::bind_node<NodeType>::ISOMORPHES_LIST::const_iterator it_iso =
	     (*it_map)->begin(); it_iso != (*it_map)->end(); ++it_iso)
  	{
	  // Fill the TableWidget
	  row = isoListView->rowCount();
	  isoListView->insertRow(row);

	  // RecLevel 
	  convertor << std::dec << (*it_iso)->getLevel();
	  QTableWidgetItem *reclevel = new QTableWidgetItem(convertor.str().c_str());
	  convertor.str(std::string());
	  convertor.clear();

	  // Addr left
	  convertor << std::hex << (*it_iso)->getLeft()->getAddr();
	  QTableWidgetItem *addrleft = new QTableWidgetItem(convertor.str().c_str());
	  convertor.str(std::string());
	  convertor.clear();

	  // Name left
	  QTableWidgetItem *nameleft = new QTableWidgetItem((*it_iso)->getLeft()->getName().c_str());

	  // Addr right
	  convertor << std::hex << (*it_iso)->getRight()->getAddr();
	  QTableWidgetItem *addright = new QTableWidgetItem(convertor.str().c_str());
	  convertor.str(std::string());
	  convertor.clear();

	  // Name right
	  QTableWidgetItem *nameright =
	    new QTableWidgetItem(tr((*it_iso)->getRight()->getName().c_str()));

	  isoListView->setItem(row, 0, reclevel);
	  isoListView->setItem(row, 1, addrleft);
	  isoListView->setItem(row, 2, nameleft);
	  isoListView->setItem(row, 3, addright);
	  isoListView->setItem(row, 4, nameright);

  	  ++nb_isomorphims;
  	}
    }

  // Fill the unmatched view for the call-graph level
  for (typename BinSlay::bind_node<NodeType>::NODES_LIST::const_iterator it =
	 _core->get_list_left().begin(); it != _core->get_list_left().end(); ++it)
    {
	  // Fill the TableWidget
	  row = matchedFunctionsView->rowCount();
	  matchedFunctionsView->insertRow(row);

	  // Addr left
	  convertor << std::hex << (*it)->getAddr();
	  QTableWidgetItem *addrleft = new QTableWidgetItem(convertor.str().c_str());
	  convertor.str(std::string());
	  convertor.clear();

	  // Name left
	  QTableWidgetItem *nameleft = new QTableWidgetItem((*it)->getName().c_str());

	  matchedFunctionsView->setItem(row, 0, addrleft);
	  matchedFunctionsView->setItem(row, 1, nameleft);
    }
  for (typename BinSlay::bind_node<NodeType>::NODES_LIST::const_iterator it =
	 _core->get_list_right().begin(); it != _core->get_list_right().end(); ++it)
    {
	  // Fill the TableWidget
	  row = unmatchedFunctionsView->rowCount();
	  unmatchedFunctionsView->insertRow(row);

	  // Addr left
	  convertor << std::hex << (*it)->getAddr();
	  QTableWidgetItem *addright = new QTableWidgetItem(convertor.str().c_str());
	  convertor.str(std::string());
	  convertor.clear();

	  // Name left
	  QTableWidgetItem *nameright = new QTableWidgetItem((*it)->getName().c_str());

	  unmatchedFunctionsView->setItem(row, 0, addright);
	  unmatchedFunctionsView->setItem(row, 1, nameright);
    }

  // Display the percentage of similarty between the two compared graphs
  unsigned int max = _core->getNbNodeInGraphLeft() < _core->getNbNodeInGraphRight() ?
    _core->getNbNodeInGraphLeft() : _core->getNbNodeInGraphRight();

  convertor << "Left: " << std::dec << nb_isomorphims << "/" << _core->getNbNodeInGraphLeft()
	    << " - Right: " << std::dec << nb_isomorphims << "/" << _core->getNbNodeInGraphRight()
	    << " => Percentage: " << (nb_isomorphims / max * 100.0) << "%";
  
  bindiffLineEdit->setText(convertor.str().c_str());  
}

void BinSlay::Gui::CallGraphLevelView::run_bindiff()
{
  if (!_core)
    return ;
  // Get the desired level (0 => CG, 1 => CFG)
  int level = 0;
  if (checkBoxLevels[fcfg]->isChecked())
    ++level;
  // Run algorithm
  if (!_core->run_bindiff_algorithm(static_cast<BinSlay::DiffingLevel>(level)))
    return;
  // Reset the view
  reset_bindiff();
  // Display the results of the bindiff algorithm in the 'isoListView'
  _display_bindiff_results();
}

void BinSlay::Gui::CallGraphLevelView::reset_bindiff()
{
  if (!_core)
    return ;
  int nb_rows = isoListView->rowCount();

  // matched functions
  for (int row = 0; row < nb_rows; ++row)
    for (int col = 0; col < isoListView->columnCount(); ++col)
      delete isoListView->takeItem(row, col);
  isoListView->setRowCount(0);

  // unmatched left
  nb_rows = matchedFunctionsView->rowCount();
  for (int row = 0; row < nb_rows; ++row)
    for (int col = 0; col < matchedFunctionsView->columnCount(); ++col)
      delete matchedFunctionsView->takeItem(row, col);
  matchedFunctionsView->setRowCount(0);

  // unmatched right
  nb_rows = unmatchedFunctionsView->rowCount();
  for (int row = 0; row < nb_rows; ++row)
    for (int col = 0; col < unmatchedFunctionsView->columnCount(); ++col)
      delete unmatchedFunctionsView->takeItem(row, col);
  unmatchedFunctionsView->setRowCount(0);

  bindiffLineEdit->setText("");
}

void BinSlay::Gui::CallGraphLevelView::compute_ged()
{
  if (!_core)
    return ;

  // Get the desired level (0 => CG, 1 => CFG)
  int level = 0;
  if (checkBoxLevels[fcfg]->isChecked())
    ++level;

  std::stringstream convertor;

  // Reset the view
  reset_ged();
  // Compute ged
  // TODO: options - level -> two arguments
  _core->compute_ged(BinSlay::gedProperties::WITH_VALIDATOR);
  //  _core->compute_ged(BinSlay::gedProperties::NO_OPTIONS);

  // Display GED results
  convertor << _core->get_ged();
  gedLineEdit->setText(convertor.str().c_str());
  convertor.str(std::string());
  convertor.clear();

  // Create the edit_path list
  typename BinSlay::bind_node<NodeType>::ISOMORPHES_LIST *edit_path = _core->get_edit_path();
  // std::cout << "nb elem in edit path list: " << edit_path->size() << std::endl;

  // std::cout << "Start displaying edit path..." << std::endl;
  // Show edit path
  int row = 0;
  for (typename BinSlay::bind_node<NodeType>::ISOMORPHES_LIST::const_iterator it_iso =
	 edit_path->begin(); it_iso != edit_path->end(); ++it_iso)
    {
      //      std::cout << "*it_iso = " << *it_iso << std::endl;
      // if (!(*it_iso)->getLeft() && !(*it_iso)->getRight())
      // 	continue;

      // Fill the TableWidget
      row = editPathView->rowCount();
      editPathView->insertRow(row);

      QTableWidgetItem *fct1 = NULL;
      QTableWidgetItem *fct2 = NULL;
      QTableWidgetItem *op = NULL;

      // Cost
      convertor << std::dec << (*it_iso)->getLevel();
      QTableWidgetItem *cost = new QTableWidgetItem(convertor.str().c_str());
      convertor.str(std::string());
      convertor.clear();

      // Operation => substitution
      if ((*it_iso)->getLeft() && (*it_iso)->getRight())
	{
	  // Function from binary 1 
	  convertor << std::hex << (*it_iso)->getLeft()->getAddr();
	  fct1 = new QTableWidgetItem(convertor.str().c_str());
	  convertor.str(std::string());
	  convertor.clear();

	  // Function from binary 2
	  convertor << std::hex << (*it_iso)->getRight()->getAddr();
	  fct2 = new QTableWidgetItem(convertor.str().c_str());
	  convertor.str(std::string());
	  convertor.clear();

	  // Transformation
	  op = new QTableWidgetItem(tr("Substitution"));
	}
      // Operation => deletion
      else if ((*it_iso)->getLeft() && !(*it_iso)->getRight())
	{
	  // Function from binary 1 
	  convertor << std::hex << (*it_iso)->getLeft()->getAddr();
	  fct1 = new QTableWidgetItem(convertor.str().c_str());
	  convertor.str(std::string());
	  convertor.clear();

	  // Transformation
	  op = new QTableWidgetItem(tr("Deletion"));
	}
      // Operation => insertion
      else if (!(*it_iso)->getLeft() && (*it_iso)->getRight())
	{
	  // Function from binary 2
	  convertor << std::hex << (*it_iso)->getRight()->getAddr();
	  fct2 = new QTableWidgetItem(convertor.str().c_str());
	  convertor.str(std::string());
	  convertor.clear();

	  // Transformation
	  op = new QTableWidgetItem(tr("Insertion"));
	}
      // else nothing (dummy node with dummy node)

      editPathView->setItem(row, 0, fct1);
      editPathView->setItem(row, 1, fct2);
      editPathView->setItem(row, 2, op);
      editPathView->setItem(row, 3, cost);
    }
  //std::cout << "End displaying edit path..." << std::endl;

  // // Delete 'edit_path' list
  // _core->delete_edit_path_list(edit_path);
}

void BinSlay::Gui::CallGraphLevelView::reset_ged()
{
  if (!_core)
    return;

  int nb_rows = editPathView->rowCount();

  gedLineEdit->setText(tr(""));

  for (int row = 0; row < nb_rows; ++row)
    for (int col = 0; col < editPathView->columnCount(); ++col)
      delete editPathView->takeItem(row, col);
  editPathView->setRowCount(0);
  gedLineEdit->setText("");
}
