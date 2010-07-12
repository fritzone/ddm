#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui/QTreeWidget>

class Project;
class MainWindowButtonDialog;
class DataTypesListForm;
class NewDataTypeForm;
class AbstractDTSupplier;
class UserDataType;
class Table;
class Solution;
class Diagram;
class ContextMenuEnabledTreeWidget;

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    /**
     * Getting here from the NewDataTypeForm, onSave
     */
    bool onSaveNewDataType(const QString& name, const QString& type,
                             const QString& sqlType, const QString& size,
                             const QString& defaultValue, const QString& cp,
                             const QStringList& mvs, const QString& desc,
                             bool unsi, bool canBeNull, bool autoInc, UserDataType* pudt);

    Project* getWorkingProject();

    /**
     * This methid gets called when a new table is saved
     */
    bool onSaveNewTable(Table* tbl);

    /**
     * This method gets called when a table is updated. The only thing this must do
     * for now is to update the tree entry in the project tree and the associated tree widget
     */
    bool onUpdateTable(Table* tbl);

    Solution* currentSolution();

    bool onSaveDiagram(Diagram*);

public slots:

    void onNewSolution();
    void onProjectTreeClicked();
    void onNewDataType();
    void onNewTable();
    void onSaveProject();
    void onOpenProject();
    void onAbout();
    void onDTTreeClicked();
    void onNewDiagram();
    void onSaveAs();

    // when the user clicked the "Delete table" from the popups
    void onDeleteTableFromPopup();
    void onTableAddColumnFromPopup();

private:

    ContextMenuEnabledTreeWidget* setupGuiForNewSolution();
    void populateTreeWithSolution(Solution* sol);
    void saveProject(bool saveAs = false);
    // to create the other dialogs needed in the app
    void createOtherDialogs();

    void enableActions();
    void connectActionsFromTablePopupMenu();

private:
    Ui::MainWindow *ui;

    // the dock window in which the project tree is to be found
    QDockWidget* dock;

    // the dock widget for the data types
    QDockWidget* dockdt;

    // the tree which contains everything related to the current project
    ContextMenuEnabledTreeWidget* projectTree;
    // the tree containing the data types
    ContextMenuEnabledTreeWidget* dataypesTree;

    // the initial three button dialog
    MainWindowButtonDialog* btndlg;

    // true if we are working on a project
    bool weHaveProject;

    QVector<Solution*> m_solutions;
    Solution* m_currentSolution;

};

#endif // MAINWINDOW_H
