#include <QApplication>
#include "MainWindow.h"
#include "InternalsCore.hh"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  // Create a instance of the InternalsCore
  BinSlay::InternalsCore core;

  // Load internal libraries (BinaryHelper)
  if (!core.init())
    {
      std::cerr << core.getErrorBuffer().str() << std::endl;
      return 1;
    }

  BinSlay::Gui::MainWindow window(core);
  window.show();
  return app.exec();
}
