#ifdef __linux__
# include <stdlib.h>
#endif

#include "ShowFunctionView.h"

BinSlay::Gui::ShowFunctionView::ShowFunctionView(
				   BinSlay::InternalsCore &int_core,
				   BinSlay::Function *fct)
  : _internals_core(int_core),
    _function(fct)
{
  // Use of a vertical layout
  QVBoxLayout *mainLayout = new QVBoxLayout;

  // Info
  createInfoGroupBox();
  mainLayout->addWidget(infoGroupBox);

  // cfg dot button
  cfgDotFormatButton = new QPushButton(tr("Create the .dot for this CFG"));
  connect(cfgDotFormatButton, SIGNAL(clicked()), this, SLOT(cfg_to_dot_format()));
  mainLayout->addWidget(cfgDotFormatButton);

  // show cfg button
  showCFGButton = new QPushButton(tr("Display the CFG of this function"));
  connect(showCFGButton, SIGNAL(clicked()), this, SLOT(doShowCFG()));
  mainLayout->addWidget(showCFGButton);

  // Disassembly
  createDisassemblyGroupBox();
  mainLayout->addWidget(disassemblyGroupBox);

  setLayout(mainLayout);
}

void BinSlay::Gui::ShowFunctionView::createInfoGroupBox()
{
  std::stringstream convertor;

  infoGroupBox = new QGroupBox(tr("Info"));
  QFormLayout *formLayout = new QFormLayout;

  convertor << std::hex << _function->getNode().getAddr();
  addrLineEdit = new QLineEdit(convertor.str().c_str());
  addrLineEdit->setReadOnly(true);
  convertor.str(std::string());
  convertor.clear();

  nameLineEdit = new QLineEdit(_function->getNode().getName().c_str());
  nameLineEdit->setReadOnly(true);

  convertor << std::dec << _function->getNode().getLabel().getX();
  nbBasicBlocksLineEdit = new QLineEdit(convertor.str().c_str());
  nbBasicBlocksLineEdit->setReadOnly(true);
  convertor.str(std::string());
  convertor.clear();

  convertor << std::dec << _function->getNode().getLabel().getY();
  nbEdgesLineEdit = new QLineEdit(convertor.str().c_str());
  nbEdgesLineEdit->setReadOnly(true);
  convertor.str(std::string());
  convertor.clear();

  convertor << std::dec << _function->getNode().getOutEdges();
  nbCallsOutLineEdit = new QLineEdit(convertor.str().c_str());
  nbCallsOutLineEdit->setReadOnly(true);
  convertor.str(std::string());
  convertor.clear();

  convertor << std::dec << _function->getNode().getInEdges();
  nbCallsInLineEdit = new QLineEdit(convertor.str().c_str());
  nbCallsInLineEdit->setReadOnly(true);
  convertor.str(std::string());
  convertor.clear();

  formLayout->addRow(tr("&Address:"), addrLineEdit);
  formLayout->addRow(tr("&Name:"), nameLineEdit);
  formLayout->addRow(tr("&Number of basic blocks:"), nbBasicBlocksLineEdit);
  formLayout->addRow(tr("&Number of internals edges:"), nbEdgesLineEdit);
  formLayout->addRow(tr("&Number of outgoing calls:"), nbCallsOutLineEdit);
  formLayout->addRow(tr("&Number of incomming calls:"), nbCallsInLineEdit);

  infoGroupBox->setLayout(formLayout);
}

void BinSlay::Gui::ShowFunctionView::createDisassemblyView()
{
  QStringList labels;
  labels << tr("Offset") << tr("Raw") << tr("Disassembly")
	 << tr("Cost");
  disassemblyView = new QTableWidget(0, 3);

  disassemblyView->setHorizontalHeaderLabels(labels);
  disassemblyView->horizontalHeader()->setCascadingSectionResizes(true);
  disassemblyView->horizontalHeader()->setStretchLastSection(true);
  disassemblyView->verticalHeader()->hide();
  disassemblyView->setShowGrid(false);
}

void BinSlay::Gui::ShowFunctionView::createDisassemblyGroupBox()
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

void BinSlay::Gui::ShowFunctionView::cfg_to_dot_format()
{
  _function->drawMe(_function->getBinary()->getFileName()
		    + "_0x"
		    + addrLineEdit->text().toStdString());
}

void BinSlay::Gui::ShowFunctionView::doShowCFG()
{
#ifdef __linux__
  if (!getenv("DISPLAY"))
    return;

  std::string sh_cmd("dot -Txlib "
		     + _function->getBinary()->getFileName()
		     + "_0x"
		     + addrLineEdit->text().toStdString()
		     + ".dot"
		     + " &");

  system(sh_cmd.c_str());
#endif
}

void BinSlay::Gui::ShowFunctionView::doDisasembly()
{
  std::stringstream convertor;
  std::string disassembly_text;
  int row = 0;

  BinSlay::ReverseAPI::INSTR_LIST *instrs =
    _function->getBinary()->recover_function_instr(_function->getNode().getAddr());

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

void BinSlay::Gui::ShowFunctionView::closeEvent(QCloseEvent*)
{
  // Delete the BinSlay::Function object
  delete this->_function;

  // Delete this ShowFunctionView instance: normally, Qt takes care of the automatic
  // deletion of widgets allocated on the heap when the parent of the widget dies.
  // But in this case, the parent is the MAIN window of our application, so we want to
  // delete this child window before the end of the program to avoid it growing faster
  // in terms of allocated memory.
  delete this;
}
