#include "MainWindow.h"

BinSlay::Gui::MainWindow::MainWindow(BinSlay::InternalsCore &core)
  : _core(core),
    _open_view(nullptr),
    _call_graph_view(nullptr)
{
  // Create the menu
  createMenus();

  // Create the '_call_graph_view' and the '_open_view' and set the
  // '_call_graph_view' as the central widget of the main window
  _call_graph_view = new BinSlay::Gui::CallGraphLevelView(core, nullptr);
  _open_view = new BinSlay::Gui::OpenBinaryView(core, *_call_graph_view);
  setCentralWidget(_call_graph_view);

  setWindowTitle(tr("BinSlayer v0.1"));
}

void BinSlay::Gui::MainWindow::createMenus()
{
  fileMenu = menuBar()->addMenu(tr("&File"));

  openAction = new QAction(tr("&Open"), this);
  exitAction = new QAction(tr("E&xit"), this);

  fileMenu->addAction(openAction);
  fileMenu->addSeparator();
  fileMenu->addAction(exitAction);

  connect(openAction, SIGNAL(triggered()), this, SLOT(open()));
  connect(exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
}

void BinSlay::Gui::MainWindow::open()
{
  // Show the 'open' window to load a new couple of binaries
  _open_view->show();
}
