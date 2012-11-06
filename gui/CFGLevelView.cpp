#include "CFGLevelView.h"
#include "ShowBasicBlockView.h"
#include "BasicBlockLevelView.h"

typedef BinSlay::BbNode NodeType;

BinSlay::Gui::CFGLevelView::CFGLevelView(
		 BinSlay::InternalsCore &int_core,
		 BinSlay::CFG_Core &core)
  : _internals_core(int_core), 
    _core(core)
{
  // Use of a vertical layout
  QVBoxLayout *mainLayout = new QVBoxLayout;

  // Selectors choice
  createConfigGroupBox();
  mainLayout->addWidget(configGroupBox);

  // Euclidian distance selection
  createEuclidianDistanceGroupBox();
  mainLayout->addWidget(euclidianDistanceGroupBox);

  // Run buttons
  createBindiffGroupBox();
  mainLayout->addWidget(bindiffGroupBox);

  // Ged
  createGedGroupBox();
  mainLayout->addWidget(gedGroupBox);

  // Results
  createResGroupBox();
  mainLayout->addWidget(resGroupBox);

  setLayout(mainLayout);
}

BinSlay::Gui::CFGLevelView::~CFGLevelView()
{
}

void BinSlay::Gui::CFGLevelView::createBindiffGroupBox()
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

void BinSlay::Gui::CFGLevelView::createResGroupBox()
{
  resGroupBox = new QGroupBox(tr("Results"));
  QVBoxLayout *vbox = new QVBoxLayout;

  tabResults = new QTabWidget;

  createIsoListView();
  createEditPathView();
  createMatchedBasicBlocksView();
  createUnMatchedBasicBlocksView();

  tabResults->addTab(isoListView, tr("Matched Blocks"));
  tabResults->addTab(editPathView, tr("Edit path"));
  tabResults->addTab(matchedBasicBlocksView, tr("Unmatched Blocks 1"));
  tabResults->addTab(unmatchedBasicBlocksView, tr("Unmatched Blocks 2"));
  vbox->addWidget(tabResults);
  resGroupBox->setLayout(vbox);
}

void BinSlay::Gui::CFGLevelView::createIsoListView()
{
  QStringList labels;
  labels << tr("RecLevel") << tr("Address 1") << tr("Address 2");
  isoListView = new QTableWidget(0, 3);

  isoListView->setHorizontalHeaderLabels(labels);
  isoListView->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
  isoListView->horizontalHeader()->setCascadingSectionResizes(true);
  isoListView->horizontalHeader()->setStretchLastSection(true);
  isoListView->verticalHeader()->hide();
  isoListView->setShowGrid(false);

  // Context menu
  isoListView->setContextMenuPolicy(Qt::ActionsContextMenu);
  QAction *showBasicBlockNode  = new QAction(tr("&Show basic block node"), isoListView);
  QAction *diffBasicBlocks  = new QAction(tr("&Diff basic block"), isoListView);
  QAction *deleteFromMatchedBlocks  = new QAction(tr("&Delete from matched list"), isoListView);
  connect(showBasicBlockNode, SIGNAL(triggered()), this, SLOT(doShowBasicBlock_fromIsoList()));
  connect(diffBasicBlocks, SIGNAL(triggered()), this, SLOT(doDiffBasicBlocks_fromIsoList()));
  connect(deleteFromMatchedBlocks, SIGNAL(triggered()), this, SLOT(doDeleteFromMatchedBlocks()));
  isoListView->addAction(showBasicBlockNode);
  isoListView->addAction(diffBasicBlocks);
  isoListView->addAction(deleteFromMatchedBlocks);
}

void BinSlay::Gui::CFGLevelView::createEditPathView()
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
  QAction *showBasicBlockNode  = new QAction(tr("&Show basic block node"), editPathView);
  QAction *diffBasicBlocks  = new QAction(tr("&Diff basic block"), editPathView);
  QAction *addMatchedBlocks  = new QAction(tr("&Add to matched list"), editPathView);
  connect(showBasicBlockNode, SIGNAL(triggered()), this, SLOT(doShowBasicBlock_fromEditPath()));
  connect(diffBasicBlocks, SIGNAL(triggered()), this, SLOT(doDiffBasicBlocks_fromEditPath()));
  connect(addMatchedBlocks, SIGNAL(triggered()), this, SLOT(doAddToMatchedBlocks()));
  editPathView->addAction(showBasicBlockNode);
  editPathView->addAction(diffBasicBlocks);
  editPathView->addAction(addMatchedBlocks);
}

void BinSlay::Gui::CFGLevelView::createMatchedBasicBlocksView()
{
  QStringList labels;
  labels << tr("Address 1");
  matchedBasicBlocksView = new QTableWidget(0, 1);

  matchedBasicBlocksView->setHorizontalHeaderLabels(labels);
  matchedBasicBlocksView->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
  matchedBasicBlocksView->horizontalHeader()->setCascadingSectionResizes(true);
  matchedBasicBlocksView->horizontalHeader()->setStretchLastSection(true);
  matchedBasicBlocksView->verticalHeader()->hide();
  matchedBasicBlocksView->setShowGrid(false);

  // Context menu
  matchedBasicBlocksView->setContextMenuPolicy(Qt::ActionsContextMenu);
  QAction *showBasicBlockNode = new QAction(tr("&Show basic block node"), matchedBasicBlocksView);
  connect(showBasicBlockNode, SIGNAL(triggered()), this, SLOT(doShowBasicBlock_fromMatched()));
  matchedBasicBlocksView->addAction(showBasicBlockNode);
}

void BinSlay::Gui::CFGLevelView::createUnMatchedBasicBlocksView()
{
  QStringList labels;
  labels << tr("Address 2");
  unmatchedBasicBlocksView = new QTableWidget(0, 1);

  unmatchedBasicBlocksView->setHorizontalHeaderLabels(labels);
  unmatchedBasicBlocksView->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
  unmatchedBasicBlocksView->horizontalHeader()->setCascadingSectionResizes(true);
  unmatchedBasicBlocksView->horizontalHeader()->setStretchLastSection(true);
  unmatchedBasicBlocksView->verticalHeader()->hide();
  unmatchedBasicBlocksView->setShowGrid(false);

  // Context menu
  unmatchedBasicBlocksView->setContextMenuPolicy(Qt::ActionsContextMenu);
  QAction *showBasicBlockNode = new QAction(tr("&Show function node"), unmatchedBasicBlocksView);
  connect(showBasicBlockNode, SIGNAL(triggered()), this, SLOT(doShowBasicBlock_fromUnMatched()));
  unmatchedBasicBlocksView->addAction(showBasicBlockNode);
}

void BinSlay::Gui::CFGLevelView::createConfigGroupBox()
{
  configGroupBox = new QGroupBox(tr("Configuration"));
  QHBoxLayout *hbox = new QHBoxLayout;

  // Selectors
  QGroupBox *selectorsGroupBox = new QGroupBox(tr("Selectors"));
  QVBoxLayout *vbox = new QVBoxLayout;

  checkBoxSelectors[cfg] = new QCheckBox(tr("&Control Flow-Graph"));
  checkBoxSelectors[crc32] = new QCheckBox(tr("&Opcode CRC32"));
  checkBoxSelectors[cfg]->setChecked(false);
  checkBoxSelectors[crc32]->setChecked(false);

  connect(checkBoxSelectors[cfg], SIGNAL(toggled(bool)), this, SLOT(doEditBindiffSelector(bool)));
  connect(checkBoxSelectors[crc32], SIGNAL(toggled(bool)), this, SLOT(doEditCrc32Selectors(bool)));

  vbox->addWidget(checkBoxSelectors[cfg]);
  vbox->addWidget(checkBoxSelectors[crc32]);
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

  checkBoxLevels[fcfg] = new QCheckBox(tr("&control flow graph"));
  //checkBoxLevels[bb] = new QCheckBox(tr("&basic block"));
  checkBoxLevels[fcfg]->setChecked(false);
  //checkBoxLevels[bb]->setChecked(false);

  connect(checkBoxLevels[fcfg], SIGNAL(toggled(bool)), this, SLOT(doEditCfgLevel(bool)));
  //connect(checkBoxLevels[bb], SIGNAL(toggled(bool)), this, SLOT(doEditBbLevel(bool)));

  vbox3->addWidget(checkBoxLevels[fcfg]);
  //  vbox3->addWidget(checkBoxLevels[bb]);
  levelsGroupBox->setLayout(vbox3);

  hbox->addWidget(selectorsGroupBox);
  hbox->addWidget(propertiesGroupBox);
  hbox->addWidget(levelsGroupBox);
  configGroupBox->setLayout(hbox);
}

void BinSlay::Gui::CFGLevelView::createEuclidianDistanceGroupBox()
{
  euclidianDistanceGroupBox = new QGroupBox(tr("Eucliadian distance"));
  QGridLayout *layout = new QGridLayout;

  eucDistLabel = new QLabel(tr("Choose a distance:"));
  eucDistSpinBox = new QSpinBox;
  eucDistSpinBox->setRange(0, 10);
  eucDistSpinBox->setSingleStep(1);  
  layout->addWidget(eucDistLabel, 0, 0);
  layout->addWidget(eucDistSpinBox, 0, 1);
  euclidianDistanceGroupBox->setLayout(layout);
}

void BinSlay::Gui::CFGLevelView::createGedGroupBox()
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

////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>

void BinSlay::Gui::CFGLevelView::doEditUpProperty(bool state)
{
  if (state)
    _core.add_Property(BinSlay::idProperties::UP);
  else
    _core.remove_Property(BinSlay::idProperties::UP);
}

void BinSlay::Gui::CFGLevelView::doEditDownProperty(bool state)
{
  if (state)
    _core.add_Property(BinSlay::idProperties::DOWN);
  else
    _core.remove_Property(BinSlay::idProperties::DOWN);
}

void BinSlay::Gui::CFGLevelView::doEditCfgLevel(bool)
{
}

// void BinSlay::Gui::CFGLevelView::doEditBbLevel(bool)
// { 
// }

void BinSlay::Gui::CFGLevelView::doEditBindiffSelector(bool state)
{
  if (state)
    {
      _core.add_Selector(BinSlay::idSelectors::CFG);
    }
  else
    _core.remove_Selector(BinSlay::idSelectors::CFG);
}

void BinSlay::Gui::CFGLevelView::doEditCrc32Selectors(bool state)
{
  if (state)
    _core.add_Selector(BinSlay::idSelectors::CRC32);
  else
    _core.remove_Selector(BinSlay::idSelectors::CRC32);
}

void BinSlay::Gui::CFGLevelView::doShowBasicBlock_fromIsoList()
{
  showBasicBlock(ISO_LIST_VIEW);
}

void BinSlay::Gui::CFGLevelView::doShowBasicBlock_fromEditPath()
{
  showBasicBlock(EDIT_PATH_VIEW);
}

void BinSlay::Gui::CFGLevelView::doShowBasicBlock_fromMatched()
{
  showBasicBlock(MATCHED_VIEW);
}

void BinSlay::Gui::CFGLevelView::doShowBasicBlock_fromUnMatched()
{
  showBasicBlock(UNMATCHED_VIEW);
}

void BinSlay::Gui::CFGLevelView::showBasicBlock(view_type which_one)
{
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
      table_view = matchedBasicBlocksView;
      break;

    case UNMATCHED_VIEW:
      table_view = unmatchedBasicBlocksView;
      break;

    default:
      return;
    }

  // Retrieve the address of the desired basic block and determine in which binary this
  // basic block resides
  BinSlay::ReverseAPI::Address bbAddr = 0;
  int which_binary = 0;

  QTableWidgetItem *addr = table_view->currentItem();

  // Get the widget associated with the header in the column where we found
  // the current selected widget
  QTableWidgetItem *col_header = NULL;
  col_header = table_view->horizontalHeaderItem(addr->column());

  // Check if it is an "Address" column and find in which binary this basic block resides
  if (col_header->text() == "Address 1")
    {
      which_binary = 0;
      convertor << addr->text().toStdString();
      convertor >> std::hex >> bbAddr;
      convertor.str(std::string());
      convertor.clear(); 
    }
  else if (col_header->text() == "Address 2")
    {
      which_binary = 1;
      convertor << addr->text().toStdString();
      convertor >> std::hex >> bbAddr;
      convertor.str(std::string());
      convertor.clear();
    }
  else
    // This is not a valid address !
    return ;

  std::cout << "Addr: " << std::hex << bbAddr << std::endl;

  // Create a BasicBlock item
  BinSlay::BasicBlock *basic_block = _internals_core.create_basic_block(&_core, bbAddr, which_binary);

  std::cout << "Basic block created!" << std::hex << bbAddr << std::endl;

  // Create a new instance of a ShowBasicBlockView with the create BasicBlock object
  BinSlay::Gui::ShowBasicBlockView *bb_show_view =
    new BinSlay::Gui::ShowBasicBlockView(_internals_core, basic_block);

  std::cout << "Basic block view created!" << std::hex << bbAddr << std::endl;

  // Set the title
  convertor << "Show basic block: @" << std::hex << bbAddr;
  bb_show_view->setWindowTitle(convertor.str().c_str());

  // Show the window
  bb_show_view->show();
}

void BinSlay::Gui::CFGLevelView::doDiffBasicBlocks_fromIsoList()
{
  diffBasicBlocks(ISO_LIST_VIEW);
}

void BinSlay::Gui::CFGLevelView::doDiffBasicBlocks_fromEditPath()
{
  diffBasicBlocks(EDIT_PATH_VIEW);
}

void BinSlay::Gui::CFGLevelView::doDiffBasicBlocks_fromMatched()
{
  diffBasicBlocks(MATCHED_VIEW);
}

void BinSlay::Gui::CFGLevelView::doDiffBasicBlocks_fromUnMatched()
{
  diffBasicBlocks(UNMATCHED_VIEW);
}

void BinSlay::Gui::CFGLevelView::diffBasicBlocks(view_type which_one)
{
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
      table_view = matchedBasicBlocksView;
      break;

    case UNMATCHED_VIEW:
      table_view = unmatchedBasicBlocksView;
      break;

    default:
      return;
    }

  // Check if at least one item is selected
  QList<QTableWidgetItem *> selected_items = table_view->selectedItems();
  if (!selected_items.size())
    return ;

  std::stringstream convertor;
  BinSlay::ReverseAPI::Address addr_bb1 = 0;
  BinSlay::ReverseAPI::Address addr_bb2 = 0;

  // Check if number of selected items equal number of coluns in the view
  if (selected_items.size() != table_view->columnCount())
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
      col_header = table_view->horizontalHeaderItem((*it)->column());

      // Check if it is an "Address" column
      if (col_header->text() == "Address 1" || col_header->text() == "Address 2")
	{
	  convertor << (*it)->text().toStdString();
	  if (!addr_bb1) convertor >> std::hex >> addr_bb1;
	  else if (!addr_bb2) convertor >> std::hex >> addr_bb2;
	  convertor.str(std::string());
	  convertor.clear();
	}
      ++column;
    }

  // Create an new instance of a bb_core
  BinSlay::BasicBlock_Core *bb_core = _internals_core.createBasicBlock_Core(&_core, addr_bb1, addr_bb2);

  // Create a new instance of a BasicBlockLevelView with the new cerated bb_core
  BinSlay::Gui::BasicBlockLevelView *bb_diff_view =
    new BinSlay::Gui::BasicBlockLevelView(_internals_core, *bb_core);

  // Make title of this subwindow
  convertor << "Diff basic blocks: @" << std::hex << addr_bb1 << " VS @" << std::hex << addr_bb2;
  bb_diff_view->setWindowTitle(convertor.str().c_str());

  // Show the window
  bb_diff_view->show();

}

void BinSlay::Gui::CFGLevelView::doAddToMatchedBlocks()
{
  // Check if at least one item is selected
  QList<QTableWidgetItem *> selected_items = editPathView->selectedItems();
  if (!selected_items.size())
    return ;

  std::stringstream convertor;
  BinSlay::ReverseAPI::Address addr_bb1 = 0;
  BinSlay::ReverseAPI::Address addr_bb2 = 0;

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
	  if (!addr_bb1) convertor >> std::hex >> addr_bb1;
	  else if (!addr_bb2) convertor >> std::hex >> addr_bb2;
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
  convertor << std::hex << addr_bb1;
  QTableWidgetItem *addrleft = new QTableWidgetItem(convertor.str().c_str());
  convertor.str(std::string());
  convertor.clear();

  // Addr right
  convertor << std::hex << addr_bb2;
  QTableWidgetItem *addright = new QTableWidgetItem(convertor.str().c_str());
  convertor.str(std::string());
  convertor.clear();

  isoListView->setItem(row, 0, reclevel);
  isoListView->setItem(row, 1, addrleft);
  isoListView->setItem(row, 2, addright);

  std::cout << std::hex << addr_bb1 << " - " << addr_bb2 << std::endl;
}

void BinSlay::Gui::CFGLevelView::doDeleteFromMatchedBlocks()
{
  
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void BinSlay::Gui::CFGLevelView::re_run_bindiff()
{
  // Run the algorithm
  if (!_core.re_run_bindiff())
    return;

  // Update results
  _display_bindiff_results();
}

void BinSlay::Gui::CFGLevelView::_display_bindiff_results()
{
  // Reset the view
  reset_bindiff();

  // Display the results of the bindiff algorithm in the 'isoListView'
  double nb_isomorphims = 0;
  int row = 0;
  std::stringstream convertor;

  // Call Graph level
  for (typename BinSlay::bind_node<NodeType>::MAPPING::const_iterator it_map =
	 _core.get_mapping().begin(); it_map != _core.get_mapping().end(); ++it_map)
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

	  // Addr right
	  convertor << std::hex << (*it_iso)->getRight()->getAddr();
	  QTableWidgetItem *addright = new QTableWidgetItem(convertor.str().c_str());
	  convertor.str(std::string());
	  convertor.clear();

	  isoListView->setItem(row, 0, reclevel);
	  isoListView->setItem(row, 1, addrleft);
	  isoListView->setItem(row, 2, addright);
  	  ++nb_isomorphims;
  	}
    }
  // Fill the unmatched view for the call-graph level
  for (typename BinSlay::bind_node<NodeType>::NODES_LIST::const_iterator it =
	 _core.get_list_left().begin(); it != _core.get_list_left().end(); ++it)
    {
	  // Fill the TableWidget
	  row = matchedBasicBlocksView->rowCount();
	  matchedBasicBlocksView->insertRow(row);

	  // Addr left
	  convertor << std::hex << (*it)->getAddr();
	  QTableWidgetItem *addrleft = new QTableWidgetItem(convertor.str().c_str());
	  convertor.str(std::string());
	  convertor.clear();

	  matchedBasicBlocksView->setItem(row, 0, addrleft);
    }
  for (typename BinSlay::bind_node<NodeType>::NODES_LIST::const_iterator it =
	 _core.get_list_right().begin(); it != _core.get_list_right().end(); ++it)
    {
	  // Fill the TableWidget
	  row = unmatchedBasicBlocksView->rowCount();
	  unmatchedBasicBlocksView->insertRow(row);

	  // Addr left
	  convertor << std::hex << (*it)->getAddr();
	  QTableWidgetItem *addright = new QTableWidgetItem(convertor.str().c_str());
	  convertor.str(std::string());
	  convertor.clear();

	  unmatchedBasicBlocksView->setItem(row, 0, addright);
    }

  // Display the percentage of similarty between the two compared graphs
  int max = _core.getNbNodeInGraphLeft() < _core.getNbNodeInGraphRight() ?
    _core.getNbNodeInGraphLeft() : _core.getNbNodeInGraphRight();

  convertor << "Left: " << std::dec << nb_isomorphims << "/" << _core.getNbNodeInGraphLeft()
	    << " - Right: " << std::dec << nb_isomorphims << "/" << _core.getNbNodeInGraphRight()
	    << " => Percentage: " << (nb_isomorphims / max * 100.0) << "%";
  
  bindiffLineEdit->setText(convertor.str().c_str());  
}

void BinSlay::Gui::CFGLevelView::run_bindiff()
{
  // Run algorithm
  if (!_core.run_bindiff_algorithm())
    return;
  // Display results
  _display_bindiff_results();  
}

void BinSlay::Gui::CFGLevelView::reset_bindiff()
{
  int nb_rows = isoListView->rowCount();

  // matched
  for (int row = 0; row < nb_rows; ++row)
    for (int col = 0; col < isoListView->columnCount(); ++col)
      delete isoListView->takeItem(row, col);
  isoListView->setRowCount(0);

  // unmatched left
  nb_rows = matchedBasicBlocksView->rowCount();
  for (int row = 0; row < nb_rows; ++row)
    for (int col = 0; col < matchedBasicBlocksView->columnCount(); ++col)
      delete matchedBasicBlocksView->takeItem(row, col);
  matchedBasicBlocksView->setRowCount(0);

  // unmatched right
  nb_rows = unmatchedBasicBlocksView->rowCount();
  for (int row = 0; row < nb_rows; ++row)
    for (int col = 0; col < unmatchedBasicBlocksView->columnCount(); ++col)
      delete unmatchedBasicBlocksView->takeItem(row, col);
  unmatchedBasicBlocksView->setRowCount(0);

  bindiffLineEdit->setText("");
}

void BinSlay::Gui::CFGLevelView::compute_ged()
{
  std::stringstream convertor;

  // Reset the view
  reset_ged();

  // Compute ged
  _core.compute_ged();

  // Display GED results
  convertor << _core.get_ged();
  gedLineEdit->setText(convertor.str().c_str());
  convertor.str(std::string());
  convertor.clear();

  // Create the edit_path list
  typename BinSlay::bind_node<NodeType>::ISOMORPHES_LIST *edit_path = _core.get_edit_path();
  std::cout << "nb elem in edit path list: " << edit_path->size() << std::endl;

  // Show edit path
  int row = 0;
  for (typename BinSlay::bind_node<NodeType>::ISOMORPHES_LIST::const_iterator it_iso =
	 edit_path->begin(); it_iso != edit_path->end(); ++it_iso)
    {
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

  // Delete 'edit_path' list
  _core.delete_edit_path_list(edit_path);
}

void BinSlay::Gui::CFGLevelView::reset_ged()
{
  int nb_rows = editPathView->rowCount();

  gedLineEdit->setText(tr(""));

  for (int row = 0; row < nb_rows; ++row)
    for (int col = 0; col < editPathView->columnCount(); ++col)
      delete editPathView->takeItem(row, col);
  editPathView->setRowCount(0);
  gedLineEdit->setText("");
}

void BinSlay::Gui::CFGLevelView::closeEvent(QCloseEvent*)
{
  // Free allocated memory for object cfg_core: all cores manipulations
  // have to be done via the '_internals_core' !!! Do not try to delete
  // your given core yourself.
  //_internals_core.deleteCFG_Core(&_core);

  // Delete this CFGLevelView instance: normally, Qt takes care of the automatic
  // deletion of widgets allocated on the heap when the parent of the widget dies.
  // But in this case, the parent is the MAIN window of our application, so we want to
  // delete this child window before the end of the program to avoid it growing faster
  // in terms of allocated memory.
  delete this;
}
