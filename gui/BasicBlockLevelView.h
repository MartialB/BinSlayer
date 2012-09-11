#ifndef BASIC_BLOCK_LEVEL_VIEW_H
# define BASIC_BLOCK_LEVEL_VIEW_H

# include <QtGui>
# include "InternalsCore.hh"

namespace BinSlay {
  namespace Gui {
    class BasicBlockLevelView : public QDialog
    {
      Q_OBJECT

      public:
      BasicBlockLevelView(BinSlay::InternalsCore &int_core, BinSlay::BasicBlock_Core &core);
      ~BasicBlockLevelView();

    private slots:
      void doDiff();
      void doResetDiff();

    protected:
      void closeEvent(QCloseEvent* event);

    private:
      BinSlay::InternalsCore	&_internals_core;
      BinSlay::BasicBlock_Core	&_bb_core;

      void createButtonsGroupBox();
      void createDisassemblyGroupBox();

      QGroupBox *buttonsGroupBox;
      QGroupBox *disassemblyGroupBox;

      QTableWidget *disassemblyLeftView;
      QTableWidget *disassemblyRightView;

      QPushButton *diffButton;
      QPushButton *resetButton;
    };    
  }
}

#endif /* !BASIC_BLOCK_LEVEL_VIEW_H */
