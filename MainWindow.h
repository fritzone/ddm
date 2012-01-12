#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidget>
#include <QLabel>

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
class Query;
class NewViewForm;
class Connection;
class Deployer;
class InjectSqlGenerator;
class ReverseEngineerer;
class View;

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    typedef bool (MainWindow::*dynamicAction)(QString);

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

    bool onSaveDiagram(Diagram*);

    void instantiateTableCallback(const QString& tabName);
    void specificDeploymentCallback(const QString& connName);

    ContextMenuEnabledTreeWidgetItem* instantiateTable(const QString& tabName, QStringList otherTablesBeingInstantiated, bool ref = false, Table* referencingTable = 0, TableInstance* becauseOfThis = 0);

    void showTable(const QString& tabName, bool focus = true);
    void showTableInstance(const QString& tabName, bool focus = true);
    void showDataType(const QString &name, bool focus = true);
    void showDiagram(const QString& name);
    void showView(const QString& viewName);
    void showProc(const QString& procName);

    void rerenderQuery(Query*);
    void showConnections();
    static MainWindow* instance()
    {
        return m_instance;
    }
    void doLoadSolution(const QString& fileName, bool splashVisible);
protected:

    void changeEvent(QEvent *e);

public slots:

    void onNewSolution();
    //void onProjectTreeClicked();
    void onNewDataType();
    void onNewTable();
    void onSaveSolution();
    void onOpenSolution();
    void onAbout();
    void onDTTreeClicked();
    void onNewDiagram();
    void onSaveAs();
    void onCloseSolution();
    void currentProjectTreeItemChanged(QTreeWidgetItem * current, QTreeWidgetItem*);

    void dtTreeItemClicked ( QTreeWidgetItem * item, int column );
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
    void onViewConnectionsTree();
    void onViewDatatypesTree();
    void onNewView();
    void onNewViewWithSql();
    void onNewProcedure();

    void onReverseEngineerWizardNextPage(int);
    void onReverseEngineerWizardAccept();

    void onGotoIssueLocation();
    void onIgnoreIssue();
    void onIgnoreIssuesOfATable();

    void onValidate();
    void onNewConnection();
    void onConnectConnection();
    void onBrowseConnection();
    void onDropConnection();
    void onDeleteConnection();
    void onEditConnection();
    void onDeploymentFinished(Deployer*);
    void onSqlGenerationFinished(InjectSqlGenerator*);
    void onReverseEngineeringFinished(ReverseEngineerer*);
    void onDeployHovered();
    void onConnectionItemDoubleClicked(QTreeWidgetItem*,int);
    void onInjectBrowsedTable();
    void onBrowseBrowsedTable();
    void onDeleteView();
    void onDeleteUnusedDatatypes();

public:
    void createStatusLabel();
    void setStatus(const QString& s, bool err);
private:

    void setupGuiForNewSolution();
    void populateTreeWithSolution(Solution* sol);
    void saveProject(bool saveAs = false);
    void enableActions();
    void connectActionsFromPopupMenus();
    Table* getRightclickedTable();
    TableInstance* getRightclickedTableInstance();
    UserDataType* getRightclickedDatatype();
    Connection* getRightclickedConnection();
    Diagram* getRightclickedDiagram();
    ContextMenuEnabledTreeWidgetItem* createDataTypeTreeEntry(UserDataType*);
    ContextMenuEnabledTreeWidgetItem* createConnectionTreeEntry(Connection*);
    void showButtonDialog();
    void freeGuiElements();
    void showProjectDetails();
    void showNewDataTypeWindow(int);
    NewTableForm* showExistingTable(Table*);
    View* getRightclickedView();

    void doDeployment(const QString& codePage, QStringList connectionNames);
    void createConnectionTreeEntryForTables(Connection *c);

private:
    Ui::MainWindow *m_ui;

    QDockWidget* m_projectTreeDock;
    QDockWidget* m_datatypesTreeDock;
    QDockWidget* m_issuesTreeDock;
    QDockWidget* m_connectionsTreeDock;

    ContextMenuEnabledTreeWidget* m_projectTree;
    ContextMenuEnabledTreeWidget* m_datatypesTree;
    ContextMenuEnabledTreeWidget* m_issuesTree;
    ContextMenuEnabledTreeWidget* m_connectionsTree;

    ContextMenuHandler* m_issuesContextMenuHandler;
    ContextMenuHandler* m_connectionsContextMenuHandler;

    // the initial three button dialog
    MainWindowButtonDialog* m_btndlg;

    Workspace* m_workspace;

    ReverseEngineerWizard* m_revEngWizard;

    NewViewForm* m_nvf;
    ContextMenuHandler* m_contextMenuHandler;
    QVector<Deployer*> m_deployers;
    QLabel* lblStatus;
    static MainWindow* m_instance;
    bool m_splashWasVisible;
};

#endif // MAINWINDOW_H
