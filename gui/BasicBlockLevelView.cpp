#include "BasicBlockLevelView.h"

BinSlay::Gui::BasicBlockLevelView::BasicBlockLevelView(
			    BinSlay::InternalsCore &int_core,
			    BinSlay::BasicBlock_Core &bb_core)
  : _internals_core(int_core),
    _bb_core(bb_core)
{
  // Use of a vertical layout
  QVBoxLayout *mainLayout = new QVBoxLayout;

  // Info
  createButtonsGroupBox();
  mainLayout->addWidget(buttonsGroupBox);

  // Disassembly
  createDisassemblyGroupBox();
  mainLayout->addWidget(disassemblyGroupBox);

  setLayout(mainLayout);
}

BinSlay::Gui::BasicBlockLevelView::~BasicBlockLevelView()
{
}

void BinSlay::Gui::BasicBlockLevelView::createDisassemblyGroupBox()
{
  disassemblyGroupBox = new QGroupBox;
  QHBoxLayout *hbox = new QHBoxLayout;

  QStringList labels;
  labels << tr("Offset") << tr("Raw") << tr("Disassembly")
	 << tr("Cost");

  disassemblyLeftView = new QTableWidget(0, 3);
  disassemblyLeftView->setHorizontalHeaderLabels(labels);
  disassemblyLeftView->horizontalHeader()->setCascadingSectionResizes(true);
  disassemblyLeftView->horizontalHeader()->setStretchLastSection(true);
  disassemblyLeftView->verticalHeader()->hide();
  disassemblyLeftView->setShowGrid(false);

  disassemblyRightView = new QTableWidget(0, 3);
  disassemblyRightView->setHorizontalHeaderLabels(labels);
  disassemblyRightView->horizontalHeader()->setCascadingSectionResizes(true);
  disassemblyRightView->horizontalHeader()->setStretchLastSection(true);
  disassemblyRightView->verticalHeader()->hide();
  disassemblyRightView->setShowGrid(false);

  hbox->addWidget(disassemblyLeftView);
  hbox->addWidget(disassemblyRightView);
  disassemblyGroupBox->setLayout(hbox);
}

void BinSlay::Gui::BasicBlockLevelView::createButtonsGroupBox()
{
  buttonsGroupBox = new QGroupBox;
  QHBoxLayout *hbox = new QHBoxLayout;

  diffButton = new QPushButton(tr("Diff"));
  resetButton = new QPushButton(tr("Reset"));

  connect(diffButton, SIGNAL(clicked()), this, SLOT(doDiff()));
  connect(resetButton, SIGNAL(clicked()), this, SLOT(doResetDiff()));

  hbox->addWidget(diffButton);
  hbox->addWidget(resetButton);
  buttonsGroupBox->setLayout(hbox);
}

void BinSlay::Gui::BasicBlockLevelView::doDiff()
{
  std::stringstream convertor;
  int row = 0;

  BinSlay::ReverseAPI::INSTR_LIST *instrs_left =
    _bb_core.getLeft().getBinary()->recover_basic_block_instr(_bb_core.getLeft().getNode().getAddr());
  BinSlay::ReverseAPI::INSTR_LIST *instrs_right =
    _bb_core.getRight().getBinary()->recover_basic_block_instr(_bb_core.getRight().getNode().getAddr());

  // basic block left
  row = 0;
  for (BinSlay::ReverseAPI::INSTR_LIST::const_iterator it = instrs_left->begin();
       it != instrs_left->end(); ++it)
    {
      // Add a new row
      row = disassemblyLeftView->rowCount();
      disassemblyLeftView->insertRow(row);

      // Offset
      convertor << std::hex << (*it)->_offset;
      QTableWidgetItem *offset = new QTableWidgetItem(convertor.str().c_str());
      convertor.str(std::string());
      convertor.clear();  
      // Raw
      QTableWidgetItem *raw = new QTableWidgetItem((*it)->_raw.c_str());
      // Asm
      QTableWidgetItem *asm_instr = new QTableWidgetItem((*it)->_disassembly.c_str());

      disassemblyLeftView->setItem(row, 0, offset);
      disassemblyLeftView->setItem(row, 1, raw);
      disassemblyLeftView->setItem(row, 2, asm_instr);
    }  

  // basic block right
  row = 0;
  for (BinSlay::ReverseAPI::INSTR_LIST::const_iterator it = instrs_right->begin();
       it != instrs_right->end(); ++it)
    {
      // Add a new row
      row = disassemblyRightView->rowCount();
      disassemblyRightView->insertRow(row);

      // Offset
      convertor << std::hex << (*it)->_offset;
      QTableWidgetItem *offset = new QTableWidgetItem(convertor.str().c_str());
      convertor.str(std::string());
      convertor.clear();  
      // Raw
      QTableWidgetItem *raw = new QTableWidgetItem((*it)->_raw.c_str());
      // Asm
      QTableWidgetItem *asm_instr = new QTableWidgetItem((*it)->_disassembly.c_str());

      disassemblyRightView->setItem(row, 0, offset);
      disassemblyRightView->setItem(row, 1, raw);
      disassemblyRightView->setItem(row, 2, asm_instr);
    }
}

void BinSlay::Gui::BasicBlockLevelView::doResetDiff()
{
}

void BinSlay::Gui::BasicBlockLevelView::closeEvent(QCloseEvent*)
{
  // Free allocated memory for object bb_core: all cores manipulations
  // have to be done via the '_internals_core' !!! Do not try to delete
  // your given core yourself.
  _internals_core.deleteBasicBlock_Core(&_bb_core);

  // Delete this BasicBlockLevelView instance: normally, Qt takes care of the automatic
  // deletion of widgets allocated on the heap when the parent of the widget dies.
  // But in this case, the parent is the MAIN window of our application, so we want to
  // delete this child window before the end of the program to avoid it growing faster
  // in terms of allocated memory.
  delete this;
}
