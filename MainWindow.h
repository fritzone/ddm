#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidget>
#include <QLabel>

#include "commons.h"

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
class Procedure;
class GuiElements;
class ConnectionGuiElements;

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    typedef bool (MainWindow::*dynamicAction)(QString);
    typedef void (MainWindow::*showSomething)(const QString&, bool);

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
    void showDiagram(const QString& name, bool focus = true);
    void showView(const QString& viewName, bool focus = true);
    void showProcedure(const QString& procName, bool focus = true);
    void showFunction(const QString& funcName, bool focus = true);
    void showTrigger(const QString& triggerName, bool focus = true);

    void rerenderQuery(Query*);
    void showConnections();
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
    void onNewTrigger();
    void onNewFunction();

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
    void onDropConnection();
    void onDeleteConnection();
    void onEditConnection();
    void onRecreateConnection();
    void onDeploymentFinished(Deployer*);
    void onSqlGenerationFinished(InjectSqlGenerator*);
    void onReverseEngineeringFinished(ReverseEngineerer*);
    void onDeployHovered();
    void onConnectionItemDoubleClicked(QTreeWidgetItem*,int);
    void onInjectBrowsedTable();
    void onBrowseBrowsedTable();
    void onDeleteView();
    void onDeleteUnusedDatatypes();
    void onDeleteProcedure();

private:

    void setupGuiForNewSolution();
    void populateTreeWithSolution(Solution* sol);
    void saveProject(bool saveAs = false);
    void enableActions();
    void connectActionsFromPopupMenus();
    void freeGuiElements();
    void showProjectDetails();
    void showNewDataTypeWindow(int);
    NewTableForm* showExistingTable(Table*);
    UserDataType* getRightClickedDatatype();
    Connection* getRightClickedConnection();

    template <class T> T* getRightClickedObject(itemGetter);
    template <class T> T* getNamedObject(QTreeWidgetItem*, itemGetter);
    template <class T> void showNamedObjectList(showSomething s, const QVector<T*> items, const QIcon& icon, const QString& title);

    void doDeployment(const QString& codePage, QStringList connectionNames);
    void createConnectionTreeEntryForTables(Connection *c);
    void createConnectionTreeEntryForViews(Connection *c);
    void createConnectionTreeEntryForProcs(Connection *c);
    void createConnectionTreeEntryForFuncs(Connection *c);
    void createConnectionTreeEntryForTriggers(Connection *c);
    void hideSplashwindow();
    void tryBrowseConnection(Connection* c);
    void showNamedObject(QTreeWidgetItem* current, showSomething s, bool focus=true);


private:
    Ui::MainWindow *m_ui;
    MainWindowButtonDialog* m_btndlg;

    Workspace* m_workspace;

    ReverseEngineerWizard* m_revEngWizard;

    NewViewForm* m_nvf;
    QVector<Deployer*> m_deployers;
    QLabel* lblStatus;
    static MainWindow* m_instance;
    bool m_splashWasVisible;
    GuiElements* m_guiElements;
    ConnectionGuiElements* m_connectionGuiElements;
};

#endif // MAINWINDOW_H
