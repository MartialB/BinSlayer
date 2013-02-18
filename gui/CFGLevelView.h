#ifndef CFGLEVELVIEW_H
# define CFGLEVELVIEW_H

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

# include "InternalsCore.hh"
# include "BinDiff.hh"

namespace BinSlay {
  namespace Gui {
    class CFGLevelView : public QDialog
    {
      Q_OBJECT

      public:
      CFGLevelView(BinSlay::InternalsCore &int_core, BinSlay::CFG_Core &core);
      ~CFGLevelView();									     

      enum view_type {
      	ISO_LIST_VIEW = 0,
      	EDIT_PATH_VIEW,
	MATCHED_VIEW,
	UNMATCHED_VIEW
      };


    private slots:
      void doEditBindiffSelector(bool state);
      void doEditCrc32Selectors(bool state);
      void run_bindiff();
      void re_run_bindiff();
      void reset_bindiff();
      void compute_ged();
      void reset_ged();
      void doEditUpProperty(bool);
      void doEditDownProperty(bool);
      void doEditCfgLevel(bool);
      //      void doEditBbLevel(bool);

      void doShowBasicBlock_fromIsoList();
      void doShowBasicBlock_fromEditPath();
      void doShowBasicBlock_fromMatched();
      void doShowBasicBlock_fromUnMatched();

      void doDiffBasicBlocks_fromIsoList();
      void doDiffBasicBlocks_fromEditPath();
      void doDiffBasicBlocks_fromMatched();
      void doDiffBasicBlocks_fromUnMatched();

      void doAddToMatchedBlocks();
      void doDeleteFromMatchedBlocks();

    protected:
      void closeEvent(QCloseEvent* event);

    private:
      BinSlay::InternalsCore &_internals_core;
      BinSlay::CFG_Core	&_core;

      void showBasicBlock(view_type);
      void diffBasicBlocks(view_type);

      void createConfigGroupBox();
      void createEuclidianDistanceGroupBox();
      void createGedGroupBox();
      void createResGroupBox();
      void createIsoListView();
      void createEditPathView();
      void createMatchedBasicBlocksView();
      void createUnMatchedBasicBlocksView();
      void createBindiffGroupBox();

      void _display_bindiff_results();

      QGroupBox *configGroupBox;
      QGroupBox *euclidianDistanceGroupBox;
      QGroupBox *gedGroupBox;
      QGroupBox *resGroupBox;
      QGroupBox *bindiffGroupBox;

      QTabWidget *tabResults;
      QTableWidget *isoListView;
      QTableWidget *editPathView;
      QTableWidget *matchedBasicBlocksView;
      QTableWidget *unmatchedBasicBlocksView;

      enum idSelectors { name = 0, crc32, cfg };
      enum idProperties { up = 0, down };
      enum idLevels { fct = 0, fcfg, bb };

      QCheckBox *checkBoxSelectors[3];
      QCheckBox *checkBoxProperties[2];
      QCheckBox *checkBoxLevels[3];
      QLabel *eucDistLabel;
      QSpinBox *eucDistSpinBox;
      QPushButton *gedRunButton;
      QPushButton *reRunButton;
      QPushButton *gedResetButton;
      QLineEdit *gedLineEdit;
      QLineEdit *bindiffLineEdit;
      QPushButton *runButton;
      QPushButton *resetButton;
      QLineEdit *dotBin1LineEdit;
      QLineEdit *dotBin2LineEdit;
    };    
  }
}

#endif /* !CFGLEVELVIEW_H */
