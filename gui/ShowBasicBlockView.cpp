#include "ShowBasicBlockView.h"

BinSlay::Gui::ShowBasicBlockView::ShowBasicBlockView(
			    BinSlay::InternalsCore &int_core,
			    BinSlay::BasicBlock *bb)
  : _internals_core(int_core),
    _basic_block(bb)
{
  // Use of a vertical layout
  QVBoxLayout *mainLayout = new QVBoxLayout;

  // Info
  createInfoGroupBox();
  mainLayout->addWidget(infoGroupBox);

  // Disassembly
  createDisassemblyGroupBox();
  mainLayout->addWidget(disassemblyGroupBox);

  setLayout(mainLayout);
}

void BinSlay::Gui::ShowBasicBlockView::createInfoGroupBox()
{
  std::stringstream convertor;

  infoGroupBox = new QGroupBox(tr("Info"));
  QFormLayout *formLayout = new QFormLayout;

  convertor << std::hex << _basic_block->getNode().getAddr();
  addrLineEdit = new QLineEdit(convertor.str().c_str());
  addrLineEdit->setReadOnly(true);
  convertor.str(std::string());
  convertor.clear();

  // convertor << std::hex << _basic_block->getNode().getAddr();
  // nbInstrLineEdit = new QLineEdit(convertor.str().c_str());
  // nbInstrLineEdit->setReadOnly(true);
  // convertor.str(std::string());
  // convertor.clear();

  convertor << std::hex << _basic_block->getNode().getOutEdges();
  nbCallsOutLineEdit = new QLineEdit(convertor.str().c_str());
  nbCallsOutLineEdit->setReadOnly(true);
  convertor.str(std::string());
  convertor.clear();

  formLayout->addRow(tr("&Address:"), addrLineEdit);
  //  formLayout->addRow(tr("&Number of instruction:"), nbInstrLineEdit);
  formLayout->addRow(tr("&Number of outgoing calls:"), nbCallsOutLineEdit);

  infoGroupBox->setLayout(formLayout);
}

void BinSlay::Gui::ShowBasicBlockView::createDisassemblyView()
{
  QStringList labels;
  labels << tr("Offset") << tr("Raw") << tr("Disassembly")
	 << tr("Cost");
  disassemblyView = new QTableWidget(0, 3);

  disassemblyView->setHorizontalHeaderLabels(labels);
  disassemblyView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
  disassemblyView->horizontalHeader()->setCascadingSectionResizes(true);
  disassemblyView->horizontalHeader()->setStretchLastSection(true);
  disassemblyView->verticalHeader()->hide();
  disassemblyView->setShowGrid(false);
}

void BinSlay::Gui::ShowBasicBlockView::createDisassemblyGroupBox()
{
  disassemblyGroupBox = new QGroupBox(tr("Disassembly view"));
  QVBoxLayout *vbox = new QVBoxLayout;

  disassemblyButton = new QPushButton(tr("Disassemble"));
  createDisassemblyView();

  connect(disassemblyButton, SIGNAL(clicked()), this, SLOT(doDisasembly()));

  vbox->addWidget(disassemblyButton);
  vbox->addWidget(disassemblyView);
  disassemblyGroupBox->setLayout(vbox);
}

void BinSlay::Gui::ShowBasicBlockView::doDisasembly()
{
  std::stringstream convertor;
  std::string disassembly_text;
  int row = 0;

  BinSlay::ReverseAPI::INSTR_LIST *instrs =
    _basic_block->getBinary()->recover_basic_block_instr(_basic_block->getNode().getAddr());

  for (BinSlay::ReverseAPI::INSTR_LIST::const_iterator it = instrs->begin();
       it != instrs->end(); ++it)
    {
      // Add a new row
      row = disassemblyView->rowCount();
      disassemblyView->insertRow(row);

      // Offset
      convertor << std::hex << (*it)->_offset;
      QTableWidgetItem *offset = new QTableWidgetItem(convertor.str().c_str());
      convertor.str(std::string());
      convertor.clear();  
      // Raw
      QTableWidgetItem *raw = new QTableWidgetItem((*it)->_raw.c_str());
      // Asm
      QTableWidgetItem *asm_instr = new QTableWidgetItem((*it)->_disassembly.c_str());

      disassemblyView->setItem(row, 0, offset);
      disassemblyView->setItem(row, 1, raw);
      disassemblyView->setItem(row, 2, asm_instr);
    }  
}

void BinSlay::Gui::ShowBasicBlockView::closeEvent(QCloseEvent*)
{
  // Delete the BinSlay::BasicBlock object
  delete _basic_block;

  // Delete this ShowBasicBlockView instance: normally, Qt takes care of the automatic
  // deletion of widgets allocated on the heap when the parent of the widget dies.
  // But in this case, the parent is the MAIN window of our application, so we want to
  // delete this child window before the end of the program to avoid it growing faster
  // in terms of allocated memory.
  delete this;
}
