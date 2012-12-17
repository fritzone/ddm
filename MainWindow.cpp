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
#include "strings.h"
#include "Workspace.h"
#include "VersionGuiElements.h"
#include "InjectSqlDialog.h"
#include "reveng_ReverseEngineerWizard.h"
#include "IssueManager.h"
#include "NewViewForm.h"
#include "qbr_SelectQuery.h"
#include "core_View.h"
#include "gui_HelpWindow.h"
#include "core_Connection.h"
#include "core_ConnectionManager.h"
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

#include <QtGui>

MainWindow* MainWindow::m_instance = 0;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), m_ui(new Ui::MainWindow),
    m_btndlg(0), m_workspace(Workspace::getInstance()), m_revEngWizard(0), m_nvf(0),
    lblStatus(0), m_splashWasVisible(false),
    m_guiElements(0), m_connectionGuiElements(0)
{
    m_ui->setupUi(this);
    m_instance = this;
    Configuration::instance();
    ConnectionManager::instance();

    m_connectionGuiElements = new ConnectionGuiElements();
    m_connectionGuiElements->createGuiElements();
    m_ui->action_ConnectionsTree->setChecked(true);
    showConnections();

    m_ui->action_NewDatabaseConnection->setEnabled(true);
    showMaximized();
    setWindowTitle(tr("DDM - [No Solution]"));

    m_btndlg = new MainWindowButtonDialog();
    m_btndlg->showMe();
    //m_btndlg->raise();

    QApplication::instance()->installEventFilter(this);
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
        // based on the centralWidget select the desried help...
        // TODO: these things should come from the centralWidget somehow :)
        QWidget* c = centralWidget();
        QString s = QString("/doc/main.html");
        if(dynamic_cast<NewTableForm*>(c))
        {
            s = QString("/doc/tabl.html");
        }
        if(dynamic_cast<TableInstanceForm*>(c))
        {
            s = QString("/doc/tinst.html");
        }
        if(dynamic_cast<NewViewForm*>(c))
        {
            s = QString("/doc/view.html");
        }
        if(dynamic_cast<NewDataTypeForm*>(c))
        {
            s = QString("/doc/dtyp.html");
        }
        if(dynamic_cast<TriggerForm*>(c))
        {
            s = QString("/doc/trig.html");
        }
        if(dynamic_cast<ProcedureForm*>(c))
        {
            s = QString("/doc/proc.html");
        }
        if(dynamic_cast<DiagramForm*>(c))
        {
            s = QString("/doc/dgram.html");
        }
        HelpWindow* hw = HelpWindow::instance();
        hw->showHelp(s);
        hw->show();
    }
}

bool MainWindow::eventFilter(QObject *obj, QEvent *evt)
{
    if(obj==qApp && ( evt->type() == QEvent::ApplicationActivate
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


MainWindow::~MainWindow()
{
    ConnectionManager::instance()->shutDown();
    delete m_ui;
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
//    addDockWidget(Qt::LeftDockWidgetArea, m_guiElements->getGenDock());

    IssueManager::getInstance().setIssuesDock(m_guiElements->getIssuesDock());

    showMaximized();
}

void MainWindow::onNewSolution()
{
    bool w = false;
    if(m_btndlg && m_btndlg->isVisible())
    {
        m_btndlg->hide();

#ifdef Q_WS_WIN
        Qt::WindowFlags flags = m_btndlg->windowFlags();
        m_btndlg->setWindowFlags(flags ^ (Qt::SplashScreen |Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint));
#endif

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
#ifdef Q_WS_WIN
                    Qt::WindowFlags flags = m_btndlg->windowFlags();
                    m_btndlg->setWindowFlags(flags | Qt::SplashScreen |Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
#endif
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
#ifdef Q_WS_WIN
                    Qt::WindowFlags flags = m_btndlg->windowFlags();
                    m_btndlg->setWindowFlags(flags | Qt::SplashScreen |Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
#endif
                    m_btndlg->show();
                }
                return;
            }
        }

        Project* project = new Project(nprjdlg->getProjectName().toUpper(), nprjdlg->enableOOPFeatures());
        m_workspace->addProjectToSolution(m_workspace->currentSolution(), project);
        project->setEngine(nprjdlg->getDatabaseEngine());

        setupGuiForNewSolution();

        if(nprjdlg->getProjectType() == NewProjectDialog::PRJ_BINDTODATABASE)
        {
            InjectSqlDialog* injectDialog = new InjectSqlDialog(0, 0, 0, "");
            injectDialog->setupForBindToDatabase();
            injectDialog->setModal(true);
            if(injectDialog->exec() == QDialog::Accepted)
            {
                QString host = injectDialog->getHost();
                QString user = injectDialog->getUser();
                QString password = injectDialog->getPassword();
                int port = injectDialog->getPort();
                QString db = injectDialog->getDatabase();

                Connection c("temp", host, user, password, db, false, false, port);
                QString meta = nprjdlg->getDatabaseEngine()->getDbMetadata(&c);
                QDomDocument doc;
                QString lastErr;
                if(!doc.setContent(meta, &lastErr))
                {
//                    qDebug() << "Cannot set data:" << lastErr;
                }
                MajorVersion* mv = new MajorVersion;
                project->addMajorVersion(mv);
                DeserializationFactory::createMajorVersion(mv, project, project->getEngine(), doc, doc.firstChildElement().firstChildElement());
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
            QVector<UserDataType*> dts = m_workspace->loadDefaultDatatypesIntoCurrentSolution();

            for(int i=0; i<dts.size(); i++)
            {
                m_workspace->workingVersion()->getGui()->createDataTypeTreeEntry(dts.at(i));
            }

        }

        // expand the tree
        m_guiElements->getProjectTree()->expandAll();
        Workspace::getInstance()->workingVersion()->getGui()->collapseDTEntries();

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
            m_revEngWizard->setWizardStyle(QWizard::ClassicStyle);
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
#ifdef Q_WS_WIN
            Qt::WindowFlags flags = m_btndlg->windowFlags();
            m_btndlg->setWindowFlags(flags | Qt::SplashScreen |Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
#endif
            m_btndlg->show();
        }
    }
}

void MainWindow::onDTTreeClicked()
{
}

void MainWindow::showNothing(Version */*v*/, const QString &, bool /*focus*/ )
{

}

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

void MainWindow::showTableInstanceWithGuid(Version */*v*/, const QString &guid, bool focus)
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
        ProcedureForm* pf = v->getGui()->getProcedureForm(MODE_PROCEDURE);
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
        ProcedureForm* pf = v->getGui()->getProcedureForm(MODE_FUNCTION);
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
        TriggerForm* pf = v->getGui()->getTriggerForm();
        const QVector<Table*>& allTables = v->getTables();
        pf->feedInTables(allTables);
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
            v->getHelper()->setForm(this);
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
            classUid = obj->getClassUid();
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
            QMap<QString, showSomething> mapping;
            mapping.insert(uidTable, (showSomething)&MainWindow::showTableWithGuid);
            mapping.insert(uidTableInstance, (showSomething)&MainWindow::showTableInstanceWithGuid);
            mapping.insert(uidDiagram, (showSomething)&MainWindow::showDiagramWithGuid);
            mapping.insert(uidProcedure, (showSomething)&MainWindow::showProcedureWithGuid);
            mapping.insert(uidFunction, (showSomething)&MainWindow::showFunctionWithGuid);
            mapping.insert(uidTrigger, (showSomething)&MainWindow::showTriggerWithGuid);
            mapping.insert(uidView, (showSomething)&MainWindow::showViewWithGuid);

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

        ObjectWithUid* obj = UidWarehouse::instance().getElement(uid);
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
            SqlSourceEntity* ent = foundVersion->getSqlSourceEntityWithGuid(name);
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
            classUid = obj->getClassUid();
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
            DataTypesListForm* dtLst = new DataTypesListForm(this);
            dtLst->feedInDataTypes(foundVersion->getDataTypes());
            setCentralWidget(dtLst);
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

void MainWindow::onNewProcedureFromPopup()
{
    if(m_guiElements->getProjectTree()->getLastRightclickedItem() == 0)
    {
        return;
    }

    ContextMenuEnabledTreeWidgetItem* item = m_guiElements->getProjectTree()->getLastRightclickedItem();
    m_guiElements->getProjectTree()->setLastRightclickedItem(0);

    QVariant qv = item->data(0, Qt::UserRole);
    QString diagramsUid = qv.toString();

    Version* v = UidWarehouse::instance().getVersionForUid(diagramsUid);
//    qDebug() << v->getVersionText();

    // TODO: This is pure duplication with onNewProcedure except the version
    ProcedureForm* frm = v->getGui()->getProcedureForm(MODE_PROCEDURE);
    Procedure* p = new Procedure(NameGenerator::getUniqueName(v, (itemGetter)&Version::getProcedure, QString("proc")),
                                                               QUuid::createUuid().toString(), v);
    frm->setProcedure(p);
    frm->initSql();
    v->addProcedure(p, false);
    v->getGui()->createProcedureTreeEntry(p);

    setCentralWidget(frm);
}


void MainWindow::onNewViewWithSqlFromPopup()
{
    if(m_guiElements->getProjectTree()->getLastRightclickedItem() == 0)
    {
        return;
    }

    ContextMenuEnabledTreeWidgetItem* item = m_guiElements->getProjectTree()->getLastRightclickedItem();
    m_guiElements->getProjectTree()->setLastRightclickedItem(0);

    QVariant qv = item->data(0, Qt::UserRole);
    QString diagramsUid = qv.toString();

    Version* ver = UidWarehouse::instance().getVersionForUid(diagramsUid);
    // Ok. New views from the button go always to the working version
    m_nvf = new NewViewForm(ver, false, 0, this);
    View* v = new View(true, QUuid::createUuid().toString(), ver);
    ver->addView(v, false);
    ver->getGui()->createViewTreeEntry(v);

    m_nvf->setSqlSource(v);
    m_nvf->setView(v);
    m_nvf->presentSql(Workspace::getInstance()->currentProject(), ver);

    setCentralWidget(m_nvf);
}

void MainWindow::onNewViewFromPopup()
{
    if(m_guiElements->getProjectTree()->getLastRightclickedItem() == 0)
    {
        return;
    }

    ContextMenuEnabledTreeWidgetItem* item = m_guiElements->getProjectTree()->getLastRightclickedItem();
    m_guiElements->getProjectTree()->setLastRightclickedItem(0);

    QVariant qv = item->data(0, Qt::UserRole);
    QString diagramsUid = qv.toString();

    Version* ver = UidWarehouse::instance().getVersionForUid(diagramsUid);
    // Ok. New views from the button go always to the working version
    View* v = new View(false, QUuid::createUuid().toString(), ver);
    m_nvf = 0;
    v->getHelper()->setForm(this);
    ver->addView(v, false);
    ver->getGui()->createViewTreeEntry(v);

    rerenderQuery(v->getQuery());
}


void MainWindow::onNewTriggerFromPopup()
{

    if(m_guiElements->getProjectTree()->getLastRightclickedItem() == 0)
    {
        return;
    }

    ContextMenuEnabledTreeWidgetItem* item = m_guiElements->getProjectTree()->getLastRightclickedItem();
    m_guiElements->getProjectTree()->setLastRightclickedItem(0);

    QVariant qv = item->data(0, Qt::UserRole);
    QString diagramsUid = qv.toString();

    Version* v = UidWarehouse::instance().getVersionForUid(diagramsUid);

    // TODO: Duplication with onNewTrigger
    const QVector<Table*>& allTables = v->getTables();
    if(allTables.size() == 0)
    {
        QMessageBox::critical(this, tr("Cannot create a trigger when there are no tables"), tr("No tables defined"), QMessageBox::Ok);
        return;
    }
    TriggerForm* frm = v->getGui()->getTriggerForm();
    Trigger* trigger = new Trigger(NameGenerator::getUniqueName(v, (itemGetter)&Version::getTrigger, QString("trig")),
                                   QUuid::createUuid().toString(), v);
    frm->setTrigger(trigger);
    frm->initSql();
    frm->feedInTables(allTables);
    trigger->setTable(allTables.at(0)->getName());
    frm->feedInTriggerEvents(Workspace::getInstance()->currentProjectsEngine()->getTriggerEvents());
    frm->feedInTriggerTimes(Workspace::getInstance()->currentProjectsEngine()->getTriggerTimings());
    v->addTrigger(trigger, false);
    v->getGui()->createTriggerTreeEntry(trigger);
    setCentralWidget(frm);

}

void MainWindow::onNewFunctionFromPopup()
{
    if(m_guiElements->getProjectTree()->getLastRightclickedItem() == 0)
    {
        return;
    }

    ContextMenuEnabledTreeWidgetItem* item = m_guiElements->getProjectTree()->getLastRightclickedItem();
    m_guiElements->getProjectTree()->setLastRightclickedItem(0);

    QVariant qv = item->data(0, Qt::UserRole);
    QString diagramsUid = qv.toString();

    Version* v = UidWarehouse::instance().getVersionForUid(diagramsUid);
//    qDebug() << v->getVersionText();

    // TODO: This is pure duplication with onNewFunction except the version
    ProcedureForm* frm = v->getGui()->getProcedureForm(MODE_FUNCTION);
    Function* p = new Function(NameGenerator::getUniqueName(v, (itemGetter)&Version::getFunction, QString("func")),
                                                               QUuid::createUuid().toString(), v);
    frm->setProcedure(p);
    frm->initSql();
    v->addFunction(p, false);
    v->getGui()->createFunctionTreeEntry(p);

    setCentralWidget(frm);
}

void MainWindow::onNewDiagramFromPopup()
{
    if(m_guiElements->getProjectTree()->getLastRightclickedItem() == 0)
    {
        return;
    }

    ContextMenuEnabledTreeWidgetItem* item = m_guiElements->getProjectTree()->getLastRightclickedItem();
    m_guiElements->getProjectTree()->setLastRightclickedItem(0);

    QVariant qv = item->data(0, Qt::UserRole);
    QString diagramsUid = qv.toString();

    Version* v = UidWarehouse::instance().getVersionForUid(diagramsUid);
//    qDebug() << v->getVersionText();

    // TODO: This is pure duplication with onNewDiagram
    Diagram* dgram = new Diagram(v, QUuid::createUuid().toString());
    DiagramForm* df = new DiagramForm(v, dgram, this);
    v->addDiagram(dgram, false);
    setCentralWidget(df);
    dgram->setForm(df);
    onSaveDiagram(dgram, v);
}

void MainWindow::onNewTableFromPopup()
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

    // TODO: duplicate with below
    m_ui->action_NewTable->setDisabled(true);

    NewTableForm* frm = v->getGui()->getTableFormForNewTable();
    frm->focusOnName();
    m_guiElements->getProjectTree()->setCurrentItem(0);
    setCentralWidget(frm);
    m_ui->action_NewTable->setDisabled(false);

}

void MainWindow::onNewTable()
{
    m_ui->action_NewTable->setDisabled(true);
    NewTableForm* frm = Workspace::getInstance()->workingVersion()->getGui()->getTableFormForNewTable();
    frm->focusOnName();

    m_guiElements->getProjectTree()->setCurrentItem(frm->getTable()->getLocation());
    setCentralWidget(frm);
    m_ui->action_NewTable->setDisabled(false);
}

void MainWindow::showNewDataTypeWindow(int a, Version *v)
{
    NewDataTypeForm* frm = new NewDataTypeForm(v, (DT_TYPE)a, m_workspace->currentProjectsEngine(), this);
    frm->focusOnName();
    m_guiElements->getProjectTree()->setCurrentItem(0);
    setCentralWidget(frm);
}

void MainWindow::onNewDataType()
{
    showNewDataTypeWindow(DT_INVALID, m_workspace->workingVersion());
}

bool MainWindow::onSaveNewDataType(const QString& name, const QString& type, const QString& sqlType, const QString& size, const QString& defaultValue,
                             const QStringList& mvs, const QString& desc, bool unsi, bool canBeNull,  UserDataType* pudt, Version *v)
{
    if(name.length() == 0 || type.length() == 0 || sqlType.length() == 0)
    {
        QMessageBox::critical (this, tr("Error"), tr("Please specify all the required data"), QMessageBox::Ok);
        return false;
    }


    UserDataType* udt = new UserDataType(name, type, sqlType, size, defaultValue, mvs, unsi, desc, canBeNull, QUuid::createUuid().toString(), v);

    if(! m_workspace->currentProjectsEngine()->getDTSupplier()->isValid(udt))
    {
        QMessageBox::critical (this, tr("Error"), tr("This datatype requires a length"), QMessageBox::Ok);
        return false;
    }

    UserDataType* other = v->getDataType(name);

    if(other &&  other != pudt)
    {
        QMessageBox::critical (this, tr("Error"), tr("Only one datatype with the name ") + name + tr(" can exist in the project."), QMessageBox::Ok);
        return false;
    }

    // check if this is Saving an existing data type or updating a new one
    if(pudt)    // saving
    {
        *pudt = *udt;
        pudt->getLocation()->setIcon(0, IconFactory::getIconForDataType(pudt->getType()));
        pudt->setDisplayText(name);
        pudt->getLocation()->setText(1, pudt->sqlAsString());

        // updating the "data" of the tree item
        QVariant var;
        var.setValue(udt->getObjectUid().toString());
        pudt->getLocation()->setData(0, Qt::UserRole, var);
    }
    else        // new stuff
    {
        // add to the project itself
        v->addNewDataType(udt, false);

        // create the tree entry
        ContextMenuEnabledTreeWidgetItem* newDtItem = v->getGui()->createDataTypeTreeEntry(udt);

        m_guiElements->getProjectTree()->setCurrentItem(newDtItem);
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
    }
}

void MainWindow::doLoadSolution(const QString& fileName, bool splashVisible)
{
    if(!m_workspace->loadSolution(fileName))
    {
        QMessageBox::critical (this, tr("Error"), tr("Cannot load the solution."), QMessageBox::Ok);
        if(m_btndlg && splashVisible)
        {
#ifdef Q_WS_WIN
            Qt::WindowFlags flags = m_btndlg->windowFlags();
            m_btndlg->setWindowFlags(flags | Qt::SplashScreen |Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
#endif
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

    //m_guiElements->getProjectTree()->expandAll();
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
        m_btndlg->hide();
#ifdef Q_WS_WIN
        Qt::WindowFlags flags = m_btndlg->windowFlags();
        m_btndlg->setWindowFlags(flags ^ (Qt::SplashScreen |Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint));
#endif
        splashVisible = true;
    }

    QString fileName = QFileDialog::getOpenFileName(this,  tr("Open solution"), "", tr("DDM solution files (*.dmx);;All files (*.*)"));
    if(fileName.length() == 0)
    {
        if(splashVisible)
        {
#ifdef Q_WS_WIN
            Qt::WindowFlags flags = m_btndlg->windowFlags();
            m_btndlg->setWindowFlags(flags |Qt::SplashScreen | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
#endif
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
    m_ui->action_ConnectionsTree->setCheckable(false);
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

void MainWindow::connectActionsFromPopupMenus()
{
    QObject::connect(ContextMenuCollection::getInstance()->getAction_RemoveTable(), SIGNAL(triggered()), this, SLOT(onDeleteTableFromPopup()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_AddTable(), SIGNAL(triggered()), this, SLOT(onNewTableFromPopup()));
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
    QObject::connect(ContextMenuCollection::getInstance()->getAction_DeleteDiagram(), SIGNAL(triggered()), this, SLOT(onDeleteDiagramFromPopup()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_RenameDiagram(), SIGNAL(triggered()), this, SLOT(onRenameDiagramFromPopup()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_AddDiagram(), SIGNAL(triggered()), this, SLOT(onNewDiagramFromPopup()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_AddString(), SIGNAL(triggered()), this, SLOT(onNewStringType()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_AddNumeric(), SIGNAL(triggered()), this, SLOT(onNewNumericType()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_AddBool(), SIGNAL(triggered()), this, SLOT(onNewBoolType()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_AddDateType(), SIGNAL(triggered()), this, SLOT(onNewDateTimeType()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_AddMisc(), SIGNAL(triggered()), this, SLOT(onNewMiscType()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_AddBlob(), SIGNAL(triggered()), this, SLOT(onNewBlobType()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_AddSpatial(), SIGNAL(triggered()), this, SLOT(onNewSpatialType()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_GotoIssueLocation(), SIGNAL(triggered()), this, SLOT(onGotoIssueLocation()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_IgnoreIssue(), SIGNAL(triggered()), this, SLOT(onIgnoreIssue()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_IgnoreIssuesFromThisTable(), SIGNAL(triggered()), this, SLOT(onIgnoreIssuesOfATable()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_CreateViewUsingQueryBuilder(), SIGNAL(triggered()), this, SLOT(onNewView()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_CreateViewUsingSql(), SIGNAL(triggered()), this, SLOT(onNewViewWithSql()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_ConnectionConnect(), SIGNAL(triggered()), this, SLOT(onConnectConnection()));
    ContextMenuCollection::getInstance()->getAction_BrowsedTableInject()->setVisible(true);
    QObject::connect(ContextMenuCollection::getInstance()->getAction_DeleteView(), SIGNAL(triggered()), this, SLOT(onDeleteViewFromPopup()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_DeleteProcedure(), SIGNAL(triggered()), this, SLOT(onDeleteProcedure()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_DeleteFunction(), SIGNAL(triggered()), this, SLOT(onDeleteFunction()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_DeleteTrigger(), SIGNAL(triggered()), this, SLOT(onDeleteTrigger()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_ReleaseMajorVersion(), SIGNAL(triggered()), this, SLOT(onReleaseMajorVersion()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_unlock(), SIGNAL(triggered()), this, SLOT(onUnlockSomething()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_relock(), SIGNAL(triggered()), this, SLOT(onRelockSomething()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_FinalizePatch(), SIGNAL(triggered()), this, SLOT(finalizePatch()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_RenamePatch(), SIGNAL(triggered()), this, SLOT(renamePatch()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_Undelete(), SIGNAL(triggered()), this, SLOT(onUndeleteSomething()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_CompareTable(), SIGNAL(triggered()), this, SLOT(onCompareTables()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_AddProcedure(), SIGNAL(triggered()), this, SLOT(onNewProcedureFromPopup()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_AddFunction(), SIGNAL(triggered()), this, SLOT(onNewFunctionFromPopup()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_AddTrigger(), SIGNAL(triggered()), this, SLOT(onNewTriggerFromPopup()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_AddView(), SIGNAL(triggered()), this, SLOT(onNewViewFromPopup()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_AddViewWithSql(), SIGNAL(triggered()), this, SLOT(onNewViewWithSqlFromPopup()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_InitiatePatch(), SIGNAL(triggered()), this, SLOT(onInitiatePatch()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_DeployVersion(), SIGNAL(triggered()), this, SLOT(onDeployVersion()));

}

template <class T>
void MainWindow::showNamedObjectList(showSomething s, const QVector<T*> items, const QIcon& icon, const QString& title)
{
    NamedObjectListingForm* lstForm = new NamedObjectListingForm(this, s, icon, title);
    lstForm->populateObjects(items);
    setCentralWidget(lstForm);
}

void MainWindow::onDeployVersion()
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
                // TODO: This is duplicate from onDeploy except the version
                InjectSqlDialog* injectDialog = new InjectSqlDialog(
                            m_workspace->getInstance()->currentProjectsEngine(), this, m, "");
                injectDialog->setModal(true);
                if(injectDialog->exec() == QDialog::Accepted)
                {
                    createStatusLabel();
                    QStringList connectionNames = injectDialog->getSelectedConnections();
                    DeploymentInitiator* dinit = new DeploymentInitiator();
                    dinit->doDeployment(m, connectionNames,
                                        injectDialog->injectMetadataRequired(),
                                        injectDialog->getUidsToDeploy(),
                                        injectDialog->getUidsToDrop());
                }
            }
        }
    }
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
            }
        }
    }
}

bool MainWindow::showObjectWithGuid(const QString & guid)
{
    ObjectWithUid* element = UidWarehouse::instance().getElement(guid);
    QString classUid = element->getClassUid().toString().toUpper();
    Version* v = UidWarehouse::instance().getVersionForUid(guid);
    // TODO: this map is a duplicate from the tree change
    QMap<QString, showSomething> mapping;
    mapping.insert(uidTable, (showSomething)&MainWindow::showTableWithGuid);
    mapping.insert(uidTableInstance, (showSomething)&MainWindow::showTableInstanceWithGuid);
    mapping.insert(uidDiagram, (showSomething)&MainWindow::showDiagramWithGuid);
    mapping.insert(uidProcedure, (showSomething)&MainWindow::showProcedureWithGuid);
    mapping.insert(uidFunction, (showSomething)&MainWindow::showFunctionWithGuid);
    mapping.insert(uidTrigger, (showSomething)&MainWindow::showTriggerWithGuid);
    mapping.insert(uidView, (showSomething)&MainWindow::showViewWithGuid);

    if(mapping.contains(classUid))
    {
        showObjectWithGuidAndVersion(v, guid, mapping[classUid], false);
        return true;
    }
    else
    {
        return false;
    }
}

void MainWindow::finallyDoLockLikeOperation(bool reLocking, const QString& guid)
{
    ObjectWithUid* element = UidWarehouse::instance().getElement(guid);
    Version* v = UidWarehouse::instance().getVersionForUid(guid);
    // now find the type of this element
    UserDataType* udt = dynamic_cast<UserDataType*>(element);
    bool doneSomething = false;

    if(udt)
    {
        if(!reLocking)
        {
            udt->unlock();
        }
        else
        {
            udt->lock(LockableElement::LOCKED);
        }

        udt->updateGui();
        showDataType(v, udt->getName(), true);
        doneSomething = true;
    }

    LockableElement* le = dynamic_cast<LockableElement*>(element);

    if(le && !doneSomething)
    {
        if(!reLocking)
        {
            le->unlock();
        }
        else
        {
            le->lock(LockableElement::LOCKED);
        }

        le->updateGui();
        doneSomething = true;

    }

    // and finally tell the version of the element that a new patch is about to be born
    if(doneSomething)
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
    m_guiElements->updateItemForPatchWithState(v->getWorkingPatch(), element->getClassUid(), guid, ni->getName(), state);
}

void MainWindow::createPatchElement(Version* v, ObjectWithUid * element, const QString& guid, bool reLocking)
{
    NamedItem *ni = dynamic_cast<NamedItem*>(element);
    if(!ni) return;
    if(!m_guiElements->getPatchesDock()->isVisible() && !reLocking)
    {
        m_guiElements->getPatchesDock()->show();
        addDockWidget(Qt::LeftDockWidgetArea, m_guiElements->getPatchesDock());
    }

    if(!reLocking)
    {
        v->getWorkingPatch()->addElement(guid);
        m_guiElements->createNewItemForPatch(v->getWorkingPatch(), element->getClassUid(), guid, ni->getName());
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
        finallyDoLockLikeOperation(reLocking, obj->getObjectUid());
    }
}

void MainWindow::onUnlockSomething()
{
    doLockLikeOperation(false);
}

void MainWindow::onRelockSomething()
{
    doLockLikeOperation(true);
}

void MainWindow::onDeleteFunction()
{
    Function* f = getRightClickedObject<Function>();
    if(f)
    {
        if(QMessageBox::question(this, tr("Are you sure?"), tr("Really delete ") + f->getName()+ "?", QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
        {
            return;
        }
        f->version()->deleteFunction(f->getName());
        f->version()->getGui()->updateForms();

        showNamedObjectList(&MainWindow::showFunctionWithGuid, f->version()->getFunctions(), IconFactory::getFunctionIcon(), "Functions");

        f->version()->getGui()->getFunctionsItem()->treeWidget()->clearSelection();
        f->version()->getGui()->getFunctionsItem()->setSelected(true);
    }
}

void MainWindow::onDeleteTrigger()
{
    Trigger* t = getRightClickedObject<Trigger>();
    if(t)
    {
        if(QMessageBox::question(this, tr("Are you sure?"), tr("Really delete ") + t->getName()+ "?", QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
        {
            return;
        }
        t->version()->deleteTrigger(t->getName());
        t->version()->getGui()->updateForms();

        showNamedObjectList(&MainWindow::showTriggerWithGuid, t->version()->getTriggers(), IconFactory::getTriggerIcon(), "Triggers");

        t->version()->getGui()->getTriggersItem()->treeWidget()->clearSelection();
        t->version()->getGui()->getTriggersItem()->setSelected(true);
    }
}

void MainWindow::onDeleteProcedure()
{
    Procedure* p = getRightClickedObject<Procedure>();
    if(p)
    {
        if(QMessageBox::question(this, tr("Are you sure?"), tr("Really delete ") + p->getName()+ "?", QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
        {
            return;
        }
        p->version()->deleteProcedure(p->getName());
        p->version()->getGui()->updateForms();

        showNamedObjectList(&MainWindow::showProcedureWithGuid, p->version()->getProcedures(), IconFactory::getProcedureIcon(), "Procedures");

        p->version()->getGui()->getProceduresItem()->treeWidget()->clearSelection();
        p->version()->getGui()->getProceduresItem()->setSelected(true);
    }
}


void MainWindow::onDeleteViewFromPopup()
{
    View* v = getRightClickedObject<View>();
    if(v)
    {
        if(QMessageBox::question(this, tr("Are you sure?"), tr("Really delete ") + v->getName()+ "?", QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
        {
            return;
        }
        v->version()->deleteView(v->getName());
        v->version()->getGui()->updateForms();

        showNamedObjectList(&MainWindow::showViewWithGuid, v->version()->getViews(), IconFactory::getViewsIcon(), "Views");

        v->version()->getGui()->getViewsItem()->treeWidget()->clearSelection();
        v->version()->getGui()->getViewsItem()->setSelected(true);

    }
}

void MainWindow::onAbout()
{
    bool wasBtndlg = false;
    if(m_btndlg && m_btndlg->isVisible())
    {
        m_btndlg->hide();
#ifdef Q_WS_WIN
        Qt::WindowFlags flags = m_btndlg->windowFlags();
        m_btndlg->setWindowFlags(flags ^ (Qt::SplashScreen |Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint));
#endif
        wasBtndlg = true;
    }

    AboutBoxDialog* about = new AboutBoxDialog(this);
    about->setModal(true);
    about->exec();

    if(m_btndlg && wasBtndlg)
    {
#ifdef Q_WS_WIN
        Qt::WindowFlags flags = m_btndlg->windowFlags();
        m_btndlg->setWindowFlags(flags | Qt::SplashScreen |Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
#endif
        m_btndlg->show();
    }

}

void MainWindow::onNewDiagram()
{
    Diagram* dgram = new Diagram(m_workspace->workingVersion(), QUuid::createUuid().toString());
    DiagramForm* df = new DiagramForm(m_workspace->workingVersion(), dgram, this);
    m_workspace->workingVersion()->addDiagram(dgram, false);
    setCentralWidget(df);
    dgram->setForm(df);
    onSaveDiagram(dgram, m_workspace->workingVersion());
    m_guiElements->getProjectTree()->setCurrentItem(dgram->getLocation());
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
//        qDebug() << "table is null";
        return;
    }
    Version* v = table->version();
    if(!v)
    {
        return;
    }
    v->getGui()->updateForms();
    TableInstance* tinst = instantiateTable(table->getName(), QStringList(), v);
    showTableInstanceWithGuid(v, tinst->getObjectUid());
}

void MainWindow::onSpecializeTableFromPopup()
{
    Table* table = getRightClickedObject<Table>();
    if(table)
    {
        Table* specializedTable = new Table(table->version(), QUuid::createUuid().toString(), 0);
        specializedTable->initializeFor(Workspace::getInstance()->currentProjectsEngine(), QUuid(uidTable));

        specializedTable->setName(table->getName() + "_specialized");
        specializedTable->setParent(table);
        specializedTable->setParentUid(table->getObjectUid());

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
    ObjectWithUid* obj = UidWarehouse::instance().getElement(uid);
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

        showNamedObjectList(&MainWindow::showTableInstanceWithGuid, v->getTableInstances(), IconFactory::getTabinstIcon(), QObject::tr("Tables"));
    }
}

void MainWindow::onDeleteTableFromPopup()
{
    Table* tab =  getRightClickedObject<Table>();
    if(tab)
    {
        Version* v = tab->version();
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
            createPatchElement(v, tab, tab->getObjectUid(), false);
            v->getWorkingPatch()->addNewElement(tab->getObjectUid()); // this will be a new element ...
            updatePatchElementToReflectState(v, tab, tab->getObjectUid(), 1);
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

void MainWindow::onNewTableInstanceFromPopup()
{
    // TODO: Duplicate from various spaces :)
    if(m_guiElements->getProjectTree()->getLastRightclickedItem() == 0)
    {
        return;
    }

    ContextMenuEnabledTreeWidgetItem* item = m_guiElements->getProjectTree()->getLastRightclickedItem();
    m_guiElements->getProjectTree()->setLastRightclickedItem(0);

    QVariant qv = item->data(0, Qt::UserRole);
    QString tablesUid = qv.toString();

    Version* v = UidWarehouse::instance().getVersionForUid(tablesUid);

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
            instantiateTable(items.at(i), items, m_workspace->workingVersion());
        }

        // This is OK, the button defaultly adds to the working version
        m_workspace->workingVersion()->getGui()->updateForms();
        m_guiElements->getProjectTree()->setCurrentItem(m_workspace->workingVersion()->getGui()->getTableInstancesItem());


    }
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
        act->setText(cons.at(i)->getName() + " - " + cons.at(i)->getDb()+"@"+cons.at(i)->getHost());
        act->setData(cons.at(i)->getName());
        act->setIcon(IconFactory::getConnectionStateIcon(cons.at(i)->getState()));
        deployPopupMenu->addAction(act);
        QObject::connect(act, SIGNAL(triggered()),
                new DynamicActionHandlerforMainWindow(cons.at(i)->getName(), this, (MainWindow::dynamicAction)&MainWindow::specificDeploymentCallback),
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
            scw->updateSql(m_workspace->currentProject(), scw->version());
        }
    }
}

void MainWindow::onRenameInstanceFromPopup()
{
    TableInstance* tinst =  getRightClickedObject<TableInstance>();

    if(tinst)
    {
        SimpleTextInputDialog* dlg = new SimpleTextInputDialog(this, tr("Enter the new name"));
        dlg->setModal(true);
        dlg->setText(tinst->getName());
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

            m_guiElements->updateItemForPatchWithState(tinst->version()->getWorkingPatch(), uidTableInstance, tinst->getObjectUid(), t, 2);
        }
    }
}

void MainWindow::onRenameDiagramFromPopup()
{
    Diagram* dgr =  getRightClickedObject<Diagram>();
    if(dgr)
    {
        SimpleTextInputDialog* dlg = new SimpleTextInputDialog(this, tr("Enter the new name"));
        dlg->setModal(true);
        dlg->setText(dgr->getName());
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
}

void MainWindow::onDeleteDatatypeFromPopup()
{
    UserDataType* udt = getRightClickedObject<UserDataType>();
    if(udt)
    {
        QString dtName = udt->getName();
        if(QMessageBox::question(this, tr("Are you sure?"), tr("Really delete ") + dtName + "?", QMessageBox::Yes | QMessageBox::No) ==  QMessageBox::Yes)
        {
            // now check that this stuff is not used in any of the tables and the delete it from the current version
            const QVector<Table*>& allTables = udt->version()->getTables();
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

            delete udt->getLocation();
            udt->version()->deleteDataType(dtName);

            DataTypesListForm* dtLst = new DataTypesListForm(this);
            dtLst->feedInDataTypes(udt->version()->getDataTypes());
            setCentralWidget(dtLst);

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
            if(!c) return;
            QString tab = s.left(s.indexOf("?")).mid(2);
            QString validName = NameGenerator::getUniqueName(Workspace::getInstance()->currentProject()->getWorkingVersion(), (itemGetter)&Version::getTable, tab);
            Table* t = Workspace::getInstance()->currentProjectsEngine()->reverseEngineerTable(c, validName, Workspace::getInstance()->currentProject(), true, m_workspace->workingVersion());
            t->setName(validName);
            Workspace::getInstance()->currentProject()->getWorkingVersion()->addTable(t, false);
            m_workspace->workingVersion()->getGui()->createTableTreeEntry(t, m_workspace->workingVersion()->getGui()->getTablesItem());
            showTableWithGuid(m_workspace->workingVersion(), t->getObjectUid());
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
            m_btndlg->hide();
#ifdef Q_WS_WIN
            Qt::WindowFlags flags = m_btndlg->windowFlags();
            m_btndlg->setWindowFlags(flags ^ (Qt::SplashScreen |Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint));
#endif
            w = true;
        }

        InjectSqlDialog* ij = new InjectSqlDialog(0, 0, 0, "");
        ij->setModal(true);
        ij->populateConnectionDetails(c);
        if(ij->exec()  == QDialog::Accepted)
        {
            c->resetTo(ij->getName(), ij->getHost(), ij->getUser(), ij->getPassword(), ij->getDatabase(), ij->getPort(), true, ij->getAutoConnect());
            c->setDisplayText(c->getName());
            c->getLocation()->setText(1, c->getDb()+"@"+c->getHost());
            QVariant var(c->getName());
            c->getLocation()->setData(0, Qt::UserRole, var);
            c->getLocation()->setIcon(0, IconFactory::getConnectionStateIcon(c->getState()));
        }

        if(m_btndlg && w)
        {
#ifdef Q_WS_WIN
            Qt::WindowFlags flags = m_btndlg->windowFlags();
            m_btndlg->setWindowFlags(flags | Qt::SplashScreen |Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
#endif
            m_btndlg->show();
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

void MainWindow::onDropConnection()
{
    Connection* c = getRightClickedConnection();
    if(c)
    {
        bool w = false;
        if(m_btndlg && m_btndlg->isVisible())
        {
            m_btndlg->hide();
#ifdef Q_WS_WIN
            Qt::WindowFlags flags = m_btndlg->windowFlags();
            m_btndlg->setWindowFlags(flags ^ (Qt::SplashScreen |Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint));
#endif
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
#ifdef Q_WS_WIN
            Qt::WindowFlags flags = m_btndlg->windowFlags();
            m_btndlg->setWindowFlags(flags | Qt::SplashScreen |Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
#endif
            m_btndlg->show();
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

void MainWindow::onDeleteDiagramFromPopup()
{
    Diagram* dgr =  getRightClickedObject<Diagram>();
    if(dgr)
    {
        if(QMessageBox::question(this, tr("Are you sure?"), tr("Really delete ") + dgr->getName()+ "?", QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
        {
            return;
        }
        Version *v = dgr->version();
        v->deleteDiagram(dgr->getName());
        v->getGui()->updateForms();

        showNamedObjectList(&MainWindow::showDiagramWithGuid, v->getTables(), IconFactory::getDiagramIcon(), "Diagrams");
        v->getGui()->getDiagramsItem()->treeWidget()->clearSelection();
        v->getGui()->getDiagramsItem()->setSelected(true);
    }
}

void MainWindow::dtTreeItemClicked ( QTreeWidgetItem *, int)
{
    onDTTreeClicked();
}

Version* MainWindow::getVersionOfLastRightClickedElement()
{
    if(m_guiElements->getProjectTree()->getLastRightclickedItem() == 0)
    {
        return 0;
    }

    ContextMenuEnabledTreeWidgetItem* item = m_guiElements->getProjectTree()->getLastRightclickedItem();
    m_guiElements->getProjectTree()->setLastRightclickedItem(0);

    QVariant qv = item->data(0, Qt::UserRole);
    QString diagramsUid = qv.toString();

    Version* ver = UidWarehouse::instance().getVersionForUid(diagramsUid);
    return ver;
}

void MainWindow::onNewStringType()
{
    showNewDataTypeWindow(DT_STRING, getVersionOfLastRightClickedElement());
}

void MainWindow::onNewNumericType()
{
    showNewDataTypeWindow(DT_NUMERIC, getVersionOfLastRightClickedElement());
}

void MainWindow::onNewBoolType()
{
    showNewDataTypeWindow(DT_BOOLEAN, getVersionOfLastRightClickedElement());
}

void MainWindow::onNewDateTimeType()
{
    showNewDataTypeWindow(DT_DATETIME, getVersionOfLastRightClickedElement());
}

void MainWindow::onNewBlobType()
{
    showNewDataTypeWindow(DT_BLOB, getVersionOfLastRightClickedElement());
}

void MainWindow::onNewMiscType()
{
    showNewDataTypeWindow(DT_MISC, getVersionOfLastRightClickedElement());
}

void MainWindow::onNewSpatialType()
{
    showNewDataTypeWindow(DT_SPATIAL, getVersionOfLastRightClickedElement());
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
            c->resetTo(ij->getName(), ij->getHost(), ij->getUser(), ij->getPassword(), ij->getDatabase(), ij->getPort(), true, ij->getAutoConnect());
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

void MainWindow::onDeploy()
{
    InjectSqlDialog* injectDialog = new InjectSqlDialog(
                m_workspace->getInstance()->currentProjectsEngine(), this,
                Workspace::getInstance()->workingVersion(), "");
    injectDialog->setModal(true);
    if(injectDialog->exec() == QDialog::Accepted)
    {
        createStatusLabel();
        QStringList connectionNames = injectDialog->getSelectedConnections();
        DeploymentInitiator* dinit = new DeploymentInitiator();
        dinit->doDeployment(m_workspace->workingVersion(), connectionNames,
                            injectDialog->injectMetadataRequired(),
                            injectDialog->getUidsToDeploy(),
                            injectDialog->getUidsToDrop());
    }
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
                iss->getLocation()->setIcon(3, IconFactory::getIconForClassUid(obj->getClassUid()));
            }
            if(!lblStatus)
            {
                createStatusLabel();
            }
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
            QMessageBox::critical(m_revEngWizard, tr("Error"), tr("Please select a database"), QMessageBox::Ok);
            m_revEngWizard->back();
        }

        m_revEngWizard->connectAndRetrieveTables();
        break;
    case 3: // user selected the tables, advanced to the views
        if(!m_revEngWizard->selectDatabase()) // did he select a database?
        {
            QMessageBox::critical(m_revEngWizard, tr("Error"), tr("Please select a database"), QMessageBox::Ok);
            m_revEngWizard->back();
        }
        m_revEngWizard->connectAndRetrieveViews();
        break;
    case 4: // procedures
        if(!m_revEngWizard->selectDatabase()) // did he select a database?
        {
            QMessageBox::critical(m_revEngWizard, tr("Error"), tr("Please select a database"), QMessageBox::Ok);
            m_revEngWizard->back();
        }
        m_revEngWizard->connectAndRetrieveProcedures();
        break;
    case 5: // functions
        if(!m_revEngWizard->selectDatabase()) // did he select a database?
        {
            QMessageBox::critical(m_revEngWizard, tr("Error"), tr("Please select a database"), QMessageBox::Ok);
            m_revEngWizard->back();
        }
        m_revEngWizard->connectAndRetrieveFunctions();
        break;
    case 6: // triggers
        if(!m_revEngWizard->selectDatabase()) // did he select a database?
        {
            QMessageBox::critical(m_revEngWizard, tr("Error"), tr("Please select a database"), QMessageBox::Ok);
            m_revEngWizard->back();
        }
        m_revEngWizard->connectAndRetrieveTriggers();
        break;
    }
}

void MainWindow::onReverseEngineerWizardAccept()
{
    QString host = m_revEngWizard->getHost();
    QString user = m_revEngWizard->getUser();
    QString pass = m_revEngWizard->getPasword();
    QString db = m_revEngWizard->getDatabase();
    int port = m_revEngWizard->getPort();
    Project* p = m_workspace->currentProject();
    bool c = !m_revEngWizard->createDataTypesForColumns();
    DatabaseEngine* engine = m_workspace->currentProjectsEngine();

    createStatusLabel();
    lblStatus->setText(QApplication::translate("MainWindow", "Reverse engineering started", 0, QApplication::UnicodeUTF8));

    ReverseEngineerer* revEng = new ReverseEngineerer(c, engine, p, host, user, pass, db, port,
                                                      m_revEngWizard->getTablesToReverse(),
                                                      m_revEngWizard->getViewsToReverse(),
                                                      m_revEngWizard->getProceduresToReverse(),
                                                      m_revEngWizard->getFunctionsToReverse(),
                                                      m_revEngWizard->getTriggersToReverse(),
                                                      0);
    connect(revEng, SIGNAL(done(ReverseEngineerer*)), this, SLOT(onReverseEngineeringFinished(ReverseEngineerer*)));
    revEng->reverseEngineer();
}

void MainWindow::onReverseEngineeringFinished(ReverseEngineerer*)
{
    lblStatus->setText(QApplication::translate("MainWindow", "Reverse engineering finished", 0, QApplication::UnicodeUTF8));

    // Ok, reverse engineering can happen only to the current version
    m_workspace->workingVersion()->getGui()->populateTreeItems();

}

void MainWindow::onHelp()
{
    if(m_btndlg && m_btndlg->isVisible())
    {
        m_btndlg->hide();
#ifdef Q_WS_WIN
        Qt::WindowFlags flags = m_btndlg->windowFlags();
        m_btndlg->setWindowFlags(flags ^ (Qt::SplashScreen |Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint));
#endif
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
#ifdef Q_WS_WIN
            Qt::WindowFlags flags = m_btndlg->windowFlags();
            m_btndlg->setWindowFlags(flags ^ (Qt::SplashScreen |Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint));
#endif
        m_btndlg->hide();
        wasVisible = true;
    }
    m_workspace->createNewConnection();
    if(wasVisible)
    {
#ifdef Q_WS_WIN
        Qt::WindowFlags flags = m_btndlg->windowFlags();
        m_btndlg->setWindowFlags(flags | Qt::SplashScreen |Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
#endif

        m_btndlg->show();
    }
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
    // Ok. New views from the button go always to the working version
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
    // Ok. New views from the button go always to the working version
    View* v = new View(false, QUuid::createUuid().toString(), m_workspace->workingVersion());
    m_nvf = 0;
    v->getHelper()->setForm(this);
    Workspace::getInstance()->workingVersion()->addView(v, false);
    Workspace::getInstance()->workingVersion()->getGui()->createViewTreeEntry(v);
    m_guiElements->getProjectTree()->setCurrentItem(v->getLocation());
    rerenderQuery(v->getQuery());
}

void MainWindow::onNewProcedure()
{
    // Ok. New procedures from the button go always to the working version
    ProcedureForm* frm = Workspace::getInstance()->workingVersion()->getGui()->getProcedureForm(MODE_PROCEDURE);
    Procedure* p = new Procedure(NameGenerator::getUniqueName(Workspace::getInstance()->workingVersion(),
                                                              (itemGetter)&Version::getProcedure, QString("proc")),
                                                               QUuid::createUuid().toString(), m_workspace->workingVersion());
    frm->setProcedure(p);
    frm->initSql();
    Workspace::getInstance()->workingVersion()->addProcedure(p, false);
    Workspace::getInstance()->workingVersion()->getGui()->createProcedureTreeEntry(p);
    m_guiElements->getProjectTree()->setCurrentItem(p->getLocation());
    setCentralWidget(frm);
}

void MainWindow::onNewFunction()
{
    // Ok. New functions from the button go always to the working version
    ProcedureForm* frm = Workspace::getInstance()->workingVersion()->getGui()->getProcedureForm(MODE_FUNCTION);
    Function* func = new Function(NameGenerator::getUniqueName(Workspace::getInstance()->workingVersion(), (itemGetter)&Version::getFunction, QString("func")), QUuid::createUuid().toString(), m_workspace->workingVersion());
    frm->setProcedure(func);
    frm->initSql();
    Workspace::getInstance()->workingVersion()->addFunction(func, false);
    Workspace::getInstance()->workingVersion()->getGui()->createFunctionTreeEntry(func);
    m_guiElements->getProjectTree()->setCurrentItem(func->getLocation());
    //m_guiElements->getProjectTree()->setCurrentItem(p->getLocation());
    setCentralWidget(frm);
}

void MainWindow::onNewTrigger()
{
    // Ok. New triggers from the button go always to the working version
    const QVector<Table*>& allTables = Workspace::getInstance()->workingVersion()->getTables();
    if(allTables.size() == 0)
    {
        QMessageBox::critical(this, tr("Cannot create a trigger when there are no tables"), tr("No tables defined"), QMessageBox::Ok);
        return;
    }
    TriggerForm* frm = Workspace::getInstance()->workingVersion()->getGui()->getTriggerForm();
    Trigger* trigger = new Trigger(NameGenerator::getUniqueName(Workspace::getInstance()->workingVersion(), (itemGetter)&Version::getTrigger, QString("trig")), QUuid::createUuid().toString(), m_workspace->workingVersion());
    frm->setTrigger(trigger);
    frm->initSql();
    frm->feedInTables(allTables);
    trigger->setTable(allTables.at(0)->getName());
    frm->feedInTriggerEvents(Workspace::getInstance()->currentProjectsEngine()->getTriggerEvents());
    frm->feedInTriggerTimes(Workspace::getInstance()->currentProjectsEngine()->getTriggerTimings());
    Workspace::getInstance()->workingVersion()->addTrigger(trigger, false);
    Workspace::getInstance()->workingVersion()->getGui()->createTriggerTreeEntry(trigger);
    m_guiElements->getProjectTree()->setCurrentItem(trigger->getLocation());
    setCentralWidget(frm);

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

    m_nvf = new NewViewForm(dynamic_cast<View*>(q->getSourceEntity())->version(), true, q->getHelper(), this);
    m_nvf->setSqlSource(q->getSourceEntity());
    m_nvf->setView(dynamic_cast<View*>(q->getSourceEntity()));

    m_nvf->setGraphicsItem(q->getGraphicsItem());
    q->getHelper()->setScene(m_nvf->getScene());
    m_nvf->presentSql(Workspace::getInstance()->currentProject(), dynamic_cast<View*>(q->getSourceEntity())->version());
    setCentralWidget(m_nvf);
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
        tryBrowseConnection(c);
        c->getLocation()->setExpanded(true);
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
        Version* v = p->version();
        Project* prj = v->getProject();
        MajorVersion* newVersion = prj->createMajorVersion(v->getMajor(), v->getMinor() + 1);
        v->cloneInto(newVersion, LockableElement::FINAL_LOCK);
        newVersion->setSourceUid(v->getObjectUid());


        newVersion->createTreeItems(m_guiElements, p->getLocation(), m_guiElements->getProjectTree()->indexOfTopLevelItem(v->getLocation()) + 1);
        newVersion->getGui()->populateTreeItems();

        v->getGui()->getVersionItem()->setExpanded(false);
        newVersion->getGui()->getVersionItem()->setExpanded(true);
        newVersion->getGui()->getVersionItem()->setPopupMenu(ContextMenuCollection::getInstance()->getReleasedVersionPopupMenu());

        v->updateGui();

        v->getGui()->getVersionItem()->setPopupMenu(ContextMenuCollection::getInstance()->getFinalisedVersionPopupMenu());

        p->suspendPatch();

        v->lockVersion(LockableElement::FINAL_LOCK);
        newVersion->lockVersion(LockableElement::LOCKED);
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

        SimpleTextInputDialog* dlg = new SimpleTextInputDialog(this, tr("Enter the new name"));
        dlg->setModal(true);
        dlg->setText(p->getName());
        if(dlg->exec() == QDialog::Accepted)
        {
            QString t = dlg->getText();
            //if(m_workspace->getInstance()->workingVersion()->getTableInstance(t))
            //{
            //    QMessageBox::critical(this, tr("Error"), tr("You can have only one table instance called ") + t, QMessageBox::Ok);
            //    return;
            //}
            p->setName(t);
            p->setDisplayText(t);
        }
    }
}

void MainWindow::onUndeleteSomething()
{
    ObjectWithUid* obj = getRightClickedObject<ObjectWithUid>();
    Version* v = UidWarehouse::instance().getVersionForUid(obj->getObjectUid());
    Version::PatchTreeRemovalStatus removeFromTree = v->undeleteObject(obj->getObjectUid(), false);
    if(removeFromTree != Version::DO_NOT_REMOVE_FROM_PATCH_TREE_FAILURE)
    {
//        if(removeFromTree == Version::REMOVE_FROM_PATCH_TREE)
        {
            m_guiElements->removeItemForPatch(v->getWorkingPatch(), obj->getObjectUid());
        }
        showObjectWithGuid(obj->getObjectUid());
    }
}

void MainWindow::onInitiatePatch()
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
                if(!m_guiElements->getPatchesDock()->isVisible())
                {
                    m_guiElements->getPatchesDock()->show();
                    addDockWidget(Qt::LeftDockWidgetArea, m_guiElements->getPatchesDock());
                }
                m->getWorkingPatch();
            }
        }
    }
}

void MainWindow::onCompareTables()
{
    Table* table =getRightClickedObject<Table>();
    if(table == 0)  // shouldn't be ...
    {
//        qDebug() << "table for comparison is null";
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
    tcf->setFromVersion(Workspace::getInstance()->workingVersion());
    tcf->setToVersion(Workspace::getInstance()->workingVersion());
    setCentralWidget(tcf);
}

