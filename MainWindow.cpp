#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "MainWindowButtonDialog.h"
#include "NewProjectDialog.h"
#include "DataTypesListForm.h"
#include "NewDataTypeForm.h"
#include "UserDataType.h"
#include "Project.h"
#include "db_DatabaseEngine.h"
#include "Version.h"
#include "db_AbstractDTSupplier.h"
#include "NewTableForm.h"
#include "Table.h"
#include "Column.h"
#include "IconFactory.h"
#include "Configuration.h"
#include "TablesListForm.h"
#include "ProjectDetailsForm.h"
#include "Solution.h"
#include "DeserializationFactory.h"
#include "AboutBoxDialog.h"
#include "DiagramForm.h"
#include "Diagram.h"
#include "ContextMenuEnabledTreeWidget.h"
#include "CreateTableInstancesDialog.h"
#include "TableInstanceForm.h"
#include "DynamicActionHandlerForMainWindow.h"
#include "SqlForm.h"
#include "TableInstance.h"
#include "TableInstancesListForm.h"
#include "PreferencesDialog.h"
#include "SimpleTextInputDialog.h"
#include "ContextMenuCollection.h"
#include "strings.h"
#include "Workspace.h"
#include "VersionGuiElements.h"
#include "InjectSqlDialog.h"
#include "ReverseEngineerWizard.h"
#include "DataType.h" // TODO: this is simply bad design, Mainwindow should not know about datatypes ...
#include "IssueManager.h"
#include "NewViewForm.h"
#include "qbr_SelectQuery.h"
#include "core_View.h"
#include "gui_HelpWindow.h"
#include "core_Connection.h"
#include "core_ConnectionManager.h"
#include "core_Deployer.h"
#include "core_InjectSqlGenerator.h"
#include "core_ReverseEngineerer.h"
#include "helper_MostRecentlyUsedFiles.h"
#include "BrowseTableForm.h"

#include <QtGui>

MainWindow* MainWindow::m_instance = 0;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), m_ui(new Ui::MainWindow), m_projectTreeDock(0), m_datatypesTreeDock(0), m_issuesTreeDock(0), m_connectionsTreeDock(0),
        m_projectTree(0), m_datatypesTree(0), m_issuesTree(0), m_connectionsTree(0), m_btndlg(0), m_workspace(0), m_revEngWizard(0), m_nvf(0), m_contextMenuHandler(0), lblStatus(0)
{
    m_ui->setupUi(this);

    showButtonDialog();
    Configuration::instance();
    ConnectionManager::instance();

    // set up the tree
    m_contextMenuHandler = new ContextMenuHandler();
    m_workspace = Workspace::getInstance();
    m_instance = this;
}

MainWindow::~MainWindow()
{
    ConnectionManager::instance()->shutDown();
    delete m_ui;
}

void MainWindow::showButtonDialog()
{
    m_btndlg = new MainWindowButtonDialog();
    m_btndlg->setMainWindow(this);
    m_btndlg->setModal(false);
    m_btndlg->setWindowFlags(Qt::SplashScreen | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);

    setWindowTitle(tr("DDM - [No Solution]"));
    showConnections();
    showMaximized();
    QDesktopWidget *d = QApplication::desktop();

    QRect t = d->availableGeometry(this);
    m_btndlg->move(mapToGlobal(this->geometry().topLeft()).x() + t.center().x() - m_btndlg->width() / 2, t.center().y()- m_btndlg->height() / 2);
    m_btndlg->show();

}

void MainWindow::freeGuiElements()
{
    if(m_projectTree)
    {
        delete m_projectTree;
        m_projectTree = 0;
    }

    if(m_datatypesTree)
    {
        delete m_datatypesTree;
        m_datatypesTree = 0;
    }

    if(m_issuesTreeDock)
    {
        delete m_issuesTreeDock;
        m_issuesTreeDock = 0;
    }

    if(m_projectTreeDock)
    {
        delete m_projectTreeDock;
        m_projectTreeDock = 0;
    }

    if(m_datatypesTreeDock)
    {
        delete m_datatypesTreeDock;
        m_datatypesTreeDock = 0;
    }
}

void MainWindow::showProjectDetails()
{
    ProjectDetailsForm* prjDetailsForm = new ProjectDetailsForm(this);
    prjDetailsForm->setProject(m_workspace->currentProject());
    setCentralWidget(prjDetailsForm);
}

void MainWindow::showConnections()
{
    if(m_connectionsTreeDock)
    {
        onViewConnectionsTree();
        return;
    }
    // create the dock window
    m_connectionsTreeDock = new QDockWidget(tr("Connections"), this);
    m_connectionsTreeDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_connectionsTreeDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
    m_connectionsTreeDock->setFloating(false);
    m_connectionsTreeDock->setMinimumSize(300, 340);
    m_connectionsTreeDock->setMaximumSize(500, 9999);
    m_connectionsTreeDock->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum));
    m_connectionsTreeDock->resize(301,341);

    m_connectionsTree = new ContextMenuEnabledTreeWidget();
    m_connectionsTree->setAllColumnsShowFocus(true);
    m_connectionsTree->setSelectionMode(QAbstractItemView::SingleSelection);
    m_connectionsTree->setSelectionBehavior(QAbstractItemView::SelectItems);
    m_connectionsTree->setItemDelegate(new ContextMenuDelegate(m_contextMenuHandler,m_projectTree));
    m_connectionsTree->setColumnCount(1);
    m_connectionsTree->setHeaderHidden(false);

    QTreeWidgetItem *___qtreewidgetitem = m_connectionsTree->headerItem();
    ___qtreewidgetitem->setText(0, QApplication::translate("MainWindow", "Connection", 0, QApplication::UnicodeUTF8));
    m_connectionsTree->header()->setDefaultSectionSize(150);

    m_connectionsContextMenuHandler = new ContextMenuHandler();
    m_connectionsTree->setItemDelegate(new ContextMenuDelegate(m_connectionsContextMenuHandler, m_connectionsTree));
    m_connectionsTreeDock->setWidget(m_connectionsTree);
    addDockWidget(Qt::RightDockWidgetArea, m_connectionsTreeDock);

    m_ui->action_NewDatabaseConnection->setEnabled(true);
    const QVector<Connection*>& cons = ConnectionManager::instance()->connections();
    for(int i=0; i<cons.size(); i++)
    {
        Connection* c = cons.at(i);
        createConnectionTreeEntry(c);
    }
    QObject::connect(ContextMenuCollection::getInstance()->getAction_ConnectionConnect(), SIGNAL(activated()), this, SLOT(onConnectConnection()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_ConnectionBrowse(), SIGNAL(activated()), this, SLOT(onBrowseConnection()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_ConnectionDrop(), SIGNAL(activated()), this, SLOT(onDropConnection()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_ConnectionDelete(), SIGNAL(activated()), this, SLOT(onDeleteConnection()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_ConnectionEdit(), SIGNAL(activated()), this, SLOT(onEditConnection()));
    connect(m_connectionsTree, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(onConnectionItemDoubleClicked(QTreeWidgetItem*,int)));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_BrowsedTableInject(), SIGNAL(activated()), this, SLOT(onInjectBrowsedTable()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_BrowsedTableBrowse(), SIGNAL(activated()), this, SLOT(onBrowseBrowsedTable()));

    m_ui->action_ConnectionsTree->setChecked(true);
}

ContextMenuEnabledTreeWidgetItem* MainWindow::createConnectionTreeEntry(Connection* c)
{
    ContextMenuEnabledTreeWidgetItem* newConnectionItem = new ContextMenuEnabledTreeWidgetItem((ContextMenuEnabledTreeWidgetItem*)0, QStringList(c->getName() + "(" + c->getDb()+"@"+c->getHost() + ")")) ;
    QVariant var(c->getName());
    newConnectionItem->setData(0, Qt::UserRole, var);
    switch(c->getState())
    {
    case Connection::DID_NOT_TRY:
        newConnectionItem->setIcon(0, IconFactory::getDatabaseIcon());
        break;
    case Connection::FAILED:
        newConnectionItem->setIcon(0, IconFactory::getUnConnectedDatabaseIcon());
        break;
    case Connection::CONNECTED:
        newConnectionItem->setIcon(0, IconFactory::getConnectedDatabaseIcon());
        break;
    }
    m_connectionsTree->addTopLevelItem(newConnectionItem);
    newConnectionItem->setPopupMenu(ContextMenuCollection::getInstance()->getConnectionsPopupMenu());
    c->setLocation(newConnectionItem);
    return newConnectionItem ;
}

void MainWindow::setupGuiForNewSolution()
{
    // create the dock window
    m_projectTreeDock = new QDockWidget(tr("Solution - ") + m_workspace->currentSolution()->name(), this);
    m_projectTreeDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_projectTreeDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
    m_projectTreeDock->setFloating(false);
    m_projectTreeDock->setMinimumSize(300, 340);
    m_projectTreeDock->setMaximumSize(500, 9999);
    m_projectTreeDock->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum));
    m_projectTreeDock->resize(301,341);

    m_datatypesTreeDock = new QDockWidget(tr("DataTypes") , this);
    m_datatypesTreeDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_datatypesTreeDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
    m_datatypesTreeDock->setFloating(false);
    m_datatypesTreeDock->setMinimumSize(300, 340);
    m_datatypesTreeDock->setMaximumSize(500, 9999);

    m_issuesTreeDock = new QDockWidget(tr("Issues"), this);
    m_issuesTreeDock->setAllowedAreas(Qt::BottomDockWidgetArea);
    m_issuesTreeDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
    m_issuesTreeDock->setFloating(false);
    m_issuesTreeDock->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum));


    m_projectTree = new ContextMenuEnabledTreeWidget();
    m_projectTree->setAllColumnsShowFocus(true);
    m_projectTree->setSelectionMode(QAbstractItemView::SingleSelection);
    m_projectTree->setSelectionBehavior(QAbstractItemView::SelectItems);
    m_projectTree->setItemDelegate(new ContextMenuDelegate(m_contextMenuHandler,m_projectTree));
    m_projectTree->setColumnCount(1);
    m_projectTree->setHeaderHidden(true);
    QObject::connect(m_projectTree, SIGNAL (currentItemChanged ( QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(currentProjectTreeItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));

    m_datatypesTree = new ContextMenuEnabledTreeWidget();
    m_datatypesTree ->setColumnCount(2);
    QTreeWidgetItem *hdr = m_datatypesTree->headerItem();
    hdr->setText(0, tr("Type"));
    hdr->setText(1, tr("SQL"));
    m_datatypesTree->header()->setDefaultSectionSize(200);
    m_datatypesTree->setItemDelegate(new ContextMenuDelegate(m_contextMenuHandler,m_datatypesTree));
    m_datatypesTree ->setHeaderHidden(false);
    QObject::connect(m_datatypesTree, SIGNAL(itemSelectionChanged()), this, SLOT(onDTTreeClicked()));
    QObject::connect(m_datatypesTree, SIGNAL (itemClicked ( QTreeWidgetItem * , int ) ), this, SLOT(dtTreeItemClicked(QTreeWidgetItem*,int)));

    m_issuesTree = new ContextMenuEnabledTreeWidget();
    m_issuesTree->setObjectName(QString::fromUtf8("m_issuesTree"));
    m_issuesTree->setAnimated(false);
    m_issuesTree->setExpandsOnDoubleClick(true);
    m_issuesTree->header()->setDefaultSectionSize(150);
    QTreeWidgetItem *___qtreewidgetitem = m_issuesTree->headerItem();
    ___qtreewidgetitem->setText(0, QApplication::translate("MainWindow", "", 0, QApplication::UnicodeUTF8));
    ___qtreewidgetitem->setText(1, QApplication::translate("MainWindow", "Type", 0, QApplication::UnicodeUTF8));
    ___qtreewidgetitem->setText(2, QApplication::translate("MainWindow", "Origin", 0, QApplication::UnicodeUTF8));
    ___qtreewidgetitem->setText(3, QApplication::translate("MainWindow", "Description", 0, QApplication::UnicodeUTF8));
    m_issuesContextMenuHandler = new ContextMenuHandler();
    m_issuesTree->setItemDelegate(new ContextMenuDelegate(m_issuesContextMenuHandler, m_issuesTree));

    m_datatypesTreeDock->setWidget(m_datatypesTree);
    m_projectTreeDock->setWidget(m_projectTree);
    m_issuesTreeDock->setWidget(m_issuesTree);
    addDockWidget(Qt::LeftDockWidgetArea, m_projectTreeDock);
    addDockWidget(Qt::LeftDockWidgetArea, m_datatypesTreeDock);
    addDockWidget(Qt::BottomDockWidgetArea, m_issuesTreeDock);

    m_issuesTreeDock->hide();
    IssueManager::getInstance().setIssuesDock(m_issuesTreeDock);

    showMaximized();
}

ContextMenuEnabledTreeWidgetItem* MainWindow::createDataTypeTreeEntry(UserDataType* udt)
{
    return m_workspace->workingVersion()->getGui()->createDataTypeTreeEntry(udt);
}

void MainWindow::onNewSolution()
{
    bool w = false;
    if(m_btndlg && m_btndlg->isVisible())
    {
        Qt::WindowFlags flags = m_btndlg->windowFlags();
        m_btndlg->setWindowFlags(flags ^ (Qt::SplashScreen |Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint));
        w = true;
    }

    NewProjectDialog* nprjdlg = new NewProjectDialog();
    nprjdlg->setModal(true);
    nprjdlg->focusOnEditField();
    nprjdlg->exec();

    if(nprjdlg->getProjectName().length() > 0 && nprjdlg->getSolutionName().length() > 0)
    {
        onCloseSolution();

        if(! m_workspace->hasCurrentSolution())
        {
            if(!m_workspace->createCurrentSolution(nprjdlg->getSolutionName()))
            {
                QMessageBox::critical (this, tr("Error"), tr("Cannot create a solution. Not enough memory?"), QMessageBox::Ok);
                if(m_btndlg && m_btndlg->isVisible() && w)
                {
                    Qt::WindowFlags flags = m_btndlg->windowFlags();
                    m_btndlg->setWindowFlags(flags | Qt::SplashScreen |Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
                    m_btndlg->show();
                }
                return;
            }
        }
        else // we are creating a new solution, it will be appended to the solutions in the workspace, but in release 0.2 this is not supported yet
        {
            if(!m_workspace->createSolution(nprjdlg->getSolutionName()))
            {
                QMessageBox::critical (this, tr("Error"), tr("Cannot create a solution. Not enough memory?"), QMessageBox::Ok);
                if(m_btndlg && m_btndlg->isVisible() && w)
                {
                    Qt::WindowFlags flags = m_btndlg->windowFlags();
                    m_btndlg->setWindowFlags(flags | Qt::SplashScreen |Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
                    m_btndlg->show();
                }
                return;
            }
        }

        setupGuiForNewSolution();

        Project* project = new Project(nprjdlg->getProjectName().toUpper(), m_projectTree, m_datatypesTree, m_issuesTree, nprjdlg->enableOOPFeatures());
        project->setEngine(nprjdlg->getDatabaseEngine());
        project->createMajorVersion();
        m_workspace->addProjectToSolution(m_workspace->currentSolution(), project);

        // are we supposed to inherit the default data types?
        if(nprjdlg->inheritDefaultDatatypes())
        {
            QVector<UserDataType*> dts = m_workspace->loadDefaultDatatypesIntoCurrentSolution();

            for(int i=0; i<dts.size(); i++)
            {
                createDataTypeTreeEntry(dts.at(i));
            }

            m_datatypesTree->scrollToItem(m_workspace->workingVersion()->getGui()->getDtsItem());
            m_datatypesTree->expandAll();
        }

        // expand the tree
        m_projectTree->expandAll();

        // show the project properties window
        showProjectDetails();

        setWindowTitle(tr("DDM - [") + m_workspace->currentSolution()->name() + tr("]"));

        connectActionsFromTablePopupMenu();
        if(!m_workspace->currentProjectIsOop())
        {
            m_workspace->workingVersion()->getGui()->getTablesItem()->setText(0, tr("Tables"));
        }

        if(nprjdlg->getProjectType() != NewProjectDialog::PRJ_DATAMODEL)
        {
            m_revEngWizard = new ReverseEngineerWizard(nprjdlg->getDatabaseEngine());
            m_revEngWizard->setWizardStyle(QWizard::ClassicStyle);
            m_revEngWizard->show();

            QObject::connect(m_revEngWizard, SIGNAL(currentIdChanged(int)), this, SLOT(onReverseEngineerWizardNextPage(int)));
            QObject::connect(m_revEngWizard, SIGNAL(accepted()), this, SLOT(onReverseEngineerWizardAccept()));
        }

        enableActions();
        m_workspace->workingVersion()->getGui()->setMainWindow(this);
        delete m_btndlg;
        m_btndlg = 0;
    }
    else
    {
        if(m_btndlg && m_btndlg->isVisible() && w)
        {
            Qt::WindowFlags flags = m_btndlg->windowFlags();
            m_btndlg->setWindowFlags(flags | Qt::SplashScreen |Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
            m_btndlg->show();
        }
    }
}

void MainWindow::onDTTreeClicked()
{
    QList<QTreeWidgetItem*> selectedItems = m_datatypesTree->selectedItems();
    if(selectedItems.length() == 1)
    {
        QTreeWidgetItem* item = selectedItems[0];
        if(item == m_workspace->workingVersion()->getGui()->getDtsItem())
        {// we have clicked on the Data Types item
            DataTypesListForm* dtLst = new DataTypesListForm(this);
            dtLst->feedInDataTypes(m_workspace->workingVersion()->getDataTypes());
            setCentralWidget(dtLst);
        }
        else
        {
            if(item->parent() != m_workspace->workingVersion()->getGui()->getDtsItem()) // this must be a data type
            {
                QVariant qv = item->data(0, Qt::UserRole);
                UserDataType* udt = static_cast<UserDataType*>(qv.data());
                if(m_workspace->workingVersion()->hasDataType(udt->getName()))
                {
                    udt = m_workspace->workingVersion()->getDataType(udt->getName());
                }
                NewDataTypeForm* frm = new NewDataTypeForm(DataType::DT_INVALID, m_workspace->currentProjectsEngine(), this);
                frm->focusOnName();
                frm->setMainWindow(this);
                frm->setDataType(udt);
                setCentralWidget(frm);
            }
        }
        HelpWindow* hw = HelpWindow::instance();
        hw->showHelp(QString("/doc/dtyp.html"));
    }
}

NewTableForm* MainWindow::showExistingTable(Table *table)
{
    NewTableForm* frm = m_workspace->workingVersion()->getGui()->getTableFormForExistingTable();
    frm->setTable(table);
    frm->focusOnName();
    setCentralWidget(frm);
    HelpWindow* hw = HelpWindow::instance();
    hw->showHelp(QString("/doc/tabl.html"));
    return frm;
}

void MainWindow::showTable(const QString &tabName, bool focus)
{
    Table* table =  m_workspace->workingVersion()->getTable(tabName);
    if(table == 0)  // shouldn't be ...
    {
        return;
    }
    showExistingTable(table);
    if(focus)
    {
        m_projectTree->setCurrentItem(table->getLocation());
    }
}

void MainWindow::showTableInstance(const QString &tabName, bool focus)
{
    TableInstance* table =  m_workspace->workingVersion()->getTableInstance(tabName);
    if(table == 0)  // shouldn't be ...
    {
        return;
    }

    TableInstanceForm* frmTinst = new TableInstanceForm(this);

    frmTinst->setTableInstance(table);
    frmTinst->createTableWithValues();
    setCentralWidget(frmTinst);

    HelpWindow* hw = HelpWindow::instance();
    hw->showHelp(QString("/doc/tinst.html"));

    if(focus) m_projectTree->setCurrentItem(table->getLocation());
}

void MainWindow::showDataType(const QString &name, bool focus)
{
    UserDataType* dt = m_workspace->workingVersion()->getDataType(name);
    if(dt == 0)  // shouldn't be ...
    {
        return;
    }

    NewDataTypeForm* frm = new NewDataTypeForm(DataType::DT_INVALID, m_workspace->currentProjectsEngine(), this);
    frm->focusOnName();
    frm->setMainWindow(this);
    frm->setDataType(dt);
    setCentralWidget(frm);

    if(focus) m_datatypesTree->setCurrentItem(dt->getLocation());

    HelpWindow* hw = HelpWindow::instance();
    hw->showHelp(QString("/doc/dtyp.html"));
}

void MainWindow::currentProjectTreeItemChanged(QTreeWidgetItem * current, QTreeWidgetItem*)
{
    if(current)
    {
        if(current == m_workspace->workingVersion()->getGui()->getTablesItem())
        {// we have clicked on the Tables item (i.e. the list of tables)
            TablesListForm* tblLst = m_workspace->workingVersion()->getGui()->getTablesListForm();
            tblLst->populateTables(m_workspace->workingVersion()->getTables());
            tblLst->setOop(m_workspace->currentProjectIsOop());
            setCentralWidget(tblLst);
        }
        else
        if(current == m_workspace->workingVersion()->getGui()->getTableInstancesItem())
        {// we have clicked on the Table instances item (i.e. the list of table instances)
            TableInstancesListForm* frm = m_workspace->workingVersion()->getGui()->getTableInstancesListForm();
            frm->populateTableInstances(m_workspace->workingVersion()->getTableInstances());
            setCentralWidget(frm);
        }
        else
        if(current == m_workspace->currentProject()->getLocation())
        {
            showProjectDetails();
        }
        else
        if(current == m_workspace->workingVersion()->getGui()->getFinalSqlItem())
        {
            SqlForm* frm = m_workspace->workingVersion()->getGui()->getSqlForm();
            frm->setSqlSource(0);
            frm->presentSql(m_workspace->currentProject(), m_workspace->currentProject()->getCodepage());
            setCentralWidget(frm);
        }
        else
        {
            if(current->parent() && current->parent() == m_workspace->workingVersion()->getGui()->getTablesItem())
            {
                // the user clicked on a table, the name of the table is a tag
                QVariant qv = current->data(0, Qt::UserRole);
                QString tabName = qv.toString();
                showTable(tabName, false);
            }
            else
            if(current->parent() && current->parent() == m_workspace->workingVersion()->getGui()->getDiagramsItem())
            {
                // the user clicked on a diagram
                QVariant qv = current->data(0, Qt::UserRole);
                QString diagramName = qv.toString();
                Diagram* dgram = m_workspace->workingVersion()->getDiagram(diagramName);
                if(dgram == 0)
                {
                    return;
                }
                DiagramForm* df = new DiagramForm(m_workspace->workingVersion(), dgram, this);
                dgram->setForm(df);
                setCentralWidget(dgram->getDiagramForm());
                df->paintDiagram();

                HelpWindow* hw = HelpWindow::instance();
                hw->showHelp(QString("/doc/dgram.html"));
            }
            else
            if(current->parent() && current->parent() == m_workspace->workingVersion()->getGui()->getTableInstancesItem())
            {
                // user clicked on a table instance
                TableInstanceForm* frm = new TableInstanceForm(this);
                QVariant qv = current->data(0, Qt::UserRole);
                QString instanceName = qv.toString();

                frm->setTableInstance(m_workspace->workingVersion()->getTableInstance(instanceName));
                frm->createTableWithValues();
                setCentralWidget(frm);

                HelpWindow* hw = HelpWindow::instance();
                hw->showHelp(QString("/doc/tinst.html"));

            }
            else
            if(current->parent() && current->parent() == m_workspace->workingVersion()->getGui()->getViewsItem())
            {
                // user clicked on a view
                QVariant qv = current->data(0, Qt::UserRole);
                QString viewName = qv.toString();
                View* v = m_workspace->workingVersion()->getView(viewName);
                if(v)
                {
                    if(v->getManual())
                    {
                        m_nvf = new NewViewForm(false, 0, this);

                        m_nvf->setSqlSource(v);
                        m_nvf->setView(v);
                        m_nvf->presentSql(Workspace::getInstance()->currentProject(), QString("latin1"));

                        setCentralWidget(m_nvf);
                    }
                    else
                    {
                        m_nvf = 0;
                        v->getHelper()->resetContent();
                        v->getHelper()->setForm(this);
                        rerenderQuery(v->getQuery());
                    }

                    HelpWindow* hw = HelpWindow::instance();
                    hw->showHelp(QString("/doc/view.html"));
                }

            }
            else
            if(current->parent() && current->parent() == m_workspace->workingVersion()->getGui()->getFinalSqlItem())
            {
                // user clicked on a SQL item
                SqlForm* frm = new SqlForm(m_workspace->currentProjectsEngine(), this);
                QVariant qv = current->data(0, Qt::UserRole);
                QString name = qv.toString();
                SqlSourceEntity* ent = NULL;

                if(m_workspace->currentProjectIsOop())
                {
                    ent = m_workspace->workingVersion()->getTableInstance(name);
                }
                else
                {
                    ent = m_workspace->workingVersion()->getTable(name);
                }

                if(ent == 0)
                {
                    ent = m_workspace->workingVersion()->getView(name);
                }

                if(ent == 0)
                {
                    // hm.. this shouldn't be
                    return;
                }

                frm->setSqlSource(ent);
                frm->presentSql(m_workspace->currentProject(), ent, m_workspace->currentProject()->getCodepage());
                setCentralWidget(frm);

                HelpWindow* hw = HelpWindow::instance();
                hw->showHelp(QString("/doc/sqls.html"));

            }
            else    // user possibly clicked on a table which had a parent a table ...
            {   // TODO: Code duplication with the "table" stuff above
                QVariant qv = current->data(0, Qt::UserRole);
                QString tabName = qv.toString();
                Table* table =  m_workspace->workingVersion()->getTable(tabName);
                if(table == 0)  // shouldn't be ...
                {
                    return;
                }

                showExistingTable(table);
            }
        }
    }
}

void MainWindow::onNewTable()
{
    NewTableForm* frm = Workspace::getInstance()->workingVersion()->getGui()->getTableFormForNewTable();
    frm->focusOnName();

    m_projectTree->setCurrentItem(0);
    setCentralWidget(frm);

    HelpWindow* hw = HelpWindow::instance();
    hw->showHelp(QString("/doc/tabl.html"));
}

void MainWindow::showNewDataTypeWindow(int a)
{
    NewDataTypeForm* frm = new NewDataTypeForm((DataType::DT_TYPE)a, m_workspace->currentProjectsEngine(), this);
    frm->focusOnName();
    frm->setMainWindow(this);
    m_projectTree->setCurrentItem(0);
    setCentralWidget(frm);

    HelpWindow* hw = HelpWindow::instance();
    hw->showHelp(QString("/doc/dtyp.html"));
}

void MainWindow::onNewDataType()
{
    showNewDataTypeWindow(DataType::DT_INVALID);
}

bool MainWindow::onSaveNewDataType(const QString& name, const QString& type, const QString& sqlType, const QString& size, const QString& defaultValue, const QString& cp,
                             const QStringList& mvs, const QString& desc, bool unsi, bool canBeNull, bool autoInc,  UserDataType* pudt)
{
    if(name.length() == 0 || type.length() == 0 || sqlType.length() == 0)
    {
        QMessageBox::critical (this, tr("Error"), tr("Please specify all the required data"), QMessageBox::Ok);
        return false;
    }


    UserDataType* udt = new UserDataType(name, type, sqlType, size, defaultValue, cp, mvs, unsi, desc, canBeNull, autoInc);

    if(! m_workspace->currentProjectsEngine()->getDTSupplier()->isValid(udt))
    {
        QMessageBox::critical (this, tr("Error"), tr("This datatype requires a length"), QMessageBox::Ok);
        return false;
    }

    UserDataType* other = m_workspace->workingVersion()->getDataType(name);

    if(other &&  other != pudt)
    {
        QMessageBox::critical (this, tr("Error"), tr("Only one datatype with the name ") + name + tr(" can exist in the project."), QMessageBox::Ok);
        return false;
    }

    // check if this is Saving an existing data type or updating a new one
    if(pudt)    // saving
    {
        *pudt = *udt;
        pudt->getLocation()->setIcon(0, pudt->getIcon());
        pudt->getLocation()->setText(0, name);
        pudt->getLocation()->setText(1, pudt->sqlAsString());

        // updating the "data" of the tree item
        QVariant var;
        var.setValue(*udt);
        pudt->getLocation()->setData(0, Qt::UserRole, var);
    }
    else        // new stuff
    {
        // add to the project itself
        m_workspace->workingVersion()->addNewDataType(udt);

        // create the tree entry
        ContextMenuEnabledTreeWidgetItem* newDtItem = createDataTypeTreeEntry(udt);

        // set the link to the tree
        m_datatypesTree->expandItem(newDtItem);
        m_datatypesTree->scrollToItem(newDtItem);
        return true;
    }
    return false;
}

void MainWindow::saveProject(bool saveAs)
{
    if(!m_workspace->hasCurrentSolution())
    {
        return;
    }

    QString fileName = "";
    if( (!m_workspace->currentSolutionWasSavedAlready() && !saveAs) || saveAs )
    {
        fileName = QFileDialog::getSaveFileName(this,  tr("Save solution"), "", tr("DDM solution files (*.dmx)"));
        if(fileName.length() == 0)
        {
            return;
        }
    }

    m_workspace->saveCurrentSolution(fileName);
    m_ui->statusBar->showMessage(tr("Saved"), 3000);
}

void MainWindow::onSaveSolution()
{
    saveProject(false);
}

void MainWindow::populateTreeWithSolution(Solution* sol)
{
    for(int i=0; i<sol->projects().size(); i++)
    {
        sol->projects()[i]->createTreeItem(m_projectTree, m_datatypesTree, m_issuesTree);
        sol->projects()[i]->populateTreeItem();
    }
}

void MainWindow::doLoadSolution(const QString& fileName, bool splashVisible)
{
    if(!m_workspace->loadSolution(fileName))
    {
        QMessageBox::critical (this, tr("Error"), tr("Cannot load the solution."), QMessageBox::Ok);
        if(splashVisible)
        {
            Qt::WindowFlags flags = m_btndlg->windowFlags();
            m_btndlg->setWindowFlags(flags | Qt::SplashScreen |Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
            m_btndlg->show();
        }

        return;
    }

    freeGuiElements();

    setupGuiForNewSolution();

    populateTreeWithSolution(m_workspace->currentSolution());

    if(!m_workspace->currentProjectIsOop())
    {
        m_workspace->workingVersion()->getGui()->getTablesItem()->setText(0, tr("Tables"));
    }

    m_projectTree->expandAll();

    ProjectDetailsForm* prjDetailsForm = new ProjectDetailsForm(this);
    prjDetailsForm->setProject(m_workspace->currentSolution()->currentProject());
    setCentralWidget(prjDetailsForm);

    setWindowTitle(tr("DDM - [") + m_workspace->currentSolution()->name() + tr("]"));

    connectActionsFromTablePopupMenu();

    enableActions();

    m_workspace->workingVersion()->getGui()->setMainWindow(this);
    delete m_btndlg;
    m_btndlg = 0;
}

void MainWindow::onOpenSolution()
{
    bool splashVisible = false;
    if(m_btndlg && m_btndlg->isVisible())
    {
        Qt::WindowFlags flags = m_btndlg->windowFlags();
        m_btndlg->setWindowFlags(flags ^ (Qt::SplashScreen |Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint));
        splashVisible = true;
    }

    QString fileName = QFileDialog::getOpenFileName(this,  tr("Open solution"), "", tr("DDM solution files (*.dmx);;All files (*.*)"));
    if(fileName.length() == 0)
    {
        if(splashVisible)
        {
            Qt::WindowFlags flags = m_btndlg->windowFlags();
            m_btndlg->setWindowFlags(flags |Qt::SplashScreen | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
            m_btndlg->show();
        }
        return;
    }

    MostRecentlyUsedFiles::instance().addFile(fileName);

    doLoadSolution(fileName, splashVisible);
}

void MainWindow::enableActions()
{
    m_ui->action_NewDataType->setEnabled(true);
    m_ui->action_NewTable->setEnabled(true);
    m_ui->action_NewDiagram->setEnabled(true);
    m_ui->action_Save->setEnabled(true);
    m_ui->action_SaveAs->setEnabled(true);
    m_ui->action_Deploy->setEnabled(true);
    m_ui->action_ProjectTree->setEnabled(true);
    m_ui->action_ProjectTree->setChecked(true);
    m_ui->action_Datatypes_Tree->setChecked(true);
    m_ui->action_Datatypes_Tree->setEnabled(true);
    m_ui->action_ConnectionsTree->setEnabled(true);
    m_ui->action_ConnectionsTree->setChecked(m_connectionsTreeDock?m_connectionsTreeDock->isVisible():false);
    m_ui->action_Validate->setEnabled(true);
    m_ui->action_NewView->setEnabled(true);
    m_ui->action_NewDatabaseConnection->setEnabled(true);
    m_ui->action_Preferences->setEnabled(true);

    if(m_workspace->currentProjectIsOop())
    {
        m_ui->action_NewTableInstance->setEnabled(true);
    }
    else
    {
        m_ui->action_NewTableInstance->setVisible(false);

        m_ui->action_NewTable->setText(tr("New Table"));
        m_ui->action_NewTable->setToolTip(tr("Create a new Table"));

        ContextMenuEnabledTreeWidgetItem* item = m_workspace->workingVersion()->getGui()->getTableInstancesItem();
        item->setHidden(true);
    }

    m_ui->action_NewTableInstance->setMenu(ContextMenuCollection::getInstance()->getCreateTableInstancesPopupMenu());
    m_ui->action_NewDataType->setMenu(ContextMenuCollection::getInstance()->getDatatypesPopupMenu());
    m_ui->action_NewView->setMenu(ContextMenuCollection::getInstance()->getCreateNewViewPopupMenu());
    m_ui->action_Deploy->setMenu(ContextMenuCollection::getInstance()->getDeployPopupMenu());
}

void MainWindow::connectActionsFromTablePopupMenu()
{
    QObject::connect(ContextMenuCollection::getInstance()->getAction_RemoveTable(), SIGNAL(activated()), this, SLOT(onDeleteTableFromPopup()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_AddTable(), SIGNAL(activated()), this, SLOT(onNewTable()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_TableAddColumn(), SIGNAL(activated()), this, SLOT(onTableAddColumnFromPopup()));
    if(m_workspace->currentProjectIsOop())
    {
        // the table popup
        QObject::connect(ContextMenuCollection::getInstance()->getAction_SpecializeTable(), SIGNAL(activated()), this, SLOT(onSpecializeTableFromPopup()));
        QObject::connect(ContextMenuCollection::getInstance()->getAction_InstantiateTable(), SIGNAL(activated()), this, SLOT(onInstantiateTableFromPopup()));

        // now the table instance popup
        QObject::connect(ContextMenuCollection::getInstance()->getAction_DeleteTableInstance(), SIGNAL(activated()), this, SLOT(onDeleteInstanceFromPopup()));
        QObject::connect(ContextMenuCollection::getInstance()->getAction_RenameTableInstance(), SIGNAL(activated()), this, SLOT(onRenameInstanceFromPopup()));

        // table instances popup
        QObject::connect(ContextMenuCollection::getInstance()->getAction_AddTableInstance(), SIGNAL(activated()), this, SLOT(onNewTableInstance()));

    }
    else
    {
        ContextMenuCollection::getInstance()->getAction_SpecializeTable()->setVisible(false);
        ContextMenuCollection::getInstance()->getAction_InstantiateTable()->setVisible(false);
    }
    QObject::connect(ContextMenuCollection::getInstance()->getAction_DuplicateTable(), SIGNAL(activated()), this, SLOT(onDuplicateTableFromPopup()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_CopyTable(), SIGNAL(activated()), this, SLOT(onCopyTableFromPopup()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_PasteTable(), SIGNAL(activated()), this, SLOT(onPasteTableFromPopup()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_DeleteDataType(), SIGNAL(activated()), this, SLOT(onDeleteDatatypeFromPopup()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_DuplicateDataType(), SIGNAL(activated()), this, SLOT(onDuplicateDatatypeFromPopup()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_DeleteDiagram(), SIGNAL(activated()), this, SLOT(onDeleteDiagramFromPopup()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_RenameDiagram(), SIGNAL(activated()), this, SLOT(onRenameDiagramFromPopup()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_AddDiagram(), SIGNAL(activated()), this, SLOT(onNewDiagram()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_AddString(), SIGNAL(activated()), this, SLOT(onNewStringType()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_AddNumeric(), SIGNAL(activated()), this, SLOT(onNewNumericType()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_AddBool(), SIGNAL(activated()), this, SLOT(onNewBoolType()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_AddDateType(), SIGNAL(activated()), this, SLOT(onNewDateTimeType()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_AddMisc(), SIGNAL(activated()), this, SLOT(onNewMiscType()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_AddBlob(), SIGNAL(activated()), this, SLOT(onNewBlobType()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_AddSpatial(), SIGNAL(activated()), this, SLOT(onNewSpatialType()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_GotoIssueLocation(), SIGNAL(activated()), this, SLOT(onGotoIssueLocation()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_IgnoreIssue(), SIGNAL(activated()), this, SLOT(onIgnoreIssue()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_IgnoreIssuesFromThisTable(), SIGNAL(activated()), this, SLOT(onIgnoreIssuesOfATable()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_CreateViewUsingQueryBuilder(), SIGNAL(activated()), this, SLOT(onNewView()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_CreateViewUsingSql(), SIGNAL(activated()), this, SLOT(onNewViewWithSql()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_ConnectionConnect(), SIGNAL(activated()), this, SLOT(onConnectConnection()));
    ContextMenuCollection::getInstance()->getAction_BrowsedTableInject()->setVisible(true);


}


void MainWindow::onAbout()
{
    bool wasBtndlg = false;
    if(m_btndlg && m_btndlg->isVisible())
    {
        Qt::WindowFlags flags = m_btndlg->windowFlags();
        m_btndlg->setWindowFlags(flags ^ (Qt::SplashScreen |Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint));
        wasBtndlg = true;
    }

    AboutBoxDialog* about = new AboutBoxDialog(this);
    about->setModal(true);
    about->exec();

    if(m_btndlg && wasBtndlg)
    {
        Qt::WindowFlags flags = m_btndlg->windowFlags();
        m_btndlg->setWindowFlags(flags | Qt::SplashScreen |Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
        m_btndlg->show();
    }

}

void MainWindow::onNewDiagram()
{
    Diagram* dgram = new Diagram(m_workspace->workingVersion());
    DiagramForm* df = new DiagramForm(m_workspace->workingVersion(), dgram, this);
    m_workspace->workingVersion()->addDiagram(dgram);
    setCentralWidget(df);
    dgram->setForm(df);
    onSaveDiagram(dgram);
}

bool MainWindow::onSaveDiagram(Diagram* dgram)
{
    if(!dgram->isSaved())
    {
        m_workspace->workingVersion()->getGui()->createDiagramTreeEntry(dgram);
    }
    else
    {
        dgram->getLocation()->setText(0, dgram->getName());
    }

    return true;
}

void MainWindow::onSaveAs()
{
    saveProject(true);
}

void MainWindow::onInstantiateTableFromPopup()
{
    if(m_projectTree->getLastRightclickedItem() == 0)
    {
        return;
    }

    // TODO: Code duplication with some stuff from below (onDelete...)
    ContextMenuEnabledTreeWidgetItem* item = m_projectTree->getLastRightclickedItem();
    m_projectTree->setLastRightclickedItem(0);

    QVariant qv = item->data(0, Qt::UserRole);
    QString tabName = qv.toString();
    Table* table =  m_workspace->workingVersion()->getTable(tabName);
    if(table == 0)  // shouldn't be ...
    {
        return;
    }

    m_workspace->workingVersion()->getGui()->updateForms();
    m_projectTree->setCurrentItem(instantiateTable(table->getName(), QStringList()));

}

void MainWindow::onSpecializeTableFromPopup()
{
    if(m_projectTree->getLastRightclickedItem() == 0)
    {
        return;
    }

    Table*table = getRightclickedTable();
    if(table == 0) return;

    Table* specializedTable = new Table(m_workspace->workingVersion());
    specializedTable->setName(table->getName() + "_specialized");
    specializedTable->setParent(table);
    specializedTable->setStorageEngine(table->getStorageEngine());

    ContextMenuEnabledTreeWidgetItem* newTblsItem = m_workspace->workingVersion()->getGui()->createTableTreeEntry(specializedTable, table->getLocation()) ;

    // add to the project itself
    m_workspace->workingVersion()->addTable(specializedTable);

    // set the link to the tree
    specializedTable->setLocation(newTblsItem);

    // now open the new table for to show this table
    showExistingTable(specializedTable);
}

void MainWindow::onTableAddColumnFromPopup()
{
    if(m_projectTree->getLastRightclickedItem() != 0)
    {
        Table *table = getRightclickedTable();
        NewTableForm* frm = showExistingTable(table);
        frm->focusOnNewColumnName();
        m_projectTree->setLastRightclickedItem(0);
    }
}

Table* MainWindow::getRightclickedTable()
{
    if(m_projectTree->getLastRightclickedItem() != 0)
    {
        ContextMenuEnabledTreeWidgetItem* item = m_projectTree->getLastRightclickedItem();
        m_projectTree->setLastRightclickedItem(0);

        QVariant qv = item->data(0, Qt::UserRole);
        QString tabName = qv.toString();
        Table* table =  m_workspace->workingVersion()->getTable(tabName);
        return table;
    }
    return 0;
}

TableInstance* MainWindow::getRightclickedTableInstance()
{
    if(m_projectTree->getLastRightclickedItem() != 0)
    {
        ContextMenuEnabledTreeWidgetItem* item = m_projectTree->getLastRightclickedItem();
        m_projectTree->setLastRightclickedItem(0);

        QVariant qv = item->data(0, Qt::UserRole);
        QString tabName = qv.toString();
        TableInstance* table =  m_workspace->workingVersion()->getTableInstance(tabName);
        return table;
    }
    return 0;
}

Diagram* MainWindow::getRightclickedDiagram()
{
    if(m_projectTree->getLastRightclickedItem() != 0)
    {
        ContextMenuEnabledTreeWidgetItem* item = m_projectTree->getLastRightclickedItem();
        m_projectTree->setLastRightclickedItem(0);

        QVariant qv = item->data(0, Qt::UserRole);
        QString dgrName = qv.toString();
        Diagram* dgr =  m_workspace->workingVersion()->getDiagram(dgrName);
        return dgr;
    }
    return 0;
}

UserDataType* MainWindow::getRightclickedDatatype()
{
    if(m_datatypesTree->getLastRightclickedItem() != 0)
    {
        ContextMenuEnabledTreeWidgetItem* item = m_datatypesTree->getLastRightclickedItem();
        m_datatypesTree->setLastRightclickedItem(0);

        QVariant qv = item->data(0, Qt::UserRole);
        UserDataType* udt = static_cast<UserDataType*>(qv.data());
        return udt;
    }
    return 0;
}

Connection* MainWindow::getRightclickedConnection()
{
    if(m_connectionsTree->getLastRightclickedItem() != 0)
    {
        ContextMenuEnabledTreeWidgetItem* item = m_connectionsTree->getLastRightclickedItem();
        m_connectionsTree->setLastRightclickedItem(0);

        QVariant qv = item->data(0, Qt::UserRole);
        QString n = qv.toString();
        Connection* c = ConnectionManager::instance()->getConnection(n);
        return c;
    }
    return 0;
}


void MainWindow::onDeleteTableFromPopup()
{
    Table* tab = getRightclickedTable();
    if(tab)
    {
        if(QMessageBox::question(this, tr("Are you sure?"), tr("Really delete ") + tab->getName() + "?", QMessageBox::Yes | QMessageBox::No) !=  QMessageBox::Yes)
        {
            return;
        }
        if(tab->hasSpecializedTables())
        {
            QMessageBox::warning(0, QObject::tr("Specialized tables were found"),
                                 QObject::tr("Cannot delete this table since there are tables specialized from it. Firstly remove those tables and then delete this one."), QMessageBox::Ok);
            return;

        }
        QString tabName = tab->getName();
        QWidget* w = centralWidget();
        if(m_workspace->workingVersion()->deleteTable(tab))
        {
            if(dynamic_cast<DiagramForm*>(w))
            {
                dynamic_cast<DiagramForm*>(w)->paintDiagram();
            }
            if(NewTableForm* ntf = dynamic_cast<NewTableForm*>(w))
            {
                if(ntf->getTableName() == tabName)
                {
                    setCentralWidget(new ProjectDetailsForm());
                }
            }
        }
        m_workspace->workingVersion()->getGui()->updateForms();
    }
}

void MainWindow::onCopyTableFromPopup()
{
    Table* tab = getRightclickedTable();
    if(tab)
    {
        tab->copy();
    }
}

void MainWindow::onPasteTableFromPopup()
{
    Table* tab = Workspace::getInstance()->pasteTable();
    if(tab)
    {
        m_workspace->getInstance()->workingVersion()->getGui()->createTableTreeEntry(tab);
        if(tab->getParent())
        {
            QTreeWidgetItem* p = tab->getLocation();
            p->parent()->removeChild(p);
            tab->getParent()->getLocation()->addChild(p);
        }
        // add the SQL item but only if it's not an oop project
        if(!m_workspace->currentProjectIsOop())
        {
            ContextMenuEnabledTreeWidgetItem* sqlItm = new ContextMenuEnabledTreeWidgetItem(m_workspace->workingVersion()->getGui()->getFinalSqlItem(), QStringList(tab->getName() + ".sql"));
            sqlItm->setIcon(0, IconFactory::getTablesIcon());
            sqlItm->setData(0, Qt::UserRole, tab->getName());
        }

        m_workspace->workingVersion()->getGui()->updateForms();
    }
}


void MainWindow::onDuplicateTableFromPopup()
{
    Table* tab = getRightclickedTable();
    if(tab)
    {
        Table* dupped = m_workspace->workingVersion()->duplicateTable(tab);
        m_workspace->onSaveNewTable(dupped);
        if(tab->getParent())
        {
            QTreeWidgetItem* p = dupped->getLocation();
            p->parent()->removeChild(p);
            tab->getParent()->getLocation()->addChild(p);
        }
        // add the SQL item but only if it's not an oop project
        if(!m_workspace->currentProjectIsOop())
        {
            ContextMenuEnabledTreeWidgetItem* sqlItm = new ContextMenuEnabledTreeWidgetItem(m_workspace->workingVersion()->getGui()->getFinalSqlItem(), QStringList(dupped->getName() + ".sql"));
            sqlItm->setIcon(0, IconFactory::getTablesIcon());
            sqlItm->setData(0, Qt::UserRole, dupped->getName());
        }

        m_workspace->workingVersion()->getGui()->updateForms();
    }
}

void MainWindow::onNewTableInstance()
{
    CreateTableInstancesDialog* newDialog = new CreateTableInstancesDialog(this);
    for(int i=0; i<m_workspace->workingVersion()->getTables().size(); i++)
    {
        newDialog->addTable(m_workspace->workingVersion()->getTables()[i]->getName());
    }

    if(newDialog->exec() == QDialog::Accepted)
    {
        QStringList items = newDialog->getSelectedTables();
        for(int i=0; i<items.size(); i++)
        {
            instantiateTable(items.at(i), items);
        }

        m_workspace->workingVersion()->getGui()->updateForms();
    }
}

void MainWindow::specificDeploymentCallback(const QString &connName)
{
    QStringList t;
    t << connName;
    doDeployment(QString("latin1"), t);
}

void MainWindow::onDeployHovered()
{
    QMenu* deployPopupMenu = ContextMenuCollection::getInstance()->getDeployPopupMenu();
    deployPopupMenu->clear();
    const QVector<Connection*> cons = ConnectionManager::instance()->connections();
    for(int i=0; i< cons.size(); i++)
    {
        QAction* act = new QAction(this);
        act->setText(cons.at(i)->getName() + " - " + cons.at(i)->getDb()+"@"+cons.at(i)->getHost());
        act->setData(cons.at(i)->getName());
        switch(cons.at(i)->getState())
        {
        case Connection::DID_NOT_TRY:
            act->setIcon(IconFactory::getDatabaseIcon());
            break;
        case Connection::FAILED:
            act->setIcon(IconFactory::getUnConnectedDatabaseIcon());
            break;
        case Connection::CONNECTED:
            act->setIcon(IconFactory::getConnectedDatabaseIcon());
            break;
        }
        deployPopupMenu->addAction(act);
        QObject::connect(act, SIGNAL(activated()),
                new DynamicActionHandlerforMainWindow(cons.at(i)->getName(), this, (MainWindow::dynamicAction)&MainWindow::specificDeploymentCallback),
                SLOT(called()));
    }
}


void MainWindow::onNewTableInstanceHovered()
{
    QMenu* createTableInstancesPopup = ContextMenuCollection::getInstance()->getCreateTableInstancesPopupMenu();
    createTableInstancesPopup->clear();

    if(m_workspace->currentSolution() && m_workspace->currentProject() && m_workspace->workingVersion())
    {
        for(int i=0; i<m_workspace->workingVersion()->getTables().size(); i++)
        {
            QAction * actionToAdd = new QAction(this);
            actionToAdd->setText(m_workspace->workingVersion()->getTables()[i]->getName());
            QIcon icon(IconFactory::getTablesIcon());
            actionToAdd->setData(QVariant(m_workspace->workingVersion()->getTables()[i]->getName()));
            actionToAdd->setIcon(icon);
            createTableInstancesPopup->addAction(actionToAdd);
            QObject::connect(actionToAdd, SIGNAL(activated()),
                             new DynamicActionHandlerforMainWindow(m_workspace->workingVersion()->getTables()[i]->getName(), this, (MainWindow::dynamicAction)&MainWindow::instantiateTableCallback), SLOT(called()));
        }
    }
}

void MainWindow::instantiateTableCallback(const QString &tabName)
{
    instantiateTable(tabName, QStringList());
}

ContextMenuEnabledTreeWidgetItem* MainWindow::instantiateTable(const QString& tabName, QStringList othersTablesBeingInstantiated, bool ref, Table* referencingTable, TableInstance* becauseOfThis)
{
    Version* cVersion = m_workspace->workingVersion();
    TableInstance* tinst = cVersion->instantiateTable(cVersion->getTable(tabName), ref);
    ContextMenuEnabledTreeWidgetItem* itm = new ContextMenuEnabledTreeWidgetItem(cVersion->getGui()->getTableInstancesItem(), QStringList(tinst->getName()));
    tinst->setLocation(itm);
    itm->setPopupMenu(ContextMenuCollection::getInstance()->getTableInstancePopupMenu());
    if(!ref)
    {
        itm->setIcon(0, IconFactory::getTabinstIcon());
    }
    else
    {
        tinst->addTableReferencingThis(referencingTable);
        itm->setIcon(0, IconFactory::getTabinstLockIcon());
    }

    if(becauseOfThis)
    {
        becauseOfThis->addInstantiatedTableInstance(tinst);
    }

    QVariant a(tinst->getName());
    itm->setData(0, Qt::UserRole, a);

    ContextMenuEnabledTreeWidgetItem* sqlItm = new ContextMenuEnabledTreeWidgetItem(cVersion->getGui()->getFinalSqlItem(), QStringList(tinst->getName()));
    sqlItm->setIcon(0, IconFactory::getTabinstIcon());
    sqlItm->setData(0, Qt::UserRole, a);
    tinst->setSqlItem(sqlItm);
    QSet<const Table*> referenced = cVersion->getTable(tabName)->getTablesReferencedByForeignKeys();
    QSetIterator<const Table*> i(referenced);
    while(i.hasNext())
    {
        const Table* tbl = i.next();
        // if this was not instantiated yet
        if(cVersion->getTableInstance(tbl->getName()) == 0)
        {
            //now check that the tbl->getName()is NOT in the provided list, and only then instantiate it recursively
            if(!othersTablesBeingInstantiated.contains(tbl->getName()))
            {
                instantiateTable(tbl->getName(), othersTablesBeingInstantiated, true, tinst->table(), tinst);
            }
        }
        else
        {
            // just add a remark to the tinst that this table instance was also be isntantiated because of this
            tinst->addInstantiatedTableInstance(cVersion->getTableInstance(tbl->getName()));
        }
    }
    return itm;
}

void MainWindow::onPreferences()
{
    PreferencesDialog* dlg = new PreferencesDialog(this);
    dlg->setModal(true);
    dlg->exec();

    QWidget* mainwidget = centralWidget();
    if(mainwidget)
    {
        SourceCodePresenterWidget* scw = dynamic_cast<SourceCodePresenterWidget*> (mainwidget);
        if(scw)
        {
            scw->updateSql(m_workspace->currentProject(), m_workspace->currentProject()->getCodepage());
        }
    }
}

void MainWindow::onRenameInstanceFromPopup()
{
    TableInstance* tinst = getRightclickedTableInstance();
    if(tinst)
    {
        SimpleTextInputDialog* dlg = new SimpleTextInputDialog(this, tr("Enter the new name"));
        dlg->setModal(true);
        dlg->setText(tinst->getName());
        if(dlg->exec() == QDialog::Accepted)
        {
            QString t = dlg->getText();
            tinst->setName(t);
            tinst->getLocation()->setText(0, t);
            QVariant a(t);
            tinst->getLocation()->setData(0, Qt::UserRole, a);
        }
    }
}

void MainWindow::onRenameDiagramFromPopup()
{
    Diagram* dgr = getRightclickedDiagram();
    if(dgr)
    {
        SimpleTextInputDialog* dlg = new SimpleTextInputDialog(this, tr("Enter the new name"));
        dlg->setModal(true);
        dlg->setText(dgr->getName());
        if(dlg->exec() == QDialog::Accepted)
        {
            QString t = dlg->getText();
            dgr->setName(t);
            dgr->getLocation()->setText(0, t);
            QVariant a(t);
            dgr->getLocation()->setData(0, Qt::UserRole, a);

            m_workspace->workingVersion()->getGui()->updateForms();
        }
    }
}

void MainWindow::onDeleteInstanceFromPopup()
{
    TableInstance* tinst = getRightclickedTableInstance();
    if(tinst)
    {
        if(QMessageBox::question(this, tr("Are you sure?"), tr("Really delete ") + tinst->getName() + "?", QMessageBox::Yes | QMessageBox::No) !=  QMessageBox::Yes)
        {
            return;
        }
        if(tinst->instantiatedBecuaseOfRkReference())
        {
            if(tinst->getReferencingTables().length() > 0)
            {
                QMessageBox::critical(this, tr("Error"), tr("Cannot delete this table instance since it was auto-instantiated because another table has a foreign key to it. Deleting the following tables will remove this table too:\n") + tinst->getReferencingTables(), QMessageBox::Ok);
                m_projectTree->setLastRightclickedItem(0);
                return;
            }
        }

        // mark for purge
        m_workspace->workingVersion()->deleteTableInstance(tinst);

        // Now delete the tree entries and the SQL tree entry
        for(int i=0; i<m_workspace->workingVersion()->getTableInstances().size(); i++)
        {
            if(m_workspace->workingVersion()->getTableInstances().at(i)->sentenced())
            {
                QString name = m_workspace->workingVersion()->getTableInstances().at(i)->getName();

                ContextMenuEnabledTreeWidgetItem* tabInstItem = m_workspace->workingVersion()->getGui()->getTableInstancesItem();
                for(int j=0; j<tabInstItem->childCount(); j++)
                {
                    QVariant a = tabInstItem->child(j)->data(0, Qt::UserRole);
                    if(a.toString() == name)
                    {
                        delete tabInstItem->child(j);
                    }
                }

                ContextMenuEnabledTreeWidgetItem* sqlItem = m_workspace->workingVersion()->getGui()->getFinalSqlItem();
                for(int j=0; j<sqlItem->childCount(); j++)
                {
                    QVariant a = sqlItem->child(j)->data(0, Qt::UserRole);
                    if(a.toString() == name)
                    {
                        delete sqlItem->child(j);
                    }
                }
            }
        }

        // and finally remove all purged table instances
        m_workspace->workingVersion()->purgeSentencedTableInstances();

        // and show a table instances list form
        TableInstancesListForm* tblLst = m_workspace->workingVersion()->getGui()->getTableInstancesListForm();
        m_workspace->workingVersion()->getGui()->updateForms();
        setCentralWidget(tblLst);
    }
}

void MainWindow::onCloseSolution()
{
    if(!m_workspace->hasCurrentSolution())
    {
        return;
    }

    m_workspace->onCloseSolution();

    freeGuiElements();

    showButtonDialog();
}

void MainWindow::onDeleteDatatypeFromPopup()
{
    ContextMenuEnabledTreeWidgetItem* itm = m_datatypesTree->getLastRightclickedItem();
    if(!itm)
    {
        return;
    }
    UserDataType* udt = getRightclickedDatatype();
    if(udt)
    {
        QString dtName = udt->getName();
        if(QMessageBox::question(this, tr("Are you sure?"), tr("Really delete ") + dtName + "?", QMessageBox::Yes | QMessageBox::No) ==  QMessageBox::Yes)
        {
            // now check that this stuff is not used in any of the tables and the delete it from the current version
            const QVector<Table*>& allTables = m_workspace->workingVersion()->getTables();
            QString usage = "";
            for(int i=0; i<allTables.size(); i++)
            {
                const QVector<Column*> & tablesColumns = allTables.at(i)->getColumns();
                for(int j=0; j<tablesColumns.size(); j++)
                {
                    QString cn = tablesColumns.at(j)->getDataType()->getName();
                    if(cn == dtName)
                    {
                        usage += tr("Table: ") + allTables.at(i)->getName() + tr(" Column:") + tablesColumns.at(j)->getName() + "\n";
                    }
                }
            }

            if(usage.length() > 0)
            {
                QMessageBox::critical(this, tr("Error"), tr("Cannot delete this data type since it's used in the following locations:\n") + usage, QMessageBox::Ok);
                return;
            }

            m_workspace->workingVersion()->deleteDataType(dtName);
            delete itm;
        }
    }
}

void MainWindow::onInjectBrowsedTable()
{
    if(m_connectionsTree->getLastRightclickedItem() == 0)
    {
        return;
    }

    if(!Workspace::getInstance()->hasCurrentSolution())
    {
        if(QMessageBox::question(this, tr("Question"), tr("There is no current solution. Would you like to create one?"), QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
        {
            onNewSolution();
        }
    }

    QVariant v = m_connectionsTree->getLastRightclickedItem()->data(0, Qt::UserRole);
    if(v.isValid())
    {
        QString s = v.toString();
        qDebug() << s;
        if(s.startsWith("B:"))
        {
            QString cname = s.mid(s.indexOf("?") + 1);
            Connection *c = ConnectionManager::instance()->getConnection(cname);
            if(!c) return;
            QString tab = s.left(s.indexOf("?")).mid(2);
            Table* t = Workspace::getInstance()->currentProjectsEngine()->reverseEngineerTable(c->getHost(), c->getUser(), c->getPassword(), c->getDb(), tab, Workspace::getInstance()->currentProject(), true);
            Workspace::getInstance()->currentProject()->getWorkingVersion()->addTable(t);
            m_workspace->workingVersion()->getGui()->createTableTreeEntry(t, m_workspace->workingVersion()->getGui()->getTablesItem());
            showTable(tab);
            QVector<TableInstance*> r = t->getTableInstances();
            for(int i=0; i<r.size(); i++)
            {
                m_workspace->workingVersion()->getGui()->createTableInstanceTreeEntry(r.at(i));
            }
        }
    }


}

void MainWindow::onBrowseBrowsedTable()
{
    if(m_connectionsTree->getLastRightclickedItem() == 0)
    {
        return;
    }

    m_connectionsTree->clearSelection();
    m_connectionsTree->getLastRightclickedItem()->setSelected(true);
    onConnectionItemDoubleClicked(m_connectionsTree->getLastRightclickedItem(), 0);
}


void MainWindow::onEditConnection()
{
    Connection* c = getRightclickedConnection();
    if(c)
    {
        InjectSqlDialog* ij = new InjectSqlDialog(0);
        ij->setModal(true);
        ij->populateConnectionDetails(c);
        if(ij->exec()  == QDialog::Accepted)
        {
            c->resetTo(ij->getName(), ij->getHost(), ij->getUser(), ij->getPassword(), ij->getDatabase(), true, ij->getAutoConnect());
            c->getLocation()->setText(0, c->getName());
            c->getLocation()->setText(1, c->getDb()+"@"+c->getHost());
            QVariant var(c->getName());
            c->getLocation()->setData(0, Qt::UserRole, var);
            switch(c->getState())
            {
            case Connection::DID_NOT_TRY:
                c->getLocation()->setIcon(0, IconFactory::getDatabaseIcon());
                break;
            case Connection::FAILED:
                c->getLocation()->setIcon(0, IconFactory::getUnConnectedDatabaseIcon());
                break;
            case Connection::CONNECTED:
                c->getLocation()->setIcon(0, IconFactory::getConnectedDatabaseIcon());
                break;
            }
        }
    }
}

void MainWindow::onDeleteConnection()
{
    Connection* c = getRightclickedConnection();
    if(c)
    {
        ConnectionManager::instance()->deleteConnection(c->getName());
        delete c->getLocation();
        delete c;
    }
}

void MainWindow::onBrowseConnection()
{
    Connection* c = getRightclickedConnection();
    if(c)
    {
        if(c->getState() == Connection::CONNECTED)
        {
            createConnectionTreeEntryForTables(c);
        }
    }
}

void MainWindow::onDropConnection()
{
    Connection* c = getRightclickedConnection();
    if(c)
    {
        bool w = false;
        if(m_btndlg && m_btndlg->isVisible())
        {
            Qt::WindowFlags flags = m_btndlg->windowFlags();
            m_btndlg->setWindowFlags(flags ^ (Qt::SplashScreen |Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint));
            w = true;
        }

        if(QMessageBox::question(this, tr("Drop?"), tr("Do you really want to drop ") + c->getName() + tr("? This is irreversible"), QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
        {
            c->getEngine()->dropDatabase(c->getHost(), c->getUser(), c->getPassword(), c->getDb());
            ConnectionManager::instance()->deleteConnection(c->getName());
            delete c->getLocation();
            delete c;
        }

        if(m_btndlg && w)
        {
            Qt::WindowFlags flags = m_btndlg->windowFlags();
            m_btndlg->setWindowFlags(flags | Qt::SplashScreen |Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
            m_btndlg->show();
        }
    }
}

void MainWindow::onConnectionItemDoubleClicked(QTreeWidgetItem* item,int)
{
    QVariant v = item->data(0, Qt::UserRole);
    if(v.isValid())
    {
        QString s = v.toString();
        qDebug() << s;
        if(s.startsWith("B:"))
        {
            if(m_btndlg && m_btndlg->isVisible())
            {
                Qt::WindowFlags flags = m_btndlg->windowFlags();
                m_btndlg->setWindowFlags(flags ^ (Qt::SplashScreen |Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint));
                m_btndlg->hide();
            }

            QString cname = s.mid(s.indexOf("?") + 1);
            Connection *c = ConnectionManager::instance()->getConnection(cname);
            if(!c) return;
            QString tab = s.left(s.indexOf("?")).mid(2);
            BrowseTableForm* frm = new BrowseTableForm(this, c, tab);
            setCentralWidget(frm);
            return;
        }
        Connection* c = ConnectionManager::instance()->getConnection(s);
        if(c)
        {
            if(c->tryConnect())
            {
                c->getLocation()->setIcon(0, IconFactory::getConnectedDatabaseIcon());
                createConnectionTreeEntryForTables(c);
            }
        }
    }
}

void MainWindow::createConnectionTreeEntryForTables(Connection *c)
{
    c->getLocation()->removeChild(c->getLocation()->child(0));

    ContextMenuEnabledTreeWidgetItem* connTablesItem = new ContextMenuEnabledTreeWidgetItem(c->getLocation(), QStringList(tr("Tables")));
    m_connectionsTree->addTopLevelItem(connTablesItem);
    connTablesItem->setIcon(0, IconFactory::getTabinstIcon());

    // Now do the browsing
    QVector<QString> dbTables = c->getEngine()->getAvailableTables(c->getHost(), c->getUser(), c->getPassword(), c->getDb());
    for(int i=0; i<dbTables.size(); i++)
    {
        ContextMenuEnabledTreeWidgetItem* newTblsItem = new ContextMenuEnabledTreeWidgetItem(connTablesItem, QStringList(dbTables.at(i)));
        QVariant var(QString("B:") + dbTables.at(i) + "?" + c->getName());
        newTblsItem->setData(0, Qt::UserRole, var);
        newTblsItem->setPopupMenu(ContextMenuCollection::getInstance()->getTableFromBrowsePopupMenu());
        newTblsItem->setIcon(0, IconFactory::getTabinstIcon());
        m_connectionsTree->addTopLevelItem(newTblsItem);
        c->addTable(dbTables.at(i));
    }

    // TODO: now come up with a mechanism that feeds continuously the reverse engineered tables into an object that feeds it in somewhere which
    // is used by the code completion enabled text edit when editing a table. Highly possible the destination will be the Connection object
    // since everyone has access to it.
}

void MainWindow::onConnectConnection()
{
    Connection* c = getRightclickedConnection();
    if(c)
    {
        if(c->getState() == Connection::CONNECTED) return;
        if(c->tryConnect())
        {
            c->getLocation()->setIcon(0, IconFactory::getConnectedDatabaseIcon());
        }
        else
        {
            c->getLocation()->setIcon(0, IconFactory::getUnConnectedDatabaseIcon());
        }
    }
}

void MainWindow::onDuplicateDatatypeFromPopup()
{
    UserDataType* udt = getRightclickedDatatype();
    if(udt)
    {
        UserDataType* dup = m_workspace->workingVersion()->duplicateDataType(udt->getName());

        ContextMenuEnabledTreeWidgetItem* newDTItem = createDataTypeTreeEntry(dup);

        m_datatypesTree->expandItem(newDTItem);
        m_datatypesTree->scrollToItem(newDTItem);

        // resize to look better
        m_datatypesTree->resizeColumnToContents(0);
        m_datatypesTree->resizeColumnToContents(1);
    }
}

void MainWindow::onDeleteDiagramFromPopup()
{
    Diagram* dgr = getRightclickedDiagram();
    if(dgr)
    {
        if(QMessageBox::question(this, tr("Are you sure?"), tr("Really delete ") + dgr->getName()+ "?", QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
        {
            return;
        }
        m_workspace->workingVersion()->deleteDiagram(dgr->getName());
        m_workspace->workingVersion()->getGui()->updateForms();
    }
}

void MainWindow::dtTreeItemClicked ( QTreeWidgetItem *, int)
{
    onDTTreeClicked();
}

void MainWindow::onNewStringType()
{
    showNewDataTypeWindow(DataType::DT_STRING);
}

void MainWindow::onNewNumericType()
{
    showNewDataTypeWindow(DataType::DT_NUMERIC);
}

void MainWindow::onNewBoolType()
{
    showNewDataTypeWindow(DataType::DT_BOOLEAN);
}

void MainWindow::onNewDateTimeType()
{
    showNewDataTypeWindow(DataType::DT_DATETIME);
}

void MainWindow::onNewBlobType()
{
    showNewDataTypeWindow(DataType::DT_BLOB);
}

void MainWindow::onNewMiscType()
{
    showNewDataTypeWindow(DataType::DT_MISC);
}

void MainWindow::onNewSpatialType()
{
    showNewDataTypeWindow(DataType::DT_SPATIAL);
}

void MainWindow::onGotoIssueLocation()
{
    Table* tab = 0;
    if(!m_issuesTree->getLastRightclickedItem()) return;
    QVariant a = m_issuesTree->getLastRightclickedItem()->data(0, Qt::UserRole);
    QString issueName = a.toString();
    Issue* selectedIssue = m_workspace->workingVersion()->getIssue(issueName);
    IssueOriginator* source = selectedIssue->getOriginator();
    Column* testCol = dynamic_cast<Column*>(source);
    if(testCol)
    {
        tab = testCol->getTable();
        NewTableForm*frm = showExistingTable(tab);
        frm->showColumn(testCol);
        frm->focusOnNewColumnName();
        frm->setCurrentColumn(testCol);
    }
    Connection* c = dynamic_cast<Connection*>(source);
    if(c)
    {
        InjectSqlDialog* ij = new InjectSqlDialog(0);
        ij->setModal(true);
        ij->populateConnectionDetails(c);
        if(ij->exec()  == QDialog::Accepted)
        {
            c->resetTo(ij->getName(), ij->getHost(), ij->getUser(), ij->getPassword(), ij->getDatabase(), true, ij->getAutoConnect());
        }
    }
}

void MainWindow::onIgnoreIssuesOfATable()
{
    if(!m_issuesTree->getLastRightclickedItem()) return;
    QVariant a = m_issuesTree->getLastRightclickedItem()->data(0, Qt::UserRole);
    QString tabName = a.toString();
    QVector<Issue*> issuesOfATable = IssueManager::getInstance().getIssuesOfTable(tabName);
    for(int i=0; i<issuesOfATable.size(); i++)
    {
        m_workspace->workingVersion()->removeIssue(issuesOfATable.at(i)->getName());
        IssueManager::getInstance().ignoringIssue(issuesOfATable.at(i));
    }
    m_workspace->workingVersion()->getGui()->cleanupOrphanedIssueTableItems();
}

void MainWindow::onIgnoreIssue()
{
    ContextMenuEnabledTreeWidgetItem* lastRclick = m_issuesTree->getLastRightclickedItem();
    if(!lastRclick) return;
    QVariant a = lastRclick->data(0, Qt::UserRole);
    QString issueName = a.toString();
    Issue* selectedIssue = m_workspace->workingVersion()->getIssue(issueName);
    // do not change the order below.
    m_workspace->workingVersion()->removeIssue(selectedIssue->getName());
    IssueManager::getInstance().ignoringIssue(selectedIssue);
    m_workspace->workingVersion()->getGui()->cleanupOrphanedIssueTableItems();
}

void MainWindow::createStatusLabel()
{
    if(lblStatus) delete lblStatus;

    lblStatus = new QLabel(m_ui->statusBar);
    lblStatus->setObjectName(QString::fromUtf8("lblStatus"));
    lblStatus->setGeometry(QRect(160, 10, 131, 21));
    QFont font;
    font.setFamily(QString::fromUtf8("Nimbus Sans L"));
    font.setBold(true);
    font.setWeight(75);
    lblStatus->setFont(font);
    lblStatus->setAutoFillBackground(true);
    lblStatus->setFrameShape(QFrame::WinPanel);
    lblStatus->setFrameShadow(QFrame::Sunken);
    m_ui->statusBar->addWidget(lblStatus);
    lblStatus->setText(QApplication::translate("MainWindow", "Deploying", 0, QApplication::UnicodeUTF8));

}

void MainWindow::onDeploy()
{
    if(m_workspace->currentProjectIsOop())
    {
        if(m_workspace->workingVersion()->getTableInstances().size() == 0)
        {
            QMessageBox msgBox;
            msgBox.setText(tr("You have the OOP features enabled, however you don't have created any table instances. There is nothing to deploy right now. Would you like to create table instances?"));
            msgBox.setIcon(QMessageBox::Question);
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox.setDefaultButton(QMessageBox::Yes);
            int ret = msgBox.exec();

            if(ret == QMessageBox::Yes)
            {
                onNewTableInstance();
            }
            else
            {
                return;
            }
        }
    }
    if(m_workspace->currentProjectIsOop() && m_workspace->workingVersion()->getTableInstances().size() == 0)
    {
        QMessageBox::information(this, tr("Deploying"), tr("Nothing to deploy"), QMessageBox::Ok);
        return;
    }
    InjectSqlDialog* injectDialog = new InjectSqlDialog(m_workspace->getInstance()->currentProjectsEngine(), this);
    injectDialog->setModal(true);
    if(injectDialog->exec() == QDialog::Accepted)
    {
        QStringList connectionNames = injectDialog->getSelectedConnections();
        QString codePage = injectDialog->getCodepage();
        doDeployment(codePage, connectionNames);
    }
}

void MainWindow::doDeployment(const QString& codePage, QStringList connectionNames)
{
    createStatusLabel();
    InjectSqlGenerator* injectSqlGen = new InjectSqlGenerator(codePage, m_workspace->workingVersion(), connectionNames, this);
    connect(injectSqlGen, SIGNAL(done(InjectSqlGenerator*)), this, SLOT(onSqlGenerationFinished(InjectSqlGenerator*)));
    injectSqlGen->generate();
}

void MainWindow::setStatus(const QString& s, bool err)
{
    if(!lblStatus) createStatusLabel();
    QString t = s;
    QTime now = QTime::currentTime();
    QDate nowd = QDate::currentDate();
    t += nowd.toString() + " - " + now.toString();
    lblStatus->setText(t);
    if(err)
    {
        lblStatus->setStyleSheet("QLabel { background-color : red; color : white; }");
    }
    else
    {
        lblStatus->setStyleSheet("QLabel { background-color : green; color : white; }");

    }
}

void MainWindow::onSqlGenerationFinished(InjectSqlGenerator *generator)
{
    QStringList sqlList = generator->getSqls();
    QStringList connectionNames = generator->getConnectionNames();
    Deployer* deployer = new Deployer(connectionNames, sqlList, this);
    m_deployers.append(deployer);
    connect(deployer, SIGNAL(done(Deployer*)), this, SLOT(onDeploymentFinished(Deployer*)));
    deployer->deploy();
}

void MainWindow::onDeploymentFinished(Deployer *d)
{
    if(d->hadErrors())
    {
        QMap<QString, QString> errors = d->getErrors();
        m_issuesTreeDock->show();
        for(QMap<QString, QString>::iterator it = errors.begin(); it != errors.end(); it++)
        {
            IssueManager::getInstance().createConnectionIssue(ConnectionManager::instance()->getConnection(it.key()), it.value());
            lblStatus->setStyleSheet("QLabel { background-color : red; color : white; }");
            QString t = QApplication::translate("MainWindow", "Deployment failed at ", 0, QApplication::UnicodeUTF8);
            QTime now = QTime::currentTime();
            QDate nowd = QDate::currentDate();
            t += nowd.toString() + " - " + now.toString();
            lblStatus->setText(t);
        }
    }
    else
    {
        QString t = QApplication::translate("MainWindow", "Deployment succeeded at ", 0, QApplication::UnicodeUTF8);
        QTime now = QTime::currentTime();
        QDate nowd = QDate::currentDate();
        t += nowd.toString() + " - " + now.toString();
        lblStatus->setText(t);
        lblStatus->setStyleSheet("QLabel { background-color : green; color : white; }");
    }

}

void MainWindow::onViewProjectTree()
{
    bool t = m_projectTreeDock->isVisible() ;
    m_ui->action_ProjectTree->setChecked(!t);
    m_projectTreeDock->setVisible(!t);
}

void MainWindow::onViewConnectionsTree()
{
    if(m_connectionsTreeDock)
    {
        bool t = m_connectionsTreeDock->isVisible() ;
        m_ui->action_ConnectionsTree->setChecked(!t);
        m_connectionsTreeDock->setVisible(!t);
    }
    else
    {
        showConnections();
    }
}

void MainWindow::onViewDatatypesTree()
{
    bool t = m_datatypesTreeDock->isVisible() ;
    m_ui->action_Datatypes_Tree->setChecked(!t);
    m_datatypesTreeDock->setVisible(!t);
}

void MainWindow::onReverseEngineerWizardNextPage(int cpage)
{
    switch(cpage)
    {
    case 1: // user just filled in the connection stuff, tell the wizard to fetch the data and feed ti to the next page
        m_revEngWizard->gatherConnectionData();

        if(!m_revEngWizard->connectAndRetrieveDatabases())  //failed?... don't go forward, there won't be databases
        {
            m_revEngWizard->back();
        }
        break;

    case 2: // user selected a database to reverse engineer
        if(!m_revEngWizard->selectDatabase()) // did he?
        {
            QMessageBox::critical(this, tr("Error"), tr("Please select a database"), QMessageBox::Ok);
            m_revEngWizard->back();
        }

        m_revEngWizard->connectAndRetrieveTables();
        break;
    case 3: // user selected the tables, advanced to the views
        if(!m_revEngWizard->selectDatabase()) // did he select a database?
        {
            QMessageBox::critical(this, tr("Error"), tr("Please select a database"), QMessageBox::Ok);
            m_revEngWizard->back();
        }

        m_revEngWizard->connectAndRetrieveViews();
    }
}

void MainWindow::onReverseEngineerWizardAccept()
{
    QVector<QString> tabsToReverse = m_revEngWizard->getTablesToReverse();
    QVector<QString> viewsToReverse = m_revEngWizard->getViewsToReverse();
    QString host = m_revEngWizard->getHost();
    QString user = m_revEngWizard->getUser();
    QString pass = m_revEngWizard->getPasword();
    QString db = m_revEngWizard->getDatabase();
    Project* p = m_workspace->currentProject();
    bool c = !m_revEngWizard->createDataTypesForColumns();
    DatabaseEngine* engine = m_workspace->currentProjectsEngine();

    createStatusLabel();
    lblStatus->setText(QApplication::translate("MainWindow", "Reverse engineering started", 0, QApplication::UnicodeUTF8));

    ReverseEngineerer* revEng = new ReverseEngineerer(c, engine, p, host, user, pass, db, tabsToReverse, viewsToReverse, this);
    connect(revEng, SIGNAL(done(ReverseEngineerer*)), this, SLOT(onReverseEngineeringFinished(ReverseEngineerer*)));
    revEng->reverseEngineer();
}

void MainWindow::onReverseEngineeringFinished(ReverseEngineerer*)
{
    lblStatus->setText(QApplication::translate("MainWindow", "Reverse engineering finished", 0, QApplication::UnicodeUTF8));
    {
    const QVector<Table*> tables = m_workspace->workingVersion()->getTables();
    for(int i=0; i<tables.size(); i++)
    {
        m_workspace->workingVersion()->getGui()->createTableTreeEntry(tables.at(i));
    }
    }
    {
    const QVector<View*> views = m_workspace->workingVersion()->getViews();
    for(int i=0; i<views.size(); i++)
    {
        m_workspace->workingVersion()->getGui()->createViewTreeEntry(views.at(i));
    }
    }
    {
    const QVector<TableInstance*> tableInstances = m_workspace->workingVersion()->getTableInstances();
    for(int i=0; i<tableInstances.size(); i++)
    {
        m_workspace->workingVersion()->getGui()->createTableInstanceTreeEntry(tableInstances.at(i));
    }
    }
}

void MainWindow::onNewConnection()
{
    bool wasVisible = false;
    if(m_btndlg && m_btndlg->isVisible())
    {
        m_btndlg->hide();
        wasVisible = true;
    }
    InjectSqlDialog* injectDialog = new InjectSqlDialog(0);
    injectDialog->setModal(true);
    if(injectDialog->exec() == QDialog::Accepted)
    {
        if(!m_connectionsTreeDock)
        {
            showConnections();
        }
        QString host = injectDialog->getHost();
        QString user = injectDialog->getUser();
        QString password = injectDialog->getPassword();
        QString db = injectDialog->getDatabase();
        QString name = injectDialog->getName();
        Connection* c = new Connection(name, host, user, password, db, true, injectDialog->getAutoConnect());
        ConnectionManager::instance()->addConnection(c);
        createConnectionTreeEntry(c);
    }
    if(wasVisible) m_btndlg->show();
}

void MainWindow::onValidate()
{
    m_ui->statusBar->showMessage(tr("Validating"), 0);
    m_workspace->workingVersion()->validateVersion(true);
    m_ui->statusBar->showMessage(tr("Validation finished"), 1000);
    if(m_workspace->workingVersion()->getIssues().size() == 0)
    {
        QMessageBox::information(this, tr("Congratulations"), tr("DDM has run a full validation on your database and it seems there were no issues found. Good work."), QMessageBox::Ok);
    }
}


void MainWindow::onNewViewWithSql()
{
    m_nvf = new NewViewForm(false, 0, this);
    View* v = new View(true);
    Workspace::getInstance()->workingVersion()->addView(v);
    Workspace::getInstance()->workingVersion()->getGui()->createViewTreeEntry(v);

    m_nvf->setSqlSource(v);
    m_nvf->setView(v);
    m_nvf->presentSql(Workspace::getInstance()->currentProject(), QString("latin1"));

    setCentralWidget(m_nvf);
}

void MainWindow::onNewView()
{
    View* v = new View(false);
    m_nvf = 0;
    v->getHelper()->setForm(this);
    Workspace::getInstance()->workingVersion()->addView(v);
    Workspace::getInstance()->workingVersion()->getGui()->createViewTreeEntry(v);

    rerenderQuery(v->getQuery());
}

void MainWindow::rerenderQuery(Query* q)
{
    int x = 20;
    int y = 20;
    int w = 0;
    int h = 0;

    q->initializeGraphicsItem();
    q->getGraphicsItem()->render(x, y, w ,h);
    q->getGraphicsItem()->updateWidth(w);

    // centerpoints
    int cx = 0;
    int cy = 0;

    if(m_nvf != 0)
    {
        m_nvf->getCenter(cx, cy);
    }

    m_nvf = new NewViewForm(true, q->getHelper(), this);
    m_nvf->setSqlSource(q->getSourceEntity());
    m_nvf->setView(dynamic_cast<View*>(q->getSourceEntity()));

    m_nvf->setGraphicsItem(q->getGraphicsItem());
    q->getHelper()->setScene(m_nvf->getScene());
    m_nvf->presentSql(Workspace::getInstance()->currentProject(), QString("latin1"));
    setCentralWidget(m_nvf);
    m_nvf->scrollTo(cx, cy);
}
