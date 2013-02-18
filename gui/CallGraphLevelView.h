#ifndef CALLGRAPHLEVELVIEW_H
# define CALLGRAPHLEVELVIEW_H

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
# include <QDialogButtonBox>
# include "InternalsCore.hh"

namespace BinSlay {
  namespace Gui {
    class CallGraphLevelView : public QDialog
    {
      Q_OBJECT

      public:
      CallGraphLevelView(BinSlay::InternalsCore &int_core, BinSlay::CG_Core *cg_core);
      void set_CG_Core(BinSlay::CG_Core *cg_core);
      void reset();

      enum view_type {
      	ISO_LIST_VIEW = 0,
      	EDIT_PATH_VIEW,
	MATCHED_VIEW,
	UNMATCHED_VIEW
      };

    private slots:
      void doEditBindiffSelector(bool state);
      void doEditCrc32Selectors(bool state);
      void doEditSymNameSelectors(bool state);
      void run_bindiff();
      void re_run_bindiff();
      void reset_bindiff();
      void compute_ged();
      void reset_ged();
      void cg_bin1_to_dot_format();
      void cg_bin2_to_dot_format();
      void doEditUpProperty(bool);
      void doEditDownProperty(bool);
      void doEditCgLevel(bool);
      void doEditCfgLevel(bool);
      //      void doEditBbLevel(bool);

      void doDiffFunctions_fromIsoList();
      void doDiffFunctions_fromEditPath();
      void doDiffFunctions_fromMatched();
      void doDiffFunctions_fromUnMatched();

      void doShowFunction_fromIsoList();
      void doShowFunction_fromEditPath();
      void doShowFunction_fromMatched();
      void doShowFunction_fromUnMatched();

      void doAddToMatchedFunctions();

    private:
      BinSlay::InternalsCore	&_internals_core;
      BinSlay::CG_Core		*_core;

      void createConfigGroupBox();
      void createEuclidianDistanceGroupBox();
      void createGedGroupBox();
      void createResGroupBox();
      void createIsoListView();
      void createEditPathView();
      void createMatchedFunctionsView();
      void createUnMatchedFunctionsView();
      void createBindiffGroupBox();

      void diffFunctions(view_type);
      void showFunction(view_type);

      void _display_bindiff_results();

      enum {
      	NumGridRows = 3,
      	NumButtons = 4
      };

      QGroupBox *openGroupBox;
      QGroupBox *configGroupBox;
      QGroupBox *euclidianDistanceGroupBox;
      QGroupBox *gedGroupBox;
      QGroupBox *resGroupBox;
      QGroupBox *bindiffGroupBox;

      QTabWidget *tabResults;
      QTableWidget *isoListView;
      QTableWidget *editPathView;
      QTableWidget *matchedFunctionsView;
      QTableWidget *unmatchedFunctionsView;

      enum idSelectors { name = 0, crc32, cfg };
      enum idProperties { up = 0, down };
      enum idLevels { fct = 0, fcfg, bb };

      QCheckBox *checkBoxSelectors[3];
      QCheckBox *checkBoxProperties[2];
      QCheckBox *checkBoxLevels[3];
      QLabel *eucDistLabel;
      QSpinBox *eucDistSpinBox;
      QPushButton *gedRunButton;
      QPushButton *gedResetButton;
      QLineEdit *gedLineEdit;
      QLineEdit *bindiffLineEdit;
      QPushButton *runButton;
      QPushButton *reRunButton;
      QPushButton *resetButton;
      QPushButton *callGraphBin1;
      QPushButton *callGraphBin2;

      QLabel *nbFct1Label;
      QLabel *nbFct2Label;
      QLabel *nbIsoFoundLabel;
      QLabel *percentageLabel;

      QLabel *labels[NumGridRows];
      QLineEdit *lineEdits[NumGridRows];
      QPushButton *buttons[NumButtons];
      QDialogButtonBox *buttonBox;

      QAction *exitAction;
    };    
  }
}

#endif /* !CALLGRAPHLEVELVIEW_H */
