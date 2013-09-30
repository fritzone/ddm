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
class ReverseEngineerer;
class View;
class Procedure;
class Function;
class GuiElements;
class ConnectionGuiElements;
class ObjectWithUid;
class Version;
class RepositoryGuiElements;
class MainWindowSlotHouse;
class DeleteHelper;

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    typedef bool (MainWindow::*dynamicAction)(QString);
    typedef void (MainWindow::*showSomething)(Version*, const QString&, bool);
    typedef void (MainWindow::*deleteSomething)();

    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool onSaveDiagram(Diagram*, Version *v);

    void instantiateTableCallback(const QString& tabName);
    void specificDeploymentCallback(const QString& connName);

    TableInstance* instantiateTable(const QString& tabName, QStringList otherTablesBeingInstantiated, Version* v, bool ref = false, Table* referencingTable = 0, TableInstance* becauseOfThis = 0);

    void showNothing(Version* v, const QString& , bool focus = true);
    void showDataType(Version* v, const QString &name, bool focus = true);

    void showTableWithGuid(Version* v, const QString& guid, bool focus = true);
    void showTableInstanceWithGuid(Version* v, const QString& guid, bool focus = true);
    void showDiagramWithGuid(Version* v, const QString& guid, bool focus = true);
    void showViewWithGuid(Version* v, const QString& guid, bool focus = true);
    void showProcedureWithGuid(Version* v, const QString& guid, bool focus = true);
    void showFunctionWithGuid(Version* v, const QString& guid, bool focus = true);
    void showTriggerWithGuid(Version* v, const QString& guid, bool focus = true);

    void rerenderQuery(Query*);
    void showConnections();
    void showRepository();
    static MainWindow* instance()
    {
        return m_instance;
    }
    void doLoadSolution(const QString& fileName, bool splashVisible);
    void createStatusLabel();
    void setStatus(const QString& s, bool err);
    ConnectionGuiElements* getConnectionGuiElements() const
    {
        return m_connectionGuiElements;
    }
    GuiElements* getGuiElements() const
    {
        return m_guiElements;
    }

    virtual void setCentralWidget(QWidget *widget);
    void finallyDoLockLikeOperation(bool reLocking, const QString& guid);

    void createPatchElement(Version *v, ObjectWithUid*, const QString &guid, bool reLocking);
    void updatePatchElementToReflectState(Version *v, ObjectWithUid*, const QString &guid, int state);

    virtual void keyPressEvent(QKeyEvent *e);

    void showDataTypesList(Version *foundVersion);

    void createTableInConnection(Connection* c, bool alreadyTried);

    void tryBrowseConnection(Connection* c);

    void updateSourceCodeWidget();

    void newDiagramInVersion(Version* v);
    void newTableInVersion(Version* v);
protected:
    virtual void closeEvent( QCloseEvent * event );
    virtual void changeEvent(QEvent *e);
    virtual bool eventFilter(QObject *obj, QEvent *evt);

public slots:

    void onNewSolution();
    void onNewDataType();
    void onNewTable();
    void onSaveSolution();
    void onOpenSolution();
    void onAbout();
    void onNewDiagram();
    void onSaveAs();
    void onCloseSolution();
    void onDestroyed();
    void onHelp();
    void projectTreeItemClicked(QTreeWidgetItem * current, int);
    void onNewTableInstance();
    void onNewTableInstanceFromPopup();
    void onNewTableInstanceHovered();
    void onDeleteTableFromPopup();
    void onTableAddColumnFromPopup();
    void onSpecializeTableFromPopup();
    void onInstantiateTableFromPopup();
    void onDuplicateTableFromPopup();
    void onCopyTableFromPopup();
    void onPasteTableFromPopup();
    void onDeleteTableInstanceFromPopup();
    void onRenameInstanceFromPopup();
    void onDeleteDatatypeFromPopup();
    void onDuplicateDatatypeFromPopup();
    void onRenameDiagramFromPopup();
    void onNewTriggerFromPopup();
    void onNewViewFromPopup();
    void onNewViewWithSqlFromPopup();
    void onInitiatePatch();
    void onPreferences();
    void onViewProjectTree();
    void onViewConnectionsTree();
    void onViewDatatypesTree();
    void onNewView();
    void onNewViewWithSql();
    void onNewTrigger();
    void onReverseEngineerWizardNextPage(int);
    void onReverseEngineerWizardAccept();
    void onGotoIssueLocation();
    void onIgnoreIssue();
    void onIgnoreIssuesOfATable();
    void onValidate();
    void onNewConnection();
    void onConnectConnection();
    void onSqlQueryInConnection();
    void onBrowseConnection();
    void onRefreshConnection();
    void onDropConnection();
    void onDeleteConnection();
    void onEditConnection();
    void onRecreateConnection();
    void onDeploymentFinished(Deployer*);
    void onReverseEngineeringFinished(ReverseEngineerer*);
    void onDeployHovered();
    void onConnectionItemDoubleClicked(QTreeWidgetItem*,int);
    void onInjectBrowsedTable();
    void onBrowseBrowsedTable();
    void onDeleteUnusedDatatypes();
    void onReleaseMajorVersion();
    void patchTreeItemClicked(QTreeWidgetItem * current, int);
    void finalizePatch();
    void renamePatch();
    void onUndeleteSomething();
    void onCompareTables();

    void onUpdateDb();
    void onRepoItemClicked(QTreeWidgetItem*,int);
    void onShowSolutionProperties();
    void onConnectionCreateTable();

    // called from the constructed DB menu
    void onActionTriggered();

private:

    void setupGuiForNewSolution();
    void populateTreeWithSolution(Solution* sol);
    void saveProject(bool saveAs = false);
    void enableActions();
    void disableActions();
    void connectActionsFromPopupMenus();
    void freeGuiElements();
    void showProjectDetails();
    void showNewDataTypeWindow(int, Version *v);
    NewTableForm* showExistingTable(Table*, Version *v);

    Connection* getRightClickedConnection();
    Version* getRightClickedVersion();
    template <class T> T* getRightClickedObject();
    template <class T> void showNamedObjectList(showSomething s, const QVector<T*> items, const QIcon& icon, const QString& title);

    void hideSplashwindow();
    void showSplashwindow();

    void showObjectWithGuidAndVersion(Version* v, QString uid, showSomething s, bool focus=true);

    void doLockLikeOperation(bool reLocking);
    ContextMenuEnabledTreeWidgetItem* getLastRightClickedTreeItem();
    Version* getVersionOfLastRightClickedElement();
    bool showObjectWithGuid(const QString&);
    bool showRepoObjectWithGuid(const QString&);
    Function *createStoredFunction(bool guided, Version *v);
    Procedure* createStoredProcedureInVersion(bool guided, Version *v);
    void createTableInstancesInVersion(Version* v);
    Version* getLastRightClickedVersion();

private:

    /**
     * @brief The ClassUidsShowMethod class holds a map for the class UID and the
     * correspondng showSomething method
     */
    class ClassUidsShowMethod
    {
    public:
        ClassUidsShowMethod();
        const QMap<QString, showSomething>& getMapping()
        {
            return mapping;
        }
    private:
        QMap<QString, showSomething> mapping;
    };

    Ui::MainWindow *m_ui;
    MainWindowButtonDialog* m_btndlg;
    Workspace* m_workspace;
    ReverseEngineerWizard* m_revEngWizard;
    NewViewForm* m_nvf;
    QLabel* lblStatus;
    static MainWindow* m_instance;
    bool m_splashWasVisible;
    GuiElements* m_guiElements;
    ConnectionGuiElements* m_connectionGuiElements;
    RepositoryGuiElements* m_repositoryGuiElements;
    QString m_lastLoadedProject;
    ClassUidsShowMethod m_showMethods;

    friend class MainWindowSlotHouse;
    friend class DeleteHelper;
};

#endif // MAINWINDOW_H
