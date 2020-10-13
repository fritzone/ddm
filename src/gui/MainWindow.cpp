#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "MainWindowButtonDialog.h"
#include "NewProjectDialog.h"
#include "DataTypesListForm.h"
#include "NewDataTypeForm.h"
#include "core_UserDataType.h"
#include "Project.h"
#include "db_DatabaseEngine.h"
#include "Version.h"
#include "db_AbstractDTSupplier.h"
#include "NewTableForm.h"
#include "core_Table.h"
#include "core_Column.h"
#include "IconFactory.h"
#include "Configuration.h"
#include "ProjectDetailsForm.h"
#include "Solution.h"
#include "DeserializationFactory.h"
#include "AboutBoxDialog.h"
#include "DiagramForm.h"
#include "core_Diagram.h"
#include "ContextMenuEnabledTreeWidget.h"
#include "CreateTableInstancesDialog.h"
#include "TableInstanceForm.h"
#include "DynamicActionHandlerForMainWindow.h"
#include "SqlForm.h"
#include "core_TableInstance.h"
#include "PreferencesDialog.h"
#include "SimpleTextInputDialog.h"
#include "ContextMenuCollection.h"
#include "ddm_strings.h"
#include "Workspace.h"
#include "VersionGuiElements.h"
#include "InjectSqlDialog.h"
#include "reveng_ReverseEngineerWizard.h"
#include "IssueManager.h"
#include "NewViewForm.h"
#include "qbr_SelectQuery.h"
#include "core_View.h"
#include "gui_HelpWindow.h"
#include "Connection.h"
#include "ConnectionManager.h"
#include "core_Deployer.h"
#include "reveng_ReverseEngineerer.h"
#include "helper_MostRecentlyUsedFiles.h"
#include "BrowseTableForm.h"
#include "NameGenerator.h"
#include "ProcedureForm.h"
#include "core_Procedure.h"
#include "GuiElements.h"
#include "ConnectionGuiElements.h"
#include "TriggerForm.h"
#include "core_Trigger.h"
#include "NamedObjectListingForm.h"
#include "core_Function.h"
#include "MajorVersion.h"
#include "UidWarehouse.h"
#include "DeploymentInitiator.h"
#include "DocumentationForm.h"
#include "DocumentationGenerator.h"
#include "QHtmlDocument.h"
#include "core_Patch.h"
#include "TableComparisonForm.h"
#include "RepositoryGuiElements.h"
#include "repo_RoleListingForm.h"
#include "repo_RepositoryElementForm.h"
#include "MySqlConnection.h"
#include "SqliteConnection.h"
#include "conn_CUBRID.h"
#include "db_DatabaseEngineManager.h"
#include "core_Repository.h"
#include "gui_MainWindowSlotHouse.h"
#include "gui_DeleteHelper.h"

#include <QtGui>
#include <QMessageBox>
#include <QFileDialog>
#include <QSharedPointer>

MainWindow* MainWindow::m_instance = 0;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), m_ui(new Ui::MainWindow),
    m_btndlg(0), m_workspace(Workspace::getInstance()), m_revEngWizard(0), m_nvf(0),
    lblStatus(0), m_splashWasVisible(false),
    m_guiElements(0), m_connectionGuiElements(0), m_showMethods()
{
    m_ui->setupUi(this);
    m_instance = this;

    Repository::instance();
    Configuration::instance();
    DatabaseEngineManager::instance();
    ConnectionManager::instance();
    MainWindowSlotHouse::instance();

    m_connectionGuiElements = new ConnectionGuiElements();
    m_connectionGuiElements->createGuiElements();

    m_ui->action_ConnectionsTree->setChecked(true);
    m_ui->action_Repository->setChecked(true);

    showConnections();

    m_ui->action_Repository->setVisible(false);

    m_ui->action_NewDatabaseConnection->setEnabled(true);
    showMaximized();
    setWindowTitle(tr("DDM - [No Solution]"));

    m_btndlg = new MainWindowButtonDialog(this);
    if(Configuration::instance().showStartupdialog())
    {
        m_btndlg->showMe();
    }

    QApplication::instance()->installEventFilter(this);
}

MainWindow::~MainWindow()
{
    ConnectionManager::instance()->shutDown();
    delete m_ui;
    delete m_connectionGuiElements;
    delete m_guiElements;
    delete m_btndlg;
    delete m_revEngWizard;
    delete m_nvf;
    delete lblStatus;
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Escape)
    {
        if(m_btndlg && m_btndlg->isVisible())
        {
            m_btndlg->hide();
            m_btndlg = 0;
        }
    }
    if(e->key() == Qt::Key_F1)
    {
        QWidget* c = centralWidget();
        HelpWindow* hw = HelpWindow::instance();
        hw->showHelpForWidget(c);
    }
}

bool MainWindow::eventFilter(QObject *obj, QEvent *evt)
{
    if(obj == qApp && ( evt->type() == QEvent::ApplicationActivate
                      || evt->type() == QEvent::ApplicationDeactivate))
    {
        bool shouldHide = evt->type() == QEvent::ApplicationDeactivate;
        if (shouldHide) {
            m_splashWasVisible = m_btndlg && m_btndlg->isVisible();
            if(m_splashWasVisible && m_btndlg)
                m_btndlg->hide();
        } else {
            if(m_splashWasVisible && m_btndlg)
                m_btndlg->show();
        }
        return true;
    }
    return QMainWindow::eventFilter(obj, evt);
}

void MainWindow::setCentralWidget(QWidget *widget)
{
    QWidget* savedWidget = centralWidget();
    if(dynamic_cast<BrowseTableForm*>(savedWidget))
    {
        savedWidget->setParent(0);
    }
    QMainWindow::setCentralWidget(widget);
}

void MainWindow::freeGuiElements()
{
    QWidget* centralWidget = new QWidget(this);
    centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
    setCentralWidget(centralWidget);

    if(m_guiElements) m_guiElements->freeGuiElements();
}

void MainWindow::showProjectDetails()
{
    ProjectDetailsForm* prjDetailsForm = new ProjectDetailsForm(this);
    prjDetailsForm->setProject(m_workspace->currentProject(), m_lastLoadedProject);
    setCentralWidget(prjDetailsForm);
}

void MainWindow::showRepository()
{
    if(!m_ui->action_ConnectionsTree->isChecked())
    {
        m_repositoryGuiElements->getTreeDock()->hide();
    }
    else
    {
        addDockWidget(Qt::RightDockWidgetArea, m_repositoryGuiElements->getTreeDock());
        m_repositoryGuiElements->getTreeDock()->show();
    }
}

void MainWindow::showConnections()
{
    if(!m_ui->action_ConnectionsTree->isChecked())
    {
        m_connectionGuiElements->getConnectionsTreeDock()->hide();
    }
    else
    {
        addDockWidget(Qt::RightDockWidgetArea, m_connectionGuiElements->getConnectionsTreeDock());
        m_connectionGuiElements->getConnectionsTreeDock()->show();
    }
}

void MainWindow::setupGuiForNewSolution()
{
    m_guiElements = new GuiElements();
    m_guiElements->createGuiElements();

    addDockWidget(Qt::LeftDockWidgetArea, m_guiElements->getProjectDock());
    addDockWidget(Qt::BottomDockWidgetArea, m_guiElements->getIssuesDock());

    IssueManager::getInstance().setIssuesDock(m_guiElements->getIssuesDock());

    showMaximized();
}

void MainWindow::onNewSolution()
{
    bool w = false;
    if(m_btndlg && m_btndlg->isVisible())
    {
        hideSplashwindow();
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
                    showSplashwindow();
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
                    showSplashwindow();
                }
                return;
            }
        }

        Project* project = new Project(nprjdlg->getProjectName().toUpper(), nprjdlg->enableOOPFeatures());
        DatabaseEngine* engine = nprjdlg->getDatabaseEngine();
        m_workspace->addProjectToSolution(m_workspace->currentSolution(), project);
        project->setEngine(engine);

        setupGuiForNewSolution();

        m_ui->action_NewFunction->setVisible(engine->storedMethodSupport());
        m_ui->action_NewProcedure->setVisible(engine->storedMethodSupport());

        if(nprjdlg->getProjectType() == NewProjectDialog::PRJ_BINDTODATABASE)
        {
            InjectSqlDialog* injectDialog = new InjectSqlDialog(0, 0, 0, "");
            injectDialog->setupForBindToDatabase();
            injectDialog->setModal(true);
            if(injectDialog->exec() == QDialog::Accepted)
            {
                Connection* c = injectDialog->provideConnection();
                if(!c)
                {
                    qDebug() << "Cannot give a connection to bind to";
                    project->createMajorVersion(1, 0);
                }
                else
                {
                    QString meta = nprjdlg->getDatabaseEngine()->getDbMetadata(c);
                    if(meta.isEmpty())
                    {
                        QMessageBox::critical(this, tr("Error"),
                                tr("There is no metadata at the given location") );
                        project->createMajorVersion(1, 0);
                    }
                    else
                    {
                        QDomDocument doc;
                        QString lastErr;
                        if(!doc.setContent(meta, &lastErr))
                        {
                            QMessageBox::critical(this, tr("Error"), tr("Invalid metadata: ") + lastErr);
                            project->createMajorVersion(1, 0);
                        }
                        else
                        {
                            MajorVersion* mv = new MajorVersion;
                            project->addMajorVersion(mv);
                            DeserializationFactory::createMajorVersion(mv, project,
                                        project->getEngine(), doc,
                                        doc.firstChildElement().firstChildElement());
                        }
                    }
                    delete c;
                }
            }
            else
            {
                project->createMajorVersion(1, 0);
            }
        }
        else
        {
            project->createMajorVersion(1, 0);
        }

        project->createTreeItem(m_guiElements);
        project->populateTreeItem(m_guiElements);

        // are we supposed to inherit the default data types?
        if(nprjdlg->inheritDefaultDatatypes())
        {
            QVector<UserDataType*> dts = m_workspace->loadDefaultDatatypesIntoCurrentSolution(Workspace::getInstance()->currentSolution());

            for(int i=0; i<dts.size(); i++)
            {
                m_workspace->workingVersion()->getGui()->createDataTypeTreeEntry(dts.at(i));
            }
        }

        // expand the tree
        m_guiElements->getProjectTree()->expandAll();
        Version *tv = Workspace::getInstance()->workingVersion();
        tv->getGui()->collapseDTEntries();

        // show the project properties window
        showProjectDetails();

        setWindowTitle(tr("DDM - [") + m_workspace->currentSolution()->getName() + tr("]"));

        connectActionsFromPopupMenus();
        if(!m_workspace->currentProjectIsOop())
        {
            m_workspace->workingVersion()->getGui()->getTablesItem()->setText(0, tr("Tables"));
        }

        if(nprjdlg->getProjectType() == NewProjectDialog::PRJ_REVERSEENGINEER)
        {
            m_revEngWizard = new ReverseEngineerWizard(nprjdlg->getDatabaseEngine());
            m_revEngWizard->show();

            QObject::connect(m_revEngWizard, SIGNAL(currentIdChanged(int)), this, SLOT(onReverseEngineerWizardNextPage(int)));
            QObject::connect(m_revEngWizard, SIGNAL(accepted()), this, SLOT(onReverseEngineerWizardAccept()));
        }

        enableActions();
        delete m_btndlg;
        m_btndlg = 0;
    }
    else
    {
        if(m_btndlg && w)
        {
            showSplashwindow();
        }
    }
}

void MainWindow::onShowSolutionProperties()
{
    showProjectDetails();
}

void MainWindow::showNothing(Version* /*v*/, const QString &, bool /*focus*/ )
{}

NewTableForm* MainWindow::showExistingTable(Table *table, Version* v)
{
    if(table == 0 || v == 0)
    {
        return 0;
    }

    NewTableForm* frm = v->getGui()->getTableFormForExistingTable();
    frm->setTable(table);
    frm->focusOnName();
    setCentralWidget(frm);
    return frm;
}

void MainWindow::showTableWithGuid(Version *v, const QString &guid, bool focus)
{
    Table* table =  dynamic_cast<Table*>(UidWarehouse::instance().getElement(guid));
    if(table == 0)  // shouldn't be ...
    {
        return;
    }
    showExistingTable(table, v);
    if(focus)
    {
        m_guiElements->getProjectTree()->setCurrentItem(table->getLocation());
    }
}

void MainWindow::showTableInstanceWithGuid(Version* /*v*/, const QString &guid, bool focus)
{
    TableInstance* table =  dynamic_cast<TableInstance*>(UidWarehouse::instance().getElement(guid));
    if(table == 0)  // shouldn't be ...
    {
        return;
    }

    TableInstanceForm* frmTinst = new TableInstanceForm(this);

    frmTinst->setTableInstance(table);
    frmTinst->createTableWithValues();
    setCentralWidget(frmTinst);

    if(focus)
    {
        m_guiElements->getProjectTree()->setCurrentItem(table->getLocation());
    }
}

void MainWindow::showDataType(Version *v, const QString &name, bool focus)
{
    UserDataType* dt = v->getDataType(name);
    if(dt == 0)  // shouldn't be ...
    {
        return;
    }

    NewDataTypeForm* frm = new NewDataTypeForm(v, DT_INVALID, m_workspace->currentProjectsEngine(), this);
    frm->focusOnName();
    frm->setDataType(dt);
    setCentralWidget(frm);

    if(focus) m_guiElements->getProjectTree()->setCurrentItem(dt->getLocation());
}

void MainWindow::showDiagramWithGuid(Version *v, const QString & guid, bool /*focus*/)
{
    Diagram* dgram = dynamic_cast<Diagram*>(UidWarehouse::instance().getElement(guid));
    if(dgram == 0)
    {
        return;
    }
    DiagramForm* df = new DiagramForm(v, dgram, this);
    dgram->setForm(df);
    setCentralWidget(dgram->getDiagramForm());
    df->paintDiagram();
}

void MainWindow::showProcedureWithGuid(Version *v, const QString & guid, bool /*focus*/)
{
    Procedure* p = dynamic_cast<Procedure*>(UidWarehouse::instance().getElement(guid));
    if(p)
    {
        ProcedureForm* pf = v->getGui()->getProcedureForm(MODE_PROCEDURE, p->isGuided());
        pf->setProcedure(p);
        setCentralWidget(pf);
        pf->showSql();
    }
}

void MainWindow::showFunctionWithGuid(Version *v, const QString & guid, bool /*focus*/)
{
    Function* p = dynamic_cast<Function*>(UidWarehouse::instance().getElement(guid));
    if(p)
    {
        ProcedureForm* pf = v->getGui()->getProcedureForm(MODE_FUNCTION, p->isGuided());
        pf->setProcedure(p);
        setCentralWidget(pf);
        pf->showSql();
    }
}

void MainWindow::showTriggerWithGuid(Version *v, const QString & guid, bool /*focus*/)
{
    Trigger* p = dynamic_cast<Trigger*>(UidWarehouse::instance().getElement(guid));static const QString strBindToDbIcon = ":/images/actions/images/big/folder_database.png";

    if(p)
    {
        QStringList names;
        if(p->version()->getProject()->oopProject())
        {
            const QVector<TableInstance*>& allTables = v->getTableInstances();
            if(allTables.size() == 0)
            {
                QMessageBox::critical(MainWindow::instance(), QObject::tr("Cannot create a trigger when there are no table instances in an OOP project"), QObject::tr("No tables defined"), QMessageBox::Ok);
                return;
            }
            for(int i=0; i<allTables.size(); i++)
            {
                names.push_back(allTables[i]->getName());
            }
        }
        else
        {
            const QVector<Table*>& allTables = v->getTables();
            if(allTables.size() == 0)
            {
                QMessageBox::critical(MainWindow::instance(), QObject::tr("Cannot create a trigger when there are no tables"), QObject::tr("No tables defined"), QMessageBox::Ok);
                return;
            }
            for(int i=0; i<allTables.size(); i++)
            {
                names.push_back(allTables[i]->getName());
            }
        }

        TriggerForm* pf = v->getGui()->getTriggerForm();
        pf->feedInTables(names);
        pf->feedInTriggerEvents(Workspace::getInstance()->currentProjectsEngine()->getTriggerEvents());
        pf->feedInTriggerTimes(Workspace::getInstance()->currentProjectsEngine()->getTriggerTimings());
        pf->setTrigger(p);
        setCentralWidget(pf);
        pf->showSql();
    }
}

void MainWindow::showViewWithGuid(Version * ver, const QString & guid, bool /*focus*/)
{
    View* v = dynamic_cast<View*>(UidWarehouse::instance().getElement(guid));
    if(v)
    {
        if(v->isManual())
        {
            m_nvf = new NewViewForm(ver, false, 0, this);

            m_nvf->setSqlSource(v);
            m_nvf->setView(v);
            m_nvf->presentSql(Workspace::getInstance()->currentProject(), v->version());

            setCentralWidget(m_nvf);
        }
        else
        {
            m_nvf = 0;
            v->getHelper()->resetContent();
            rerenderQuery(v->getQuery());
        }
    }
}

void MainWindow::showObjectWithGuidAndVersion(Version *v, QString uid, showSomething s, bool f)
{
    (this->*s)(v, uid,f);
}

void MainWindow::patchTreeItemClicked(QTreeWidgetItem * current, int)
{
    if(current)
    {
        QVariant qv = current->data(0, Qt::UserRole);
        QString uid = qv.toString();

        Version* foundVersion = UidWarehouse::instance().getVersionForUid(uid);
        ObjectWithUid* obj = UidWarehouse::instance().getElement(uid);
        QString classUid = nullUid;
        if(obj)
        {
            classUid = obj->getClassUid().toString();
            classUid = classUid.toUpper();
        }
        UserDataType* udt = 0;
        if((udt = dynamic_cast<UserDataType*>(UidWarehouse::instance().getElement(uid))))
        {
            NewDataTypeForm* frm = new NewDataTypeForm(foundVersion, DT_INVALID, m_workspace->currentProjectsEngine(), this);
            frm->focusOnName();
            frm->setDataType(udt);
            setCentralWidget(frm);
        }
        else
        {
            QMap<QString, showSomething> mapping = m_showMethods.getMapping();
            if(mapping.contains(classUid))
            {
                showObjectWithGuidAndVersion(foundVersion, uid, mapping[classUid], false);

                QFont f = current->font(0);
                if(f.italic())  // TODO: the lamest way of checking if this element is in a suspended patch or not, but I'm too lazy now.
                {
                    this->centralWidget()->setDisabled(true);
                }
            }
        }
    }
}

void MainWindow::projectTreeItemClicked(QTreeWidgetItem * current, int)
{
    if(current)
    {
        QVariant qv = current->data(0, Qt::UserRole);
        QString uid = qv.toString();

        VersionElement* obj = dynamic_cast<VersionElement*>(UidWarehouse::instance().getElement(uid));
        Version* foundVersion = UidWarehouse::instance().getVersionForUid(uid);
        if(!foundVersion)
        {
            return;
        }

        // special cases:
        // 1. Full documentation
        if(foundVersion && current == foundVersion->getGui()->getDocumentationItem())
        {
            DocumentationForm* docF = new DocumentationForm(this);
            DocumentationGenerator gen(m_workspace->currentSolution());
            docF->setDoc(gen.getDocumentation());
            setCentralWidget(docF);
            docF->initiateStyleChange(docF->s_lastStyle);
            return;
        }

        // 2. the documentation of 1 item
        if(current->parent() && current->parent() == foundVersion->getGui()->getDocumentationItem())
        {
            DocumentationForm* docF = new DocumentationForm(this);
            QVariant qv = current->data(0, Qt::UserRole);
            QString guid = qv.toString();
            docF->showDocumentationforUid(guid);
            setCentralWidget(docF);
            docF->initiateStyleChange(docF->s_lastStyle);
            return;
        }

        // 3. the SQL of 1 item
        if(current->parent() && current->parent() == foundVersion->getGui()->getFinalSqlItem())
        {
            // user clicked on a SQL item
            SqlForm* frm = new SqlForm(foundVersion, m_workspace->currentProjectsEngine(), this);
            QVariant qv = current->data(0, Qt::UserRole);
            QString name = qv.toString();
            SqlSource* ent = foundVersion->getSqlSourceEntityWithGuid(name);
            if(ent == 0)
            {   // hm.. this shouldn't be
                return;
            }
            frm->setSqlSource(ent);
            frm->presentSql(m_workspace->currentProject(), ent, (MainWindow::showSomething)&MainWindow::showNothing);
            setCentralWidget(frm);
            return;
        }

        QString classUid = nullUid;
        if(obj)
        {
            classUid = obj->getClassUid().toString();
            classUid = classUid.toUpper();
            foundVersion = obj->version();
        }

        if(current == foundVersion->getGui()->getTablesItem())
        {// we have clicked on the Tables item (i.e. the list of tables)
            showNamedObjectList(&MainWindow::showTableWithGuid, foundVersion->getTables(),
                                IconFactory::getTableIcon(),
                                Workspace::getInstance()->currentProjectIsOop()?"Table Templates":"Tables");
        }
        else
        if(current == foundVersion->getGui()->getTableInstancesItem())
        {// we have clicked on the Table instances item (i.e. the list of table instances)
            showNamedObjectList(&MainWindow::showTableInstanceWithGuid, foundVersion->getTableInstances(), IconFactory::getTabinstIcon(), "Tables");
        }
        else
        if(current == m_workspace->currentProject()->getLocation())
        {
            showProjectDetails();
        }
        else
        if(current == foundVersion->getGui()->getFinalSqlItem())
        {// user clicked on the final SQL item
            SqlForm* frm = foundVersion->getGui()->getSqlForm();
            frm->setSqlSource(0);
            frm->presentSql(m_workspace->currentProject(), foundVersion);
            setCentralWidget(frm);
        }
        else
        if(current == foundVersion->getGui()->getDiagramsItem())
        {// we have clicked on the Diagrams item (i.e. the list of diagrams)
            showNamedObjectList(&MainWindow::showDiagramWithGuid, foundVersion->getDiagrams(), IconFactory::getDiagramIcon(), "Diagrams");
        }
        else
        if(current == foundVersion->getGui()->getViewsItem())
        {// we have clicked on the Views item (i.e. the list of views)
            showNamedObjectList(&MainWindow::showViewWithGuid, foundVersion->getViews(), IconFactory::getViewIcon(), "Views");
        }
        else
        if(current == foundVersion->getGui()->getProceduresItem())
        {// we have clicked on the Procedures item (i.e. the list of procedures)
            showNamedObjectList(&MainWindow::showProcedureWithGuid, foundVersion->getProcedures(), IconFactory::getProcedureIcon(), "Procedures");
        }
        else
        if(current == foundVersion->getGui()->getFunctionsItem())
        {// we have clicked on the Procedures item (i.e. the list of procedures)
            showNamedObjectList(&MainWindow::showFunctionWithGuid, foundVersion->getFunctions(), IconFactory::getFunctionTreeIcon(), "Functions");
        }
        else
        if(current == foundVersion->getGui()->getTriggersItem())
        {// we have clicked on the Triggers item (i.e. the list of triggers)
            showNamedObjectList(&MainWindow::showTriggerWithGuid, foundVersion->getTriggers(), IconFactory::getTriggerIcon(), "Triggers");
        }
        else
        if(current == foundVersion->getGui()->getDtsItem())
        {// we have clicked on the Data Types item
            showDataTypesList(foundVersion);
        }
        else
        {
            // the user clicked on a data type, this must be performed before all of them, since hey are more hierachical
            UserDataType* udt = 0;
            if((udt = dynamic_cast<UserDataType*>(UidWarehouse::instance().getElement(uid))))
            {
                NewDataTypeForm* frm = new NewDataTypeForm(foundVersion, DT_INVALID, m_workspace->currentProjectsEngine(), this);
                frm->focusOnName();
                frm->setDataType(udt);
                setCentralWidget(frm);
            }
            else
            {
                showObjectWithGuid(uid);
            }
        }
    }
}

Procedure* MainWindow::createStoredProcedureInVersion(bool guided, Version* v)
{
    ProcedureForm* frm = v->getGui()->getProcedureForm(MODE_PROCEDURE, guided);
    Procedure* p = new Procedure(NameGenerator::getUniqueName(v, (itemGetter)&Version::getProcedure, QString("proc")),
                                 QUuid::createUuid().toString(), v, guided);
    frm->setProcedure(p);
    frm->initSql();
    v->addProcedure(p, false);
    v->getGui()->createProcedureTreeEntry(p);
    m_guiElements->getProjectTree()->setCurrentItem(p->getLocation());
    setCentralWidget(frm);
    return p;
}

void MainWindow::onNewViewWithSqlFromPopup()
{
    Version* ver = getLastRightClickedVersion();
    View* v = new View(true, QUuid::createUuid().toString(), ver);

    ver->addView(v, false);
    ver->getGui()->createViewTreeEntry(v);

    m_nvf = new NewViewForm(ver, false, 0, this);
    m_nvf->setSqlSource(v);
    m_nvf->setView(v);
    m_nvf->presentSql(Workspace::getInstance()->currentProject(), ver);

    setCentralWidget(m_nvf);
}

void MainWindow::onNewViewFromPopup()
{
    Version* ver = getLastRightClickedVersion();
    View* v = new View(false, QUuid::createUuid().toString(), ver);
    m_nvf = 0;
    ver->addView(v, false);
    ver->getGui()->createViewTreeEntry(v);
    rerenderQuery(v->getQuery());
}


void MainWindow::onNewTriggerFromPopup()
{
    Workspace::getInstance()->createTrigger(getLastRightClickedVersion());
}

Function* MainWindow::createStoredFunction(bool guided, Version* v)
{
    ProcedureForm* frm = v->getGui()->getProcedureForm(MODE_FUNCTION, guided);
    Function* func = new Function(NameGenerator::getUniqueName(v, (itemGetter)&Version::getFunction, QString("func")),
                                  QUuid::createUuid().toString(), v, guided);
    frm->setProcedure(func);
    frm->initSql();
    v->addFunction(func, false);
    v->getGui()->createFunctionTreeEntry(func);
    m_guiElements->getProjectTree()->setCurrentItem(func->getLocation());
    setCentralWidget(frm);
    return func;
}

void MainWindow::newDiagramInVersion(Version* v)
{
    Diagram* dgram = new Diagram(v, QUuid::createUuid().toString());
    DiagramForm* df = new DiagramForm(v, dgram, this);
    v->addDiagram(dgram, false);
    setCentralWidget(df);
    dgram->setForm(df);
    onSaveDiagram(dgram, v);
    m_guiElements->getProjectTree()->setCurrentItem(dgram->getLocation());
}

Version* MainWindow::getLastRightClickedVersion()
{
    if(m_guiElements->getProjectTree()->getLastRightclickedItem() == 0)
    {
        return Workspace::getInstance()->workingVersion();
    }

    ContextMenuEnabledTreeWidgetItem* item = m_guiElements->getProjectTree()->getLastRightclickedItem();
    m_guiElements->getProjectTree()->setLastRightclickedItem(0);

    QVariant qv = item->data(0, Qt::UserRole);
    QString diagramsUid = qv.toString();

    return UidWarehouse::instance().getVersionForUid(diagramsUid);
}

void MainWindow::onNewDiagram()
{
    newDiagramInVersion(Workspace::getInstance()->workingVersion());
}

void MainWindow::newTableInVersion(Version* v)
{
    m_ui->action_NewTable->setDisabled(true);

    NewTableForm* frm = v->getGui()->getTableFormForNewTable();
    frm->focusOnName();
    m_guiElements->getProjectTree()->setCurrentItem(frm->getTable()->getLocation());
    setCentralWidget(frm);
    m_ui->action_NewTable->setDisabled(false);
}

void MainWindow::onNewTable()
{
    newTableInVersion(Workspace::getInstance()->workingVersion());
}

void MainWindow::showNewDataTypeWindow(int a, Version *v)
{
    NewDataTypeForm* frm = new NewDataTypeForm(v, (DT_TYPE)a, m_workspace->currentProjectsEngine(), this);
    frm->hideDeleteButton();
    frm->focusOnName();
    m_guiElements->getProjectTree()->setCurrentItem(0);
    setCentralWidget(frm);
}

void MainWindow::onNewDataType()
{
    QWidget* w = m_ui->mainToolBar->widgetForAction(m_ui->action_NewDataType);
    m_ui->action_NewDataType->menu()->popup(w->mapToGlobal(QPoint(0, w->height())));
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

        if(!fileName.endsWith(".dmx", Qt::CaseInsensitive))
        {
            fileName += ".dmx";
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
        sol->projects()[i]->createTreeItem(m_guiElements);
        sol->projects()[i]->populateTreeItem(m_guiElements);

        QVector<MajorVersion*>& majorVersions = sol->projects()[i]->getMajorVersions();

        for(int j=0; j<majorVersions.size(); j++)
        {
            // and now the patches
            const QVector<Patch*>& patches = majorVersions[j]->getPatches();
            if(patches.size())
            {
                m_guiElements->getPatchesDock()->show();
                MainWindow::instance()->addDockWidget(Qt::LeftDockWidgetArea, m_guiElements->getPatchesDock());
            }

            for(int k=0; k<patches.size(); k++)
            {
                ContextMenuEnabledTreeWidgetItem* patchItem = m_guiElements->createNewPatchItem(patches.at(k));
                m_guiElements->populatePathcItem(patchItem, patches.at(k));
            }

            majorVersions[j]->getGui()->getVersionItem()->treeWidget()->collapseItem(majorVersions[j]->getGui()->getVersionItem());
        }

    }
}

void MainWindow::doLoadSolution(const QString& fileName, bool splashVisible)
{
    if(!m_workspace->loadSolution(fileName))
    {
        QMessageBox::critical (this, tr("Error"), tr("Cannot load the solution."), QMessageBox::Ok);
        if(m_btndlg && splashVisible)
        {
            showSplashwindow();
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

    Workspace::getInstance()->workingVersion()->getGui()->collapseDTEntries();

    ProjectDetailsForm* prjDetailsForm = new ProjectDetailsForm(this);
    prjDetailsForm->setProject(m_workspace->currentSolution()->currentProject(), fileName);
    setCentralWidget(prjDetailsForm);

    setWindowTitle(tr("DDM - [") + m_workspace->currentSolution()->getName() + tr("] - [") + fileName + tr("]"));
    m_lastLoadedProject = fileName;
    connectActionsFromPopupMenus();
    enableActions();

    if(m_btndlg)
    {
        m_btndlg->hide();
    }
    m_btndlg = 0;
}

void MainWindow::onOpenSolution()
{
    bool splashVisible = false;
    if(m_btndlg && m_btndlg->isVisible())
    {
        hideSplashwindow();
        splashVisible = true;
    }

    QString fileName = QFileDialog::getOpenFileName(this,  tr("Open solution"), "", tr("DDM solution files (*.dmx);;All files (*.*)"));
    if(fileName.length() == 0)
    {
        if(splashVisible)
        {
            showSplashwindow();
            m_btndlg->fixButtons();
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
    m_ui->action_ConnectionsTree->setChecked(m_connectionGuiElements->getConnectionsTreeDock()->isVisible());
    m_ui->action_Validate->setEnabled(true);
    m_ui->action_NewView->setEnabled(true);
    m_ui->action_NewDatabaseConnection->setEnabled(true);
    m_ui->action_Preferences->setEnabled(true);
    m_ui->action_NewProcedure->setEnabled(true);
    m_ui->action_DeleteUnusuedDatatypes->setEnabled(true);
    m_ui->action_DeploymentScript->setVisible(false);
    m_ui->action_NewTrigger->setEnabled(true);
    m_ui->action_NewFunction->setEnabled(true);
    m_ui->action_UpdateDatabase->setEnabled(true);
    m_ui->action_SolutionProperties->setEnabled(true);

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
    m_ui->action_NewProcedure->setMenu(ContextMenuCollection::getInstance()->getCreateNewProcedurePopupMenu());
    m_ui->action_NewFunction->setMenu(ContextMenuCollection::getInstance()->getCreateNewFunctionPopupMenu());
    m_ui->action_Deploy->setMenu(ContextMenuCollection::getInstance()->getDeployPopupMenu());

}

void MainWindow::connectActionsFromPopupMenus()
{
    MainWindowSlotHouse::instance()->connectSignalsToSlots();

    QObject::connect(ContextMenuCollection::getInstance()->getAction_RemoveTable(), SIGNAL(triggered()), this, SLOT(onDeleteTableFromPopup()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_TableAddColumn(), SIGNAL(triggered()), this, SLOT(onTableAddColumnFromPopup()));
    if(m_workspace->currentProjectIsOop())
    {
        // the table popup
        QObject::connect(ContextMenuCollection::getInstance()->getAction_SpecializeTable(), SIGNAL(triggered()), this, SLOT(onSpecializeTableFromPopup()));
        QObject::connect(ContextMenuCollection::getInstance()->getAction_InstantiateTable(), SIGNAL(triggered()), this, SLOT(onInstantiateTableFromPopup()));

        // now the table instance popup
        QObject::connect(ContextMenuCollection::getInstance()->getAction_DeleteTableInstance(), SIGNAL(triggered()), this, SLOT(onDeleteTableInstanceFromPopup()));
        QObject::connect(ContextMenuCollection::getInstance()->getAction_RenameTableInstance(), SIGNAL(triggered()), this, SLOT(onRenameInstanceFromPopup()));

        // table instances popup
        QObject::connect(ContextMenuCollection::getInstance()->getAction_AddTableInstance(), SIGNAL(triggered()), this, SLOT(onNewTableInstanceFromPopup()));

    }
    else
    {
        ContextMenuCollection::getInstance()->getAction_SpecializeTable()->setVisible(false);
        ContextMenuCollection::getInstance()->getAction_InstantiateTable()->setVisible(false);
    }
    QObject::connect(ContextMenuCollection::getInstance()->getAction_DuplicateTable(), SIGNAL(triggered()), this, SLOT(onDuplicateTableFromPopup()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_CopyTable(), SIGNAL(triggered()), this, SLOT(onCopyTableFromPopup()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_PasteTable(), SIGNAL(triggered()), this, SLOT(onPasteTableFromPopup()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_DeleteDataType(), SIGNAL(triggered()), this, SLOT(onDeleteDatatypeFromPopup()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_DuplicateDataType(), SIGNAL(triggered()), this, SLOT(onDuplicateDatatypeFromPopup()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_RenameDiagram(), SIGNAL(triggered()), this, SLOT(onRenameDiagramFromPopup()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_GotoIssueLocation(), SIGNAL(triggered()), this, SLOT(onGotoIssueLocation()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_IgnoreIssue(), SIGNAL(triggered()), this, SLOT(onIgnoreIssue()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_IgnoreIssuesFromThisTable(), SIGNAL(triggered()), this, SLOT(onIgnoreIssuesOfATable()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_CreateViewUsingQueryBuilder(), SIGNAL(triggered()), this, SLOT(onNewView()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_CreateViewUsingSql(), SIGNAL(triggered()), this, SLOT(onNewViewWithSql()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_ReleaseMajorVersion(), SIGNAL(triggered()), this, SLOT(onReleaseMajorVersion()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_FinalizePatch(), SIGNAL(triggered()), this, SLOT(finalizePatch()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_RenamePatch(), SIGNAL(triggered()), this, SLOT(renamePatch()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_Undelete(), SIGNAL(triggered()), this, SLOT(onUndeleteSomething()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_CompareTable(), SIGNAL(triggered()), this, SLOT(onCompareTables()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_AddTrigger(), SIGNAL(triggered()), this, SLOT(onNewTriggerFromPopup()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_AddView(), SIGNAL(triggered()), this, SLOT(onNewViewFromPopup()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_AddViewWithSql(), SIGNAL(triggered()), this, SLOT(onNewViewWithSqlFromPopup()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_InitiatePatch(), SIGNAL(triggered()), this, SLOT(onInitiatePatch()));

    QObject::connect(m_ui->action_NewProcedure, SIGNAL(triggered()), MainWindowSlotHouse::instance(), SLOT(onNewProcedure()));
    QObject::connect(m_ui->action_NewFunction, SIGNAL(triggered()), MainWindowSlotHouse::instance(), SLOT(onNewFunction()));
    QObject::connect(m_ui->action_Deploy, SIGNAL(triggered()), MainWindowSlotHouse::instance(), SLOT(onDeploy()));

    ContextMenuCollection::getInstance()->getAction_BrowsedTableInject()->setVisible(true);
}

template <class T>
void MainWindow::showNamedObjectList(showSomething s, const QVector<T*> items, const QIcon& icon, const QString& title)
{
    NamedObjectListingForm* lstForm = new NamedObjectListingForm(this, s, icon, title);
    lstForm->populateObjects(items);
    setCentralWidget(lstForm);
}

void MainWindow::onReleaseMajorVersion()
{
    MajorVersion * m = 0;
    if(m_guiElements->getProjectTree()->getLastRightclickedItem() != 0)
    {
        ContextMenuEnabledTreeWidgetItem* item = m_guiElements->getProjectTree()->getLastRightclickedItem();
        m_guiElements->getProjectTree()->setLastRightclickedItem(0);
        QVariant qv = item->data(0, Qt::UserRole);
        QString name = qv.toString();
        Version* v = Workspace::getInstance()->currentProject()->getVersionNamed(name);
        if(v)
        {
            m = dynamic_cast<MajorVersion*>(v);
            if(m)
            {
                bool go = false;
                if(!m->canSafelyRelease())
                {
                    QStringList offendingTables = m->getTablesWithEmptyPks();
                    QString tables = "";
                    for(int i=0; i<offendingTables.size(); i++)
                    {
                        tables +="<li>" + offendingTables[i];
                    }
                    if(QMessageBox::question(this, tr("Warning"), tr("It is not safe to release this version since the following tables have no primary key:<ul>")
                                             + tables + tr("</ul><p>Do you still want to release?"),
                                             QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
                    {
                        go = true;
                    }
                }
                else
                {
                    go = true;
                }

                if(!go)
                {
                    return;
                }
                Version* newVersion = Workspace::getInstance()->currentProject()->releaseMajorVersion();
                v->getGui()->getVersionItem()->setPopupMenu(ContextMenuCollection::getInstance()->getReleasedVersionPopupMenu());

                if(!newVersion->getProject()->oopProject())
                {
                    ContextMenuEnabledTreeWidgetItem* item = newVersion->getGui()->getTableInstancesItem();
                    item->setHidden(true);
                    newVersion->getGui()->getTablesItem()->setText(0, tr("Tables"));
                }

                // see if the central form is a TableComparisonForm?
                TableComparisonForm* atcf = dynamic_cast<TableComparisonForm*>(centralWidget());
                if(atcf)
                {
                    TableComparisonForm* tcf = new TableComparisonForm(TableComparisonForm::COMPARE_VERSIONS);
                    tcf->setFromVersion(atcf->getFromVersion());
                    tcf->setToVersion(atcf->getToVersion());
                    setCentralWidget(tcf);
                }
                else
                {
                    setCentralWidget(new ProjectDetailsForm(this));
                }
            }
        }
    }
}

bool MainWindow::showObjectWithGuid(const QString & guid)
{
    ObjectWithUid* element = UidWarehouse::instance().getElement(guid);
    QString classUid = element->getClassUid().toString().toUpper();
    Version* v = UidWarehouse::instance().getVersionForUid(guid);
    Q_ASSERT(v != 0);

    if(m_showMethods.getMapping().contains(classUid))
    {
        showObjectWithGuidAndVersion(v, guid, m_showMethods.getMapping()[classUid], false);
        return true;
    }
    else
    {
        return false;
    }
}

bool MainWindow::showRepoObjectWithGuid(const QString & guid)
{
    ObjectWithUid* owuid = UidWarehouse::instance().getElement(guid);
    if(owuid)
    {
        RepositoryEntityForm* repoEF = new RepositoryEntityForm(this, dynamic_cast<Entity*>(owuid));
        setCentralWidget(repoEF);
    }
    return true;
}


void MainWindow::finallyDoLockLikeOperation(bool reLocking, const QString& guid)
{
    ObjectWithUid* element = UidWarehouse::instance().getElement(guid);
    Version* v = UidWarehouse::instance().getVersionForUid(guid);
    if(v == 0 || element == 0)
    {
        return;
    }

    if(Workspace::getInstance()->doLockLikeOperation(reLocking, element, v))
    {
        createPatchElement(v, element, guid, reLocking);
        showObjectWithGuid(guid);
        m_guiElements->getProjectTree()->setCurrentItem( (dynamic_cast<TreeItem*>(element)->getLocation()));
    }
}

void MainWindow::updatePatchElementToReflectState(Version *v, ObjectWithUid *element, const QString &guid, int state)
{
    NamedItem *ni = dynamic_cast<NamedItem*>(element);
    if(!ni) return;
    m_guiElements->updateItemForPatchWithState(v->getWorkingPatch(), element->getClassUid().toString(), guid, ni->getName(), state);
}

void MainWindow::createPatchElement(Version* v, ObjectWithUid * element, const QString& guid, bool reLocking)
{
    NamedItem *ni = dynamic_cast<NamedItem*>(element);
    if(!ni)
    {
        return;
    }

    if(!m_guiElements->getPatchesDock()->isVisible() && !reLocking)
    {
        m_guiElements->getPatchesDock()->show();
        addDockWidget(Qt::LeftDockWidgetArea, m_guiElements->getPatchesDock());
    }

    if(!reLocking)
    {
        v->getWorkingPatch()->addElement(guid);
        m_guiElements->createNewItemForPatch(v->getWorkingPatch(), element->getClassUid().toString(), guid, ni->getName());
    }
    else
    {
        v->getWorkingPatch()->removeElement(guid);
        m_guiElements->removeItemForPatch(v->getWorkingPatch(), guid);
    }
}

void MainWindow::doLockLikeOperation(bool reLocking)
{
    ObjectWithUid* obj = getRightClickedObject<ObjectWithUid>();
    if(obj)
    {
        finallyDoLockLikeOperation(reLocking, obj->getObjectUid().toString());
    }
}

void MainWindow::onAbout()
{
    bool wasBtndlg = false;
    if(m_btndlg && m_btndlg->isVisible())
    {
        hideSplashwindow();
        wasBtndlg = true;
    }

    AboutBoxDialog* about = new AboutBoxDialog(this);
    about->setModal(true);
    about->exec();

    if(m_btndlg && wasBtndlg)
    {
        showSplashwindow();
    }
}

bool MainWindow::onSaveDiagram(Diagram* dgram, Version* v)
{
    if(!dgram->isSaved())
    {
        v->getGui()->createDiagramTreeEntry(dgram);
    }
    else
    {
        dgram->setDisplayText(dgram->getName());
    }

    return true;
}

void MainWindow::onSaveAs()
{
    saveProject(true);
}

void MainWindow::onInstantiateTableFromPopup()
{

    Table* table =getRightClickedObject<Table>();
    if(table == 0)  // shouldn't be ...
    {
        return;
    }
    Version* v = table->version();
    if(!v)
    {
        return;
    }
    v->getGui()->updateForms();
    TableInstance* tinst = instantiateTable(table->getName(), QStringList(), v);
    showTableInstanceWithGuid(v, tinst->getObjectUid().toString());
}

void MainWindow::onSpecializeTableFromPopup()
{
    Table* table = getRightClickedObject<Table>();
    if(table)
    {
        Table* specializedTable = new Table(table->version(), QUuid::createUuid().toString());
        specializedTable->initializeFor(Workspace::getInstance()->currentProjectsEngine(), QUuid(uidTable));

        specializedTable->setName(table->getName() + "_specialized");
        specializedTable->setParent(table);
        specializedTable->setParentUid(table->getObjectUid().toString());

        ContextMenuEnabledTreeWidgetItem* newTblsItem = table->version()->getGui()->createTableTreeEntry(specializedTable, table->getLocation()) ;

        // add to the project itself
        table->version()->addTable(specializedTable, false);

        // set the link to the tree
        specializedTable->setLocation(newTblsItem);

        // now open the new table for to show this table
        showExistingTable(specializedTable, table->version());
    }
}

void MainWindow::onTableAddColumnFromPopup()
{
    Table* table =  getRightClickedObject<Table>();
    if(!table)
    {
        return;
    }

    NewTableForm* frm = showExistingTable(table, table->version());
    frm->focusOnNewColumnName();
}

template <class T>
T* MainWindow::getRightClickedObject()
{
    ContextMenuEnabledTreeWidgetItem* item = getLastRightClickedTreeItem();
    if(item == 0)
    {
        return 0;
    }

    QVariant qv = item->data(0, Qt::UserRole);
    QString uid = qv.toString();
    VersionElement* obj = dynamic_cast<VersionElement*>(UidWarehouse::instance().getElement(uid));
    if(obj == 0)
    {
        return 0;
    }

    Version* v = obj->version();
    if(v == 0)
    {
        return 0;
    }

    T* t =  dynamic_cast<T*>(obj);
    if(t == 0)
    {
        return 0;
    }

    return t;
}

Version* MainWindow::getRightClickedVersion()
{
    ContextMenuEnabledTreeWidgetItem* item = m_guiElements->getProjectTree()->getLastRightclickedItem();
    if(item == 0)
    {
        return 0;
    }
    m_guiElements->getProjectTree()->setLastRightclickedItem(0);
    QVariant qv = item->data(0, Qt::UserRole);
    QString name = qv.toString();
    Version* v = Workspace::getInstance()->currentProject()->getVersionNamed(name);
    return v;
}

Connection* MainWindow::getRightClickedConnection()
{
    if(m_connectionGuiElements->getConnectionsTree()->getLastRightclickedItem() != 0)
    {
        ContextMenuEnabledTreeWidgetItem* item = m_connectionGuiElements->getConnectionsTree()->getLastRightclickedItem();
        m_connectionGuiElements->getConnectionsTree()->setLastRightclickedItem(0);

        QVariant qv = item->data(0, Qt::UserRole);
        QString n = qv.toString();
        Connection* c = ConnectionManager::instance()->getConnection(n);
        return c;
    }
    return 0;
}

void MainWindow::onDeleteTableInstanceFromPopup()
{
    TableInstance* tinst =  getRightClickedObject<TableInstance>();
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
                m_guiElements->getProjectTree()->setLastRightclickedItem(0);
                return;
            }
        }

        Version* v = tinst->version();
        v->deleteTableInstance(tinst);

        const QVector<TableInstance*>& otherTinsts = v->getTableInstances();
        for(int i=0; i<otherTinsts.size(); i++)
        {
            QVector<QString> fksFailures;
            otherTinsts[i]->finalizeFkMappings(fksFailures);
        }

        showNamedObjectList(&MainWindow::showTableInstanceWithGuid, v->getTableInstances(), IconFactory::getTabinstIcon(), QObject::tr("Tables"));
    }
}

void MainWindow::onDeleteTableFromPopup()
{
    Table* tab =  getRightClickedObject<Table>();
    if(tab)
    {
        Version* v = tab->version();
        const QVector<TableInstance*> tinsts = tab->getTableInstances();
        QString stinsts = "";
        for(int i=0; i<tinsts.size(); i ++)
        {
            stinsts += "<li>" + tinsts[i]->getName() + "</li>";
        }
        if(QMessageBox::question(this, tr("Are you sure?"), tr("Really delete ") + tab->getName() + "?" + tr(" This will automatically delete the table instance of it too: <ul>") + stinsts + "</ul>", QMessageBox::Yes | QMessageBox::No) !=  QMessageBox::Yes)
        {
            return;
        }
        if(tab->hasSpecializedTables())
        {
            QMessageBox::warning(0, QObject::tr("Specialized tables were found"),
                                 QObject::tr("Cannot delete this table since there are tables specialized from it. Firstly remove those tables and then delete this one."), QMessageBox::Ok);
            return;

        }

        QWidget* w = centralWidget();
        if(v->deleteTable(tab))
        {
            if(dynamic_cast<DiagramForm*>(w))
            {
                dynamic_cast<DiagramForm*>(w)->paintDiagram();
            }
            else
            {
                showNamedObjectList(&MainWindow::showTableWithGuid, v->getTables(),
                                    Workspace::getInstance()->currentProjectIsOop()?IconFactory::getTabinstIcon():IconFactory::getTableIcon(),
                                    Workspace::getInstance()->currentProjectIsOop()?tr("Table Templates"):tr("Tables"));
                v->getGui()->getTablesItem()->treeWidget()->clearSelection();
                v->getGui()->getTablesItem()->setSelected(true);
            }
        }
        v->getGui()->updateForms();
    }
}

void MainWindow::onCopyTableFromPopup()
{
    Table* tab =  getRightClickedObject<Table>();
    if(tab)
    {
        tab->copy();
    }
}

void MainWindow::onPasteTableFromPopup()
{
    ContextMenuEnabledTreeWidgetItem* item = getLastRightClickedTreeItem();
    if(item == 0)
    {
        return;
    }

    QVariant qv = item->data(0, Qt::UserRole);
    QString uid = qv.toString();
    Version* v = UidWarehouse::instance().getVersionForUid(uid);
    if(!v) return;

    Table* tab = Workspace::getInstance()->pasteTable(v);
    if(tab)
    {
        v->getGui()->createTableTreeEntry(tab);
        if(tab->getParent())
        {
            QTreeWidgetItem* p = tab->getLocation();
            p->parent()->removeChild(p);
            tab->getParent()->getLocation()->addChild(p);
        }
        // add the SQL item but only if it's not an oop project
        if(!m_workspace->currentProjectIsOop())
        {
            ContextMenuEnabledTreeWidgetItem* sqlItm = new ContextMenuEnabledTreeWidgetItem(v->getGui()->getFinalSqlItem(), QStringList(tab->getName() + ".sql"));
            sqlItm->setIcon(0, IconFactory::getTablesIcon());
            sqlItm->setData(0, Qt::UserRole, tab->getObjectUid().toString());
        }

        if(v->lockState() == LockableElement::LOCKED)
        {
            createPatchElement(v, tab, tab->getObjectUid().toString(), false);
            v->getWorkingPatch()->addNewElement(tab->getObjectUid().toString()); // this will be a new element ...
            updatePatchElementToReflectState(v, tab, tab->getObjectUid().toString(), 1);
        }
        v->getGui()->updateForms();
    }
}

ContextMenuEnabledTreeWidgetItem* MainWindow::getLastRightClickedTreeItem()
{
    if(m_guiElements->getProjectTree()->getLastRightclickedItem() == 0 && m_guiElements->getPatchesTree()->getLastRightclickedItem() == 0)
    {
        return 0;
    }

    ContextMenuEnabledTreeWidgetItem* item = m_guiElements->getProjectTree()->getLastRightclickedItem();
    if(item == 0)
    {
        item = m_guiElements->getPatchesTree()->getLastRightclickedItem();
        if(item == 0)
        {
            return 0;
        }
        else
        {
            m_guiElements->getPatchesTree()->setLastRightclickedItem(0);
        }
    }
    else
    {
        m_guiElements->getProjectTree()->setLastRightclickedItem(0);
    }

    return item;
}

void MainWindow::onDuplicateTableFromPopup()
{
    Table* tab = getRightClickedObject<Table>();

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
        dupped->getLocation()->setData(0, Qt::UserRole, QVariant(dupped->getObjectUid().toString()));
        // add the SQL item but only if it's not an oop project
        if(!m_workspace->currentProjectIsOop())
        {
            ContextMenuEnabledTreeWidgetItem* sqlItm = new ContextMenuEnabledTreeWidgetItem(m_workspace->workingVersion()->getGui()->getFinalSqlItem(), QStringList(dupped->getName() + ".sql"));
            sqlItm->setIcon(0, IconFactory::getTablesIcon());
            sqlItm->setData(0, Qt::UserRole, dupped->getObjectUid().toString());
        }

        m_workspace->workingVersion()->getGui()->updateForms();
    }
}

void MainWindow::createTableInstancesInVersion(Version* v)
{
    // Duplicate with the one below
    CreateTableInstancesDialog* newDialog = new CreateTableInstancesDialog(this);
    for(int i=0; i<v->getTables().size(); i++)
    {
        newDialog->addTable(v->getTables()[i]->getName());
    }

    if(newDialog->exec() == QDialog::Accepted)
    {
        QStringList items = newDialog->getSelectedTables();
        for(int i=0; i<items.size(); i++)
        {
            instantiateTable(items.at(i), items, v);
        }

        // This is OK, the button defaultly adds to the working version
        v->getGui()->updateForms();
        m_guiElements->getProjectTree()->setCurrentItem(v->getGui()->getTableInstancesItem());
        v->getGui()->getTableInstancesItem()->setExpanded(true);
    }
}

void MainWindow::onNewTableInstanceFromPopup()
{
    if(m_guiElements->getProjectTree()->getLastRightclickedItem() == 0)
    {
        return;
    }

    ContextMenuEnabledTreeWidgetItem* item = m_guiElements->getProjectTree()->getLastRightclickedItem();
    m_guiElements->getProjectTree()->setLastRightclickedItem(0);

    QVariant qv = item->data(0, Qt::UserRole);
    QString tablesUid = qv.toString();

    Version* v = UidWarehouse::instance().getVersionForUid(tablesUid);
    createTableInstancesInVersion(v);
}

void MainWindow::onNewTableInstance()
{
    createTableInstancesInVersion(Workspace::getInstance()->workingVersion());
}

void MainWindow::specificDeploymentCallback(const QString &connName)
{
    QStringList t;
    t << connName;
    DeploymentInitiator* dinit = new DeploymentInitiator();
    dinit->doDeployment(m_workspace->workingVersion(), t, false);
}

void MainWindow::onDeployHovered()
{
    QMenu* deployPopupMenu = ContextMenuCollection::getInstance()->getDeployPopupMenu();
    deployPopupMenu->clear();
    const QVector<Connection*> cons = ConnectionManager::instance()->connections();
    for(int i=0; i< cons.size(); i++)
    {
        QAction* act = new QAction(this);
        act->setText(cons.at(i)->getName() + " - " + cons.at(i)->getFullLocation());
        act->setData(cons.at(i)->getName());
        act->setIcon(IconFactory::getConnectionStateIcon(cons.at(i)->getState()));
        deployPopupMenu->addAction(act);
        QObject::connect(act, SIGNAL(triggered()),
                new DynamicActionHandlerforMainWindow(cons.at(i)->getName(), this,
                                                      (MainWindow::dynamicAction)&MainWindow::specificDeploymentCallback),
                SLOT(called()));
    }
}

void MainWindow::onNewTableInstanceHovered()
{
    QMenu* createTableInstancesPopup = ContextMenuCollection::getInstance()->getCreateTableInstancesPopupMenu();
    createTableInstancesPopup->clear();

    Version* v = m_workspace->workingVersion();
    if(!v) return;
    if(m_workspace && m_workspace->currentSolution() && m_workspace->currentProject() && v) // OK, using the working version here
    {
        for(int i=0; i<v->getTables().size(); i++)
        {
            QAction * actionToAdd = new QAction(this);
            actionToAdd->setText(v->getTables()[i]->getName());
            QIcon icon(IconFactory::getTablesIcon());
            actionToAdd->setData(QVariant(v->getTables()[i]->getName()));
            actionToAdd->setIcon(icon);
            createTableInstancesPopup->addAction(actionToAdd);
            QObject::connect(actionToAdd, SIGNAL(triggered()),
                             new DynamicActionHandlerforMainWindow(v->getTables()[i]->getName(), this,
                                    (MainWindow::dynamicAction)&MainWindow::instantiateTableCallback), SLOT(called()));
        }
    }
}

void MainWindow::instantiateTableCallback(const QString &tabName)
{
    instantiateTable(tabName, QStringList(), m_workspace->workingVersion());
}

TableInstance *MainWindow::instantiateTable(const QString& tabName, QStringList othersTablesBeingInstantiated, Version *v, bool ref, Table* referencingTable, TableInstance* becauseOfThis)
{
    Version* cVersion = v;
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

    QVariant a(tinst->getObjectUid());
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
                instantiateTable(tbl->getName(), othersTablesBeingInstantiated, v, true, tinst->table(), tinst);
            }
        }
        else
        {
            // just add a remark to the tinst that this table instance was also be isntantiated because of this
            tinst->addInstantiatedTableInstance(cVersion->getTableInstance(tbl->getName()));
        }
    }
    return tinst;
}

void MainWindow::updateSourceCodeWidget()
{
    QWidget* mainwidget = centralWidget();
    if(mainwidget)
    {
        SourceCodePresenterWidget* scw = dynamic_cast<SourceCodePresenterWidget*> (mainwidget);
        if(scw)
        {
            scw->updateSql(m_workspace->currentProject(), scw->version());
        }
    }
}

void MainWindow::onPreferences()
{
    PreferencesDialog* dlg = new PreferencesDialog(this);
    dlg->setModal(true);
    dlg->exec();

    updateSourceCodeWidget();
}

void MainWindow::onRenameInstanceFromPopup()
{
    TableInstance* tinst =  getRightClickedObject<TableInstance>();

    if(tinst)
    {
        SimpleTextInputDialog* dlg = new SimpleTextInputDialog(this, tr("Enter the new name"), tinst->getName());
        if(dlg->exec() == QDialog::Accepted)
        {
            QString t = dlg->getText();
            if(tinst->version()->getTableInstance(t))
            {
                QMessageBox::critical(this, tr("Error"), tr("You can have only one table instance called ") + t, QMessageBox::Ok);
                return;
            }
            tinst->setName(t);
            tinst->setDisplayText(t);
            tinst->getSqlLocation()->setText(0, t);
            const QVector<TableInstance*>& otherTinsts = tinst->version()->getTableInstances();
            for(int i=0; i<otherTinsts.size(); i++)
            {
                QVector<QString> fksFailures;
                otherTinsts[i]->finalizeFkMappings(fksFailures);
            }

            m_guiElements->updateItemForPatchWithState(tinst->version()->getWorkingPatch(), uidTableInstance, tinst->getObjectUid().toString(), t, 2);
            updateSourceCodeWidget();
        }
    }
}

void MainWindow::onRenameDiagramFromPopup()
{
    Diagram* dgr =  getRightClickedObject<Diagram>();
    if(dgr)
    {
        SimpleTextInputDialog* dlg = new SimpleTextInputDialog(this, tr("Enter the new name"), dgr->getName());
        if(dlg->exec() == QDialog::Accepted)
        {
            QString t = dlg->getText();
            dgr->setName(t);
            dgr->setDisplayText(t);
            QVariant a(t);

            m_workspace->workingVersion()->getGui()->updateForms();
        }
    }
}

void MainWindow::onDestroyed()
{
    QApplication::exit();
}

void MainWindow::onCloseSolution()
{
    if(!m_workspace->hasCurrentSolution())
    {
        return;
    }

    m_workspace->onCloseSolution();

    freeGuiElements();
    showConnections();

    disableActions();
}

void MainWindow::disableActions()
{
    m_ui->action_NewDataType->setEnabled(false);
    m_ui->action_NewTable->setEnabled(false);
    m_ui->action_NewDiagram->setEnabled(false);
    m_ui->action_Save->setEnabled(false);
    m_ui->action_SaveAs->setEnabled(false);
    m_ui->action_Deploy->setEnabled(false);
    m_ui->action_ProjectTree->setEnabled(false);
    m_ui->action_ProjectTree->setChecked(false);
    m_ui->action_Datatypes_Tree->setChecked(false);
    m_ui->action_Datatypes_Tree->setEnabled(false);
    m_ui->action_ConnectionsTree->setEnabled(false);
    m_ui->action_ConnectionsTree->setChecked(m_connectionGuiElements->getConnectionsTreeDock()->isVisible());
    m_ui->action_Validate->setEnabled(false);
    m_ui->action_NewView->setEnabled(false);
    m_ui->action_NewDatabaseConnection->setEnabled(false);
    m_ui->action_Preferences->setEnabled(false);
    m_ui->action_NewProcedure->setEnabled(false);
    m_ui->action_DeleteUnusuedDatatypes->setEnabled(false);
    m_ui->action_DeploymentScript->setVisible(false);
    m_ui->action_NewTrigger->setEnabled(false);
    m_ui->action_NewFunction->setEnabled(false);
    m_ui->action_UpdateDatabase->setEnabled(false);
    m_ui->action_SolutionProperties->setEnabled(false);

    m_ui->action_NewTableInstance->setVisible(true);
    m_ui->action_NewTableInstance->setEnabled(false);

    m_ui->action_NewTableInstance->setMenu(0);
    m_ui->action_NewDataType->setMenu(0);
    m_ui->action_NewView->setMenu(0);
    m_ui->action_Deploy->setMenu(0);

}

void MainWindow::onDeleteDatatypeFromPopup()
{
    UserDataType* udt = getRightClickedObject<UserDataType>();
    if(udt)
    {
        QString dtName = udt->getName();
        Version* v = udt->version();
        if(QMessageBox::question(this, tr("Are you sure?"), tr("Really delete ") + dtName + "?", QMessageBox::Yes | QMessageBox::No) ==  QMessageBox::Yes)
        {
            if(Workspace::getInstance()->deleteDataType(udt))
            {
                delete udt->getLocation();
                showDataTypesList(v);
            }
        }
    }
}

void MainWindow::onInjectBrowsedTable()
{
    if(m_connectionGuiElements->getConnectionsTree()->getLastRightclickedItem() == 0)
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

    QVariant v = m_connectionGuiElements->getConnectionsTree()->getLastRightclickedItem()->data(0, Qt::UserRole);
    if(v.isValid())
    {
        QString s = v.toString();
        if(s.startsWith(browsedTablePrefix))
        {
            QString cname = s.mid(s.indexOf("?") + 1);
            Connection *c = ConnectionManager::instance()->getConnection(cname);
            if(!c)
            {
                return;
            }

            QString tab = s.left(s.indexOf("?")).mid(2);
            QString validName = NameGenerator::getUniqueName(Workspace::getInstance()->currentProject()->getWorkingVersion(), (itemGetter)&Version::getTable, tab);

            // and reverse engineer the table with the database engine of the menu ... hm...
            Table* t = c->getEngine()->reverseEngineerTable(c, validName, Workspace::getInstance()->currentProject(), true, m_workspace->workingVersion());

            // finalize the table
            t->setName(validName);

            // add it to the version, GUI, etc...
            m_workspace->workingVersion()->addTable(t, false);
            m_workspace->workingVersion()->getGui()->createTableTreeEntry(t, m_workspace->workingVersion()->getGui()->getTablesItem());
            showTableWithGuid(m_workspace->workingVersion(), t->getObjectUid().toString());
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
    if(m_connectionGuiElements->getConnectionsTree()->getLastRightclickedItem() == 0)
    {
        return;
    }

    m_connectionGuiElements->getConnectionsTree()->clearSelection();
    m_connectionGuiElements->getConnectionsTree()->getLastRightclickedItem()->setSelected(true);
    onConnectionItemDoubleClicked(m_connectionGuiElements->getConnectionsTree()->getLastRightclickedItem(), 0);
}

void MainWindow::onRecreateConnection()
{
    Connection* c = getRightClickedConnection();
    if(c)
    {
        // clear the tree
        while(c->getLocation()->childCount()) c->getLocation()->removeChild(c->getLocation()->child(0));
        // drop and create the DB
        if(!c->getEngine()->dropDatabase(c))
        {
            QMessageBox::critical(this, tr("Error"), tr("Cannot drop: ") + c->getEngine()->getLastError(), QMessageBox::Ok);
            return;
        }
        if(!c->getEngine()->createDatabase(c))
        {
            QMessageBox::critical(this, tr("Error"), tr("Cannot create: ") + c->getEngine()->getLastError(), QMessageBox::Ok);
            c->setState(FAILED);
            c->getLocation()->setIcon(0, IconFactory::getConnectionStateIcon(c->getState()));
            return;
        }
        c->setState(DID_NOT_TRY);
        c->getLocation()->setIcon(0, IconFactory::getConnectionStateIcon(c->getState()));
    }
}

void MainWindow::onEditConnection()
{
    Connection* c = getRightClickedConnection();
    if(c)
    {
        bool w = false;
        if(m_btndlg && m_btndlg->isVisible())
        {
            hideSplashwindow();
            w = true;
        }

        InjectSqlDialog* ij = new InjectSqlDialog(0, 0, 0, "");
        ij->setModal(true);
        ij->populateConnectionDetails(c);
        if(ij->exec()  == QDialog::Accepted)
        {
            if(dynamic_cast<MySqlConnection*>(c))
            {
                dynamic_cast<MySqlConnection*>(c)->resetTo(ij->getName(), ij->getHost(), ij->getUser(),
                                                           ij->getPassword(), ij->getDatabase(), ij->getPort(), true, ij->getAutoConnect());
            }
            else
            if(dynamic_cast<SqliteConnection*>(c))
            {
                dynamic_cast<SqliteConnection*>(c)->resetTo(ij->getFileName());
            }
            else
            if(dynamic_cast<CUBRIDConnection*>(c))
            {
                dynamic_cast<CUBRIDConnection*>(c)->resetTo(ij->getName(), ij->getHost(), ij->getUser(),
                                                            ij->getPassword(), ij->getDatabase(), ij->getPort(), true, ij->getAutoConnect());
            }
            c->setDisplayText(c->getName());
            c->getLocation()->setText(1, c->getFullLocation());
            QVariant var(c->getName());
            c->getLocation()->setData(0, Qt::UserRole, var);
            c->getLocation()->setIcon(0, IconFactory::getConnectionStateIcon(c->getState()));
        }

        if(m_btndlg && w)
        {
            showSplashwindow();
        }
    }
}

void MainWindow::onDeleteConnection()
{
    Connection* c = getRightClickedConnection();
    if(c)
    {
        ConnectionManager::instance()->deleteConnection(c->getName());
        delete c->getLocation();
        c->setState(DELETED);
    }
}

void MainWindow::tryBrowseConnection(Connection *c)
{
    if(c->tryConnect())
    {
        m_connectionGuiElements->resetConnectionTreeForConnection(c);
        m_connectionGuiElements->populateConnectionTreeForConnection(c);
    }
}

void MainWindow::onBrowseConnection()
{
    Connection* c = getRightClickedConnection();
    if(c)
    {
        if(c->getState() == CONNECTED)
        {
            m_connectionGuiElements->resetConnectionTreeForConnection(c);
            m_connectionGuiElements->populateConnectionTreeForConnection(c);
        }
        else
        {
            tryBrowseConnection(c);
        }
        c->getLocation()->setExpanded(true);
    }
}

void MainWindow::onRefreshConnection()
{
    Connection* c = getRightClickedConnection();
    if(c)
    {
        Workspace::getInstance()->refreshConnection(c);
    }
}

void MainWindow::onDropConnection()
{
    Connection* c = getRightClickedConnection();
    if(c)
    {
        bool w = false;
        if(m_btndlg && m_btndlg->isVisible())
        {
            hideSplashwindow();
            w = true;
        }

        if(QMessageBox::question(this, tr("Drop?"), tr("Do you really want to drop ") + c->getName() + tr("? This is irreversible"), QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
        {
            c->getEngine()->dropDatabase(c);
            ConnectionManager::instance()->deleteConnection(c->getName());
            delete c->getLocation();
            c->setState(DROPPED);
        }

        if(m_btndlg && w)
        {
            showSplashwindow();
        }
    }
}

void MainWindow::hideSplashwindow()
{
    if(m_btndlg && m_btndlg->isVisible())
    {
#ifdef Q_WS_WIN
        Qt::WindowFlags flags = m_btndlg->windowFlags();
        m_btndlg->setWindowFlags(flags ^ (Qt::SplashScreen |Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint));
#endif
        m_btndlg->hide();
    }
}

void MainWindow::showSplashwindow()
{
    if(m_btndlg && !m_btndlg->isVisible())
    {
#ifdef Q_WS_WIN
        Qt::WindowFlags flags = m_btndlg->windowFlags();
        m_btndlg->setWindowFlags(flags | Qt::SplashScreen |Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
#endif
        m_btndlg->show();
    }
}

void MainWindow::onConnectionItemDoubleClicked(QTreeWidgetItem* item,int)
{
    QVariant v = item->data(0, Qt::UserRole);
    if(v.isValid())
    {
        QString s = v.toString();
        QString cname = s.mid(s.indexOf("?") + 1);
        Connection *c = ConnectionManager::instance()->getConnection(cname);
        if(!c) return;
        QString refObj = s.left(s.indexOf("?")).mid(2);

        if(s.startsWith(browsedTablePrefix) || s.startsWith(browsedViewPrefix))
        {
            hideSplashwindow();

            BrowseTableForm* frm = BrowseTableForm::instance(this, c, refObj, s.startsWith(browsedTablePrefix)?BROWSE_TABLE:BROWSE_VIEW);
            setCentralWidget(frm);
            return;
        }

        if(s.startsWith(browsedProcPrefix))
        {
            hideSplashwindow();

            BrowseTableForm* frm = BrowseTableForm::instance(this, c, refObj, BROWSE_PROCEDURE);
            setCentralWidget(frm);
            return;
        }

        if(s.startsWith(browsedFuncPrefix))
        {
            hideSplashwindow();

            BrowseTableForm* frm = BrowseTableForm::instance(this, c, refObj, BROWSE_FUNCTION);
            setCentralWidget(frm);
            return;
        }

        if(s.startsWith(browsedTriggerPrefix))
        {
            hideSplashwindow();

            BrowseTableForm* frm = BrowseTableForm::instance(this, c, refObj, BROWSE_TRIGGER);
            setCentralWidget(frm);
            return;
        }

        c = ConnectionManager::instance()->getConnection(s);
        if(c) tryBrowseConnection(c);
    }
}

void MainWindow::onConnectConnection()
{
    Connection* c = getRightClickedConnection();
    if(c)
    {
        if(c->getState() == CONNECTED) return;
        if(c->tryConnect())
        {
            c->getLocation()->setIcon(0, IconFactory::getConnectedDatabaseIcon());
        }
        else
        {

            c->getLocation()->setIcon(0, IconFactory::getUnConnectedDatabaseIcon());
            bool w = false;
            if(m_btndlg && m_btndlg->isVisible())
            {
                hideSplashwindow();
                w = true;
            }

            QMessageBox::critical(this, tr("Error"), c->getLastError(), QMessageBox::Ok);

            if(m_btndlg && w)
            {
                showSplashwindow();
            }
        }
    }
}

void MainWindow::onDuplicateDatatypeFromPopup()
{
    UserDataType* udt = getRightClickedObject<UserDataType>();
    if(udt)
    {
        QString x = udt->getName();
        Version* v = udt->version();
        UserDataType* dup = v->duplicateDataType(x);

        ContextMenuEnabledTreeWidgetItem* newDTItem = v->getGui()->createDataTypeTreeEntry(dup);

        m_guiElements->getProjectTree()->expandItem(newDTItem);
        m_guiElements->getProjectTree()->scrollToItem(newDTItem);

        // resize to look better
        m_guiElements->getProjectTree()->resizeColumnToContents(0);
        m_guiElements->getProjectTree()->resizeColumnToContents(1);
    }
}

Version* MainWindow::getVersionOfLastRightClickedElement()
{
    if(m_guiElements->getProjectTree()->getLastRightclickedItem() == 0)
    {
        // since we might get in here from the buttons too
        return Workspace::getInstance()->workingVersion();
    }

    ContextMenuEnabledTreeWidgetItem* item = m_guiElements->getProjectTree()->getLastRightclickedItem();
    m_guiElements->getProjectTree()->setLastRightclickedItem(0);

    QVariant qv = item->data(0, Qt::UserRole);
    QString diagramsUid = qv.toString();

    Version* ver = UidWarehouse::instance().getVersionForUid(diagramsUid);
    return ver;
}

void MainWindow::onGotoIssueLocation()
{
    Table* tab = 0;
    if(!m_guiElements->getIssuesTree()->getLastRightclickedItem()) return;
    QVariant a = m_guiElements->getIssuesTree()->getLastRightclickedItem()->data(0, Qt::UserRole);
    QString issueName = a.toString();
    // TODO: Associate the issue tree locations to the specific version too!
    Issue* selectedIssue = m_workspace->workingVersion()->getIssue(issueName); // TODO: Fix this too!
    IssueOriginator* source = selectedIssue->getOriginator();
    Column* testCol = dynamic_cast<Column*>(source);
    if(testCol)
    {
        tab = testCol->getTable();
        NewTableForm*frm = showExistingTable(tab, tab->version());
        frm->showColumn(testCol);
        frm->focusOnNewColumnName();
        frm->setCurrentColumn(testCol);
    }
    Connection* c = dynamic_cast<Connection*>(source);
    if(c)
    {
        InjectSqlDialog* ij = new InjectSqlDialog(0, 0, 0, "");
        ij->setModal(true);
        ij->populateConnectionDetails(c);
        if(ij->exec()  == QDialog::Accepted)
        {
            if(dynamic_cast<MySqlConnection*>(c))
            {
                dynamic_cast<MySqlConnection*>(c)->resetTo(ij->getName(), ij->getHost(), ij->getUser(),
                                                            ij->getPassword(), ij->getDatabase(), ij->getPort(), true, ij->getAutoConnect());
            }
            else
            if(dynamic_cast<SqliteConnection*>(c))
            {
                dynamic_cast<SqliteConnection*>(c)->resetTo(ij->getFileName());
            }
        }
    }
}

void MainWindow::onIgnoreIssuesOfATable()
{
    if(!m_guiElements->getIssuesTree()->getLastRightclickedItem()) return;
    QVariant a = m_guiElements->getIssuesTree()->getLastRightclickedItem()->data(0, Qt::UserRole);
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
    ContextMenuEnabledTreeWidgetItem* lastRclick = m_guiElements->getIssuesTree()->getLastRightclickedItem();
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
    lblStatus->setText(tr("Deploying"));
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

static QString getErrorForTag(const QString &src, const QString& tag)
{
    int errIdx = src.indexOf(tag);
    if(errIdx == -1) return "";
    errIdx += tag.length();
    QString result = "";
    while(errIdx < src.length() - 2)
    {
        result += src.at(errIdx);
        errIdx ++;
        if(src[errIdx] == '<' && src[errIdx + 1] == '!') return result;
    }

    return result;
}

void MainWindow::onDeploymentFinished(Deployer *d)
{
    if(d->hadErrors())
    {
        QMap<QString, QStringList> uids;
        QMap<QString, QString> errors = d->getErrors(uids);
        m_guiElements->getIssuesDock()->show();
        for(QMap<QString, QString>::iterator it = errors.begin(); it != errors.end(); it++)
        {
            QString reason = it.value();
            QString theError = getErrorForTag(reason, "<!-- DBE: -->");
            QString drvError = getErrorForTag(reason, "<!-- DRV: -->");
            QString code = getErrorForTag(reason, "<!-- NR: -->");
            QString guid = getErrorForTag(reason, "<!-- UID:");

            Issue* iss = IssueManager::getInstance().createConnectionIssue(ConnectionManager::instance()->getConnection(it.key()),
                                                                           theError + "/"  + drvError + tr(" Code: ") +  code);
            ObjectWithUid* obj = UidWarehouse::instance().getElement(guid);
            if(obj)
            {
                iss->getLocation()->setIcon(3, IconFactory::getIconForClassUid(obj->getClassUid().toString()));
            }
            if(!lblStatus)
            {
                createStatusLabel();
            }
            lblStatus->setStyleSheet("QLabel { background-color : red; color : white; }");
            QString t = QObject::tr("Deployment failed at ");
            QTime now = QTime::currentTime();
            QDate nowd = QDate::currentDate();
            t += nowd.toString() + " - " + now.toString();
            lblStatus->setText(t);
        }
    }
    else
    {
        QString t = QObject::tr("Deployment succeeded at ");
        QTime now = QTime::currentTime();
        QDate nowd = QDate::currentDate();
        t += nowd.toString() + " - " + now.toString();
        if(!lblStatus)
        {
            createStatusLabel();
        }
        lblStatus->setText(t);
        lblStatus->setStyleSheet("QLabel { background-color : green; color : white; }");
    }
}

void MainWindow::onViewProjectTree()
{
    bool t = m_guiElements->getProjectDock()->isVisible() ;
    m_ui->action_ProjectTree->setChecked(!t);
    m_guiElements->getProjectDock()->setVisible(!t);
}

void MainWindow::onViewConnectionsTree()
{
    showConnections();
}

void MainWindow::onViewDatatypesTree()
{
    bool t = m_guiElements->getProjectTree()->isVisible() ;
    m_ui->action_Datatypes_Tree->setChecked(!t);
    m_guiElements->getProjectTree()->setVisible(!t);
}

void MainWindow::onReverseEngineerWizardNextPage(int cpage)
{
    QMap<int, ReverseEngineerWizard::collectOperation> ops = DatabaseEngineManager::instance().getCollectOperations(m_revEngWizard->getDbTypeName());
    if(! ops.empty() )
    {
        if(ops.contains(cpage))
        {
            (m_revEngWizard->*ops[cpage])();
        }
    }
}

void MainWindow::onReverseEngineerWizardAccept()
{
    Project* p = m_workspace->currentProject();
    DatabaseEngine* engine = m_workspace->currentProjectsEngine();
    bool createDTForColumns = !m_revEngWizard->createDataTypesForColumns();

    ReverseEngineerer* revEng = 0;

    revEng = new ReverseEngineerer(createDTForColumns, engine, p,
                                   QSharedPointer<Connection>(m_revEngWizard->getConnectionforDb()),
                                   m_revEngWizard->getTablesToReverse(),
                                   m_revEngWizard->getViewsToReverse(),
                                   m_revEngWizard->getProceduresToReverse(),
                                   m_revEngWizard->getFunctionsToReverse(),
                                   m_revEngWizard->getTriggersToReverse(),
                                   0);

    createStatusLabel();
    lblStatus->setText(QObject::tr("Reverse engineering started"));

    connect(revEng, SIGNAL(done(ReverseEngineerer*)), this, SLOT(onReverseEngineeringFinished(ReverseEngineerer*)));
    revEng->reverseEngineer();
}

void MainWindow::onReverseEngineeringFinished(ReverseEngineerer*)
{
    lblStatus->setText(QObject::tr("Reverse engineering finished"));
    m_workspace->workingVersion()->getGui()->populateTreeItems();
}

void MainWindow::onHelp()
{
    if(m_btndlg && m_btndlg->isVisible())
    {
        hideSplashwindow();
        delete m_btndlg;
        m_btndlg = 0;
    }

    HelpWindow* hw = HelpWindow::instance();
    hw->showHelp(QString("/doc/main.html"));
    hw->show();
}

void MainWindow::onNewConnection()
{
    bool wasVisible = false;
    if(m_btndlg && m_btndlg->isVisible())
    {
        hideSplashwindow();
        wasVisible = true;
    }
    m_workspace->createNewConnection();
    if(wasVisible)
    {
        showSplashwindow();
        m_btndlg->fixButtons();
    }
}

void MainWindow::onValidate()
{
    m_ui->statusBar->showMessage(tr("Validating"), 0);
    m_workspace->workingVersion()->validateVersion(true);
    m_ui->statusBar->showMessage(tr("Validation finished"), 1000);
    if(m_workspace->workingVersion()->getIssues().size() == 0)
    {
        QMessageBox::information(this, tr("Congratulations"),
                tr("DDM has run a full validation on your database and it seems there were no issues found. Good work."),
                QMessageBox::Ok);
    }
}

void MainWindow::onNewViewWithSql()
{
    m_nvf = new NewViewForm(m_workspace->workingVersion(), false, 0, this);
    View* v = new View(true, QUuid::createUuid().toString(), m_workspace->workingVersion());
    Workspace::getInstance()->workingVersion()->addView(v, false);
    Workspace::getInstance()->workingVersion()->getGui()->createViewTreeEntry(v);

    m_nvf->setSqlSource(v);
    m_nvf->setView(v);
    m_nvf->presentSql(Workspace::getInstance()->currentProject(), v->version());

    setCentralWidget(m_nvf);
}

void MainWindow::onNewView()
{
    View* v = new View(false, QUuid::createUuid().toString(), m_workspace->workingVersion());
    v->initializeFor(m_workspace->currentProject()->getEngine(), QUuid(uidView));

    m_nvf = 0;
    Workspace::getInstance()->workingVersion()->addView(v, false);
    Workspace::getInstance()->workingVersion()->getGui()->createViewTreeEntry(v);
    m_guiElements->getProjectTree()->setCurrentItem(v->getLocation());
    rerenderQuery(v->getQuery());
}

void MainWindow::onNewTrigger()
{
    Workspace::getInstance()->createTrigger(Workspace::getInstance()->workingVersion());
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

    bool created = false;
    if(m_nvf == 0)
    {
        m_nvf = new NewViewForm(dynamic_cast<View*>(q->getSourceEntity())->version(), true, q->getHelper(), this);
        m_nvf->setSqlSource(q->getSourceEntity());
        m_nvf->setView(dynamic_cast<View*>(q->getSourceEntity()));
        q->getHelper()->setScene(m_nvf->getScene());
        created = true;
    }

    m_nvf->setGraphicsItem(q->getGraphicsItem());
    m_nvf->presentSql(Workspace::getInstance()->currentProject(), dynamic_cast<View*>(q->getSourceEntity())->version());
    if(created) setCentralWidget(m_nvf);
    m_nvf->scrollTo(cx, cy);
}

void MainWindow::closeEvent( QCloseEvent * )
{
    QApplication::exit();
}

void MainWindow::changeEvent(QEvent *e)
{
    if( e->type() == QEvent::WindowStateChange )
    {
        if( isMinimized() )
        {
            if(m_btndlg && m_btndlg->isVisible())
            {
                m_btndlg->hide();
                m_splashWasVisible = true;
            }
        }
        else
        {
            if(m_splashWasVisible)
            {
                m_btndlg->show();
                m_splashWasVisible = false;
            }
        }
    }
}

void MainWindow::onDeleteUnusedDatatypes()
{
    m_workspace->workingVersion()->cleanupDataTypes();
}

void MainWindow::onSqlQueryInConnection()
{
    Connection* c = getRightClickedConnection();
    if(c)
    {
        hideSplashwindow();
        Workspace::getInstance()->refreshConnection(c);
        BrowseTableForm* frm = BrowseTableForm::instance(this, c, "", CREATE_SCRIPT);
        setCentralWidget(frm);
        frm->focusOnTextEdit();
    }
}

void MainWindow::finalizePatch()
{
    Patch* p = getRightClickedObject<Patch>();
    if(p)
    {
        QString tempError;
        bool t = Workspace::getInstance()->finalizePatch(p, tempError);
        if(!t)
        {
            QMessageBox::critical(this, tr("Could not finalize"), tempError);
        }
    }
}

void MainWindow::renamePatch()
{
    if(m_guiElements->getPatchesTree()->getLastRightclickedItem() != 0)
    {
        ContextMenuEnabledTreeWidgetItem* item = m_guiElements->getPatchesTree()->getLastRightclickedItem();
        m_guiElements->getPatchesTree()->setLastRightclickedItem(0);

        QVariant qv = item->data(0, Qt::UserRole);
        QString uid = qv.toString();
        Patch* p = dynamic_cast<Patch*>(UidWarehouse::instance().getElement(uid));
        if(!p) return;

        SimpleTextInputDialog* dlg = new SimpleTextInputDialog(this, tr("Enter the new name"), p->getName());
        if(dlg->exec() == QDialog::Accepted)
        {
            QString t = dlg->getText();
            p->setName(t);
            p->setDisplayText(t);
        }
    }
}

void MainWindow::onUndeleteSomething()
{
    ObjectWithUid* obj = getRightClickedObject<ObjectWithUid>();
    Version* v = UidWarehouse::instance().getVersionForUid(obj->getObjectUid().toString());
    QString tempError;
    Version::PatchTreeRemovalStatus removeFromTree = v->undeleteObject(obj->getObjectUid().toString(), false, tempError);
    if(removeFromTree != Version::DO_NOT_REMOVE_FROM_PATCH_TREE_FAILURE)
    {
        m_guiElements->removeItemForPatch(v->getWorkingPatch(), obj->getObjectUid().toString());
        showObjectWithGuid(obj->getObjectUid().toString());
    }
    else
    {
        QMessageBox::critical(this, tr("Error"), tempError);
    }
}

void MainWindow::onInitiatePatch()
{
    if(m_guiElements->getProjectTree()->getLastRightclickedItem() != 0)
    {
        Version* v =getRightClickedVersion();
        if(v)
        {
            if(!m_guiElements->getPatchesDock()->isVisible())
            {
                m_guiElements->getPatchesDock()->show();
                addDockWidget(Qt::LeftDockWidgetArea, m_guiElements->getPatchesDock());
            }
            v->getWorkingPatch();
        }
    }
}

void MainWindow::onCompareTables()
{
    Table* table =getRightClickedObject<Table>();
    if(table == 0)  // shouldn't be ...
    {
        return;
    }

    TableComparisonForm* tcf = new TableComparisonForm(TableComparisonForm::COMPARE_TABLES);
    tcf->setLeftTable(table);
    tcf->setRightTable(table);

    // populate the tree of the form and show it
    tcf->populateTree();
    setCentralWidget(tcf);
}

void MainWindow::onUpdateDb()
{
    TableComparisonForm* tcf = new TableComparisonForm(TableComparisonForm::COMPARE_VERSIONS);
    setCentralWidget(tcf);
}

void MainWindow::showDataTypesList(Version* foundVersion)
{
    DataTypesListForm* dtLst = new DataTypesListForm(this);
    dtLst->feedInDataTypes(foundVersion->getDataTypes());
    setCentralWidget(dtLst);
}


void MainWindow::onRepoItemClicked(QTreeWidgetItem* itm ,int)
{
    if(m_btndlg && m_btndlg->isVisible())
    {
        hideSplashwindow();
    }

    QString txt = itm->text(0);
    if(txt == QObject::tr("Roles"))
    {

        RoleListingForm* rlf = new RoleListingForm(this);
        setCentralWidget(rlf);
    }
    else
    {
        QString uid = itm->data(0, Qt::UserRole).toString();
        if(uid.isEmpty()) return;
        if(uid.at(0) == '{')
        {
            showRepoObjectWithGuid(uid);
        }
    }
}

// TODO: This is alittel bit ugly, refactor with the one below
void MainWindow::createTableInConnection(Connection* c, bool alreadyTried)
{
    bool create = true;
    if(!alreadyTried && !c->tryConnect())
    {
        if(m_btndlg && m_btndlg->isVisible())
        {
            hideSplashwindow();
            delete m_btndlg;
            m_btndlg = 0;
        }

        create = false;
        if(QMessageBox::question(this, tr("Error"),
                                 tr("Cannot connect to: <b>") + c->getName() + "</b>. Do yo still want to create the table?",
                                 QMessageBox::Yes, QMessageBox::No
                                 ==
                                 QMessageBox::Yes
                                 ))
        {
            create = true;
        }
    }

    if(m_btndlg && m_btndlg->isVisible())
    {
        hideSplashwindow();
        delete m_btndlg;
        m_btndlg = 0;
    }

    if(create)
    {
        BrowseTableForm* btf = BrowseTableForm::instance(this, c, "", NEW_TABLE_IN_DB);
        setCentralWidget(btf);
    }

}

void MainWindow::onConnectionCreateTable()
{
    Connection* c = getRightClickedConnection();
    if(c)
    {
        bool create = true;
        if(!c->tryConnect())
        {
            if(m_btndlg && m_btndlg->isVisible())
            {
                hideSplashwindow();
                delete m_btndlg;
                m_btndlg = 0;
            }

            create = false;
            QString msg = tr("Cannot connect to: <b>") +
                    c->getName() +
                    "</b>. Do yo still want to create the table?";

            if(QMessageBox::question(this, tr("Error"), msg, QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
            {
                create = true;
            }
        }

        if(create)
        {
            Workspace::getInstance()->refreshConnection(c);
            createTableInConnection(c, true);
        }
    }
}

void MainWindow::onActionTriggered()
{
    QAction *act = qobject_cast<QAction*>(sender());

    if(act)
    {
        QString sact = act->data().toString();
        if(sact == "TEST")
        {
            onConnectConnection();
        }

        if(sact == "QUERY")
        {
            onSqlQueryInConnection();
        }

        if(sact == "DROP")
        {
            onDropConnection();
        }

        if(sact == "RECREATE")
        {
            onRecreateConnection();
        }

        if(sact == "BROWSE")
        {
            onBrowseConnection();
        }

        if(sact == "REFRESH")
        {
            onRefreshConnection();
        }

        if(sact == "EDIT")
        {
            onEditConnection();
        }

        if(sact == "DELETE")
        {
            onDeleteConnection();
        }

        if(sact == "NEWTABLE")
        {
            onConnectionCreateTable();
        }
    }
}

MainWindow::ClassUidsShowMethod::ClassUidsShowMethod()
{
    mapping.insert(uidTable, (showSomething)&MainWindow::showTableWithGuid);
    mapping.insert(uidTableInstance, (showSomething)&MainWindow::showTableInstanceWithGuid);
    mapping.insert(uidDiagram, (showSomething)&MainWindow::showDiagramWithGuid);
    mapping.insert(uidProcedure, (showSomething)&MainWindow::showProcedureWithGuid);
    mapping.insert(uidFunction, (showSomething)&MainWindow::showFunctionWithGuid);
    mapping.insert(uidTrigger, (showSomething)&MainWindow::showTriggerWithGuid);
    mapping.insert(uidView, (showSomething)&MainWindow::showViewWithGuid);
}
