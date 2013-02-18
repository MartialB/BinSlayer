#ifndef OPEN_BINARY_VIEW_H
# define OPEN_BINARY_VIEW_H

# include <QDialog>
# include <QGroupBox>
# include <QTableWidget>
# include <QLineEdit>
# include <QPushButton>
# include <QTextEdit>
# include <QVBoxLayout>
# include <QFormLayout>
# include <QHeaderView>
# include <QCheckBox>
# include <QLabel>
# include <QSpinBox>
# include <QAction>
# include <QFileDialog>

# include "InternalsCore.hh"
# include "CallGraphLevelView.h"

namespace BinSlay {
  namespace Gui {

    class OpenBinaryView : public QDialog
    {
      Q_OBJECT

      public:
      OpenBinaryView(BinSlay::InternalsCore &core,
		     BinSlay::Gui::CallGraphLevelView &cg_view);

    private slots:
      void open_binary_1();
      void open_binary_2();
      void load_binaries();

    private:
      void createOpenGroupBox();

    private:
      BinSlay::InternalsCore &_core;
      BinSlay::Gui::CallGraphLevelView &_cg_view;

      QGroupBox *openGroupBox;
      QPushButton *loadButton;
      QPushButton *bin1Button;
      QPushButton *bin2Button;
      QLineEdit *bin1lineEdit;
      QLineEdit *bin2lineEdit;
    };    
  }
}

#endif /* !OPEN_BINARY_VIEW_H */
