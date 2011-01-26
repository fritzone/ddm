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
class TableInstance;
class Diagram;
class ContextMenuEnabledTreeWidget;
class ContextMenuEnabledTreeWidgetItem;
class NewTableForm;
class NewProjectDialog;
class Solution;
class Workspace;
class ReverseEngineerWizard;
class ContextMenuHandler;
class Issue;

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

    /**
     * This methid gets called when a new table is saved
     */
    bool onSaveNewTable(Table* tbl);

    /**
     * This method gets called when a table is updated. The only thing this must do
     * for now is to update the tree entry in the project tree and the associated tree widget
     */
    bool onUpdateTable(Table* tbl);

    bool onSaveDiagram(Diagram*);

    ContextMenuEnabledTreeWidgetItem* instantiateTable(const QString& tabName, bool ref = false, Table* referencingTable = 0, TableInstance* becauseOfThis = 0);

    void showTable(const QString& tabName, bool focus = true);
    void showTableInstance(const QString& tabName, bool focus = true);
    void showDataType(const QString &name, bool focus = true);

public slots:

    void onNewSolution();
    //void onProjectTreeClicked();
    void onNewDataType();
    void onNewTable();
    void onSaveProject();
    void onOpenProject();
    void onAbout();
    void onDTTreeClicked();
    void onNewDiagram();
    void onSaveAs();
    void onCloseSolution();
    void currentProjectTreeItemChanged(QTreeWidgetItem * current, QTreeWidgetItem*);

    void dtTreeItemClicked ( QTreeWidgetItem * item, int column );
    void projectTreeItemClicked ( QTreeWidgetItem * item, int column );
    void onNewTableInstance();
    void onNewTableInstanceHovered();
    void onDeleteTableFromPopup();
    void onTableAddColumnFromPopup();
    void onSpecializeTableFromPopup();
    void onInstantiateTableFromPopup();
    void onDuplicateTableFromPopup();
    void onCopyTableFromPopup();
    void onPasteTableFromPopup();
    void onDeleteInstanceFromPopup();
    void onRenameInstanceFromPopup();
    void onDeleteDatatypeFromPopup();
    void onDuplicateDatatypeFromPopup();
    void onDeleteDiagramFromPopup();
    void onRenameDiagramFromPopup();
    void onNewStringType();
    void onNewNumericType();
    void onNewBoolType();
    void onNewDateTimeType();
    void onNewBlobType();
    void onNewMiscType();
    void onNewSpatialType();
    void onDeploy();
    void onPreferences();
    void onViewProjectTree();
    void onViewDatatypesTree();

    void onReverseEngineerWizardNextPage(int);
    void onReverseEngineerWizardAccept();

    void onGotoIssueLocation();
    void onIgnoreIssue();
    void onIgnoreIssuesOfATable();

    void onValidate();

private:

    void setupGuiForNewSolution();
    void populateTreeWithSolution(Solution* sol);
    void saveProject(bool saveAs = false);
    void enableActions();
    void connectActionsFromTablePopupMenu();
    Table* getRightclickedTable();
    TableInstance* getRightclickedTableInstance();
    UserDataType* getRightclickedDatatype();
    Diagram* getRightclickedDiagram();
    ContextMenuEnabledTreeWidgetItem* createDataTypeTreeEntry(UserDataType*);
    void showButtonDialog();
    void freeGuiElements();
    void showProjectDetails();
    void showNewDataTypeWindow(int);
    NewTableForm* showExistingTable(Table*);

private:
    Ui::MainWindow *m_ui;

    QDockWidget* m_projectTreeDock;
    QDockWidget* m_datatypesTreeDock;
    QDockWidget* m_issuesTreeDock;

    ContextMenuEnabledTreeWidget* m_projectTree;
    ContextMenuEnabledTreeWidget* m_datatypesTree;
    ContextMenuEnabledTreeWidget* m_issuesTree;

    ContextMenuHandler* m_issuesContextMenuHandler;

    // the initial three button dialog
    MainWindowButtonDialog* m_btndlg;

    Workspace* m_workspace;

    ReverseEngineerWizard* m_revEngWizard;
};

#endif // MAINWINDOW_H
