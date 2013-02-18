#ifndef SHOW_BASIC_BLOCK_VIEW_H
# define SHOW_BASIC_BLOCK_VIEW_H

# include <QDialog>
# include <QGroupBox>
# include <QTableWidget>
# include <QLineEdit>
# include <QPushButton>
# include <QTextEdit>
# include <QVBoxLayout>
# include <QFormLayout>
# include <QHeaderView>
# include "InternalsCore.hh"

namespace BinSlay {
  namespace Gui {
    class ShowBasicBlockView : public QDialog
    {
      Q_OBJECT

      public:
      ShowBasicBlockView(BinSlay::InternalsCore &int_core, BinSlay::BasicBlock *bb);

    private slots:
      void doDisasembly();

    protected:
      void closeEvent(QCloseEvent*);

    private:
      BinSlay::InternalsCore	&_internals_core;
      BinSlay::BasicBlock	*_basic_block;

      void createInfoGroupBox();
      void createDisassemblyGroupBox();
      void createDisassemblyView();

      QGroupBox *infoGroupBox;
      QGroupBox *disassemblyGroupBox;

      QTableWidget *disassemblyView;

      QLineEdit *addrLineEdit;
      QLineEdit *nbInstrLineEdit;
      QLineEdit *nbCallsOutLineEdit;

      QPushButton *disassemblyButton;
      QPushButton *cfgDotFormatButton;

      QTextEdit	*disassemblyTextEdit;
    };    
  }
}

#endif /* !SHOW_BASIC_BLOCK_VIEW_H */
