#ifndef MAINWINDOW_H
# define MAINWINDOW_H

# include <QMainWindow>
# include <QAction>
# include <QMenu>
# include <QMenuBar>
# include "InternalsCore.hh"
# include "OpenBinaryView.h"
# include "CallGraphLevelView.h"

namespace BinSlay {
  namespace Gui {

    class MainWindow : public QMainWindow
    {
      Q_OBJECT

      public:
      MainWindow(BinSlay::InternalsCore &core);

    protected:

    private slots:
      void open();

    private:
      void createMenus();

    private:
      BinSlay::InternalsCore &_core;
      BinSlay::Gui::OpenBinaryView *_open_view;
      BinSlay::Gui::CallGraphLevelView *_call_graph_view;

      QMenu *fileMenu;
      QAction *openAction;
      QAction *exitAction;
    };
  }
}

#endif /* !MAINWINDOW_H */
