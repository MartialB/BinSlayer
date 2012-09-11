#include "OpenBinaryView.h"

BinSlay::Gui::OpenBinaryView::OpenBinaryView(BinSlay::InternalsCore &core,
					     BinSlay::Gui::CallGraphLevelView &cg_view)
  : _core(core),
    _cg_view(cg_view)
{
  // Use of a vertical layout
  QVBoxLayout *mainLayout = new QVBoxLayout;

  // Open files
  createOpenGroupBox();
  mainLayout->addWidget(openGroupBox);

  // Load button
  loadButton = new QPushButton("Load");
  connect(loadButton, SIGNAL(clicked()), this, SLOT(load_binaries()));
  mainLayout->addWidget(loadButton);

  setLayout(mainLayout);
  setWindowTitle(tr("Open"));
}

void BinSlay::Gui::OpenBinaryView::createOpenGroupBox()
{
  openGroupBox = new QGroupBox(tr("Select binaries to compare"));
  QFormLayout *layout = new QFormLayout;

  bin1Button = new QPushButton("Binary 1");
  bin2Button = new QPushButton("Binary 2");
  bin1lineEdit = new QLineEdit;
  bin1lineEdit->setReadOnly(true);
  bin2lineEdit = new QLineEdit;
  bin2lineEdit->setReadOnly(true);

  connect(bin1Button, SIGNAL(clicked()), this, SLOT(open_binary_1()));
  connect(bin2Button, SIGNAL(clicked()), this, SLOT(open_binary_2()));

  // Select binary 2
  QHBoxLayout *hbox_1 = new QHBoxLayout;
  hbox_1->addWidget(bin1Button);
  hbox_1->addWidget(bin1lineEdit);
  layout->addRow(hbox_1);
  
  // Select binary 2
  QHBoxLayout *hbox_2 = new QHBoxLayout;
  hbox_2->addWidget(bin2Button);
  hbox_2->addWidget(bin2lineEdit);
  layout->addRow(hbox_2);
  
  openGroupBox->setLayout(layout);
}

void BinSlay::Gui::OpenBinaryView::open_binary_1()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "",
					       tr("Binary (*)"));
  
  if (fileName != "")
    {
      bin1lineEdit->setText(fileName);
    }
}

void BinSlay::Gui::OpenBinaryView::open_binary_2()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "",
					       tr("Binary (*)"));
  
  if (fileName != "")
    {
      bin2lineEdit->setText(fileName);
    }
}

void BinSlay::Gui::OpenBinaryView::load_binaries()
{
  // Create a new core for the given couple of binaries
  BinSlay::CG_Core *cg_core =
    _core.createCG_Core(bin1lineEdit->text().toStdString(), bin2lineEdit->text().toStdString());

  // Reset the '_cg_view'
  _cg_view.reset();

  // Update the 'cg_core' in the view
  _cg_view.set_CG_Core(cg_core);

  // Hide the open_view and reset its QLineEdit
  this->hide();
  bin1lineEdit->setText(tr(""));
  bin2lineEdit->setText(tr(""));
}
