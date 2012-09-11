#ifndef SHOW_FUNCTION_VIEW_H
# define SHOW_FUNCTION_VIEW_H

# include <QtGui>
# include "InternalsCore.hh"

namespace BinSlay {
  namespace Gui {
    class ShowFunctionView : public QDialog
    {
      Q_OBJECT

      public:
      ShowFunctionView(BinSlay::InternalsCore &int_core, BinSlay::Function *fct);

    private slots:
      void cfg_to_dot_format();
      void doShowCFG();
      void doDisasembly();

    protected:
      void closeEvent(QCloseEvent*);

    private:
      BinSlay::InternalsCore	&_internals_core;
      BinSlay::Function		*_function;

      void createInfoGroupBox();
      void createDisassemblyView();
      void createDisassemblyGroupBox();

      QGroupBox *infoGroupBox;
      QGroupBox *disassemblyGroupBox;

      QTableWidget *disassemblyView;

      QLineEdit *addrLineEdit;
      QLineEdit *nameLineEdit;
      QLineEdit *nbBasicBlocksLineEdit;
      QLineEdit *nbEdgesLineEdit;
      QLineEdit *nbCallsOutLineEdit;
      QLineEdit *nbCallsInLineEdit;

      QPushButton *disassemblyButton;
      QPushButton *cfgDotFormatButton;
      QPushButton *showCFGButton;

      QTextEdit	*disassemblyTextEdit;
    };    
  }
}

#endif /* !SHOW_FUNCTION_VIEW_H */
