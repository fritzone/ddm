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
#include "PreferencesDialog.h"
#include "SimpleTextInputDialog.h"
#include "ContextMenuCollection.h"
#include "strings.h"
#include "Workspace.h"
#include "VersionGuiElements.h"
#include "InjectSqlDialog.h"
#include "ReverseEngineerWizard.h"
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
    showConnections();

    m_ui->action_NewDatabaseConnection->setEnabled(true);
    showMaximized();
    setWindowTitle(tr("DDM - [No Solution]"));

    m_btndlg = new MainWindowButtonDialog();
    m_btndlg->showMe();
    //m_btndlg->raise();

    QApplication::instance()->installEventFilter(this);
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
    addDockWidget(Qt::RightDockWidgetArea, m_connectionGuiElements->getConnectionsTreeDock());
}

void MainWindow::setupGuiForNewSolution()
{

    m_guiElements = new GuiElements();
    m_guiElements->createGuiElements();

    addDockWidget(Qt::LeftDockWidgetArea, m_guiElements->getProjectDock());
    addDockWidget(Qt::LeftDockWidgetArea, m_guiElements->getDatatypesDock());
    addDockWidget(Qt::BottomDockWidgetArea, m_guiElements->getIssuesDock());

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

        setupGuiForNewSolution();

        Project* project = new Project(nprjdlg->getProjectName().toUpper(), nprjdlg->enableOOPFeatures());
        m_workspace->addProjectToSolution(m_workspace->currentSolution(), project);
        project->setEngine(nprjdlg->getDatabaseEngine());

        if(nprjdlg->getProjectType() == NewProjectDialog::PRJ_BINDTODATABASE)
        {
            InjectSqlDialog* injectDialog = new InjectSqlDialog(0);
            injectDialog->setupForBindToDatabase();
            injectDialog->setModal(true);
            if(injectDialog->exec() == QDialog::Accepted)
            {
                QString host = injectDialog->getHost();
                QString user = injectDialog->getUser();
                QString password = injectDialog->getPassword();
                QString db = injectDialog->getDatabase();

                Connection c("temp", host, user, password, db, false, false);
                QString meta = nprjdlg->getDatabaseEngine()->getDbMetadata(&c);
                QDomDocument doc;
                QString lastErr;
                if(!doc.setContent(meta, &lastErr))
                {
                    qDebug() << "Cannot set data:" << lastErr;
                }
                MajorVersion* mv = DeserializationFactory::createMajorVersion(project, project->getEngine(), doc, doc.firstChildElement().firstChildElement());
                project->addMajorVersion(mv);
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

            m_guiElements->getDataTypesTree()->scrollToItem(m_workspace->workingVersion()->getGui()->getDtsItem());
            m_guiElements->getDataTypesTree()->expandAll();
        }

        // expand the tree
        m_guiElements->getProjectTree()->expandAll();

        // show the project properties window
        showProjectDetails();

        setWindowTitle(tr("DDM - [") + m_workspace->currentSolution()->name() + tr("]"));

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
    QList<QTreeWidgetItem*> selectedItems = m_guiElements->getDataTypesTree()->selectedItems();
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
                QString n = qv.toString();
                UserDataType* udt = 0;
                if(m_workspace->workingVersion()->hasDataType(n))
                {
                    udt = m_workspace->workingVersion()->getDataType(n);
                }
                NewDataTypeForm* frm = new NewDataTypeForm(DT_INVALID, m_workspace->currentProjectsEngine(), this);
                frm->focusOnName();
                frm->setDataType(udt);
                setCentralWidget(frm);
            }
        }
    }
}

void MainWindow::showNothing(const QString &, bool )
{

}

NewTableForm* MainWindow::showExistingTable(Table *table)
{
    if(table == 0) return 0;
    NewTableForm* frm = m_workspace->workingVersion()->getGui()->getTableFormForExistingTable();
    frm->setTable(table);
    frm->focusOnName();
    setCentralWidget(frm);
    return frm;
}

void MainWindow::showTableWithGuid(const QString &guid, bool focus)
{
    Table* table =  dynamic_cast<Table*>(UidWarehouse::instance().getElement(guid));
    if(table == 0)  // shouldn't be ...
    {
        return;
    }
    showExistingTable(table);
    if(focus)
    {
        m_guiElements->getProjectTree()->setCurrentItem(table->getLocation());
    }
}

void MainWindow::showTableInstanceWithGuid(const QString &guid, bool focus)
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

    if(focus) m_guiElements->getProjectTree()->setCurrentItem(table->getLocation());
}

void MainWindow::showDataType(const QString &name, bool focus)
{
    UserDataType* dt = m_workspace->workingVersion()->getDataType(name);
    if(dt == 0)  // shouldn't be ...
    {
        return;
    }

    NewDataTypeForm* frm = new NewDataTypeForm(DT_INVALID, m_workspace->currentProjectsEngine(), this);
    frm->focusOnName();
    frm->setDataType(dt);
    setCentralWidget(frm);

    if(focus) m_guiElements->getDataTypesTree()->setCurrentItem(dt->getLocation());
}

void MainWindow::showDiagramWithGuid(const QString &guid, bool /*focus*/)
{
    Diagram* dgram = dynamic_cast<Diagram*>(UidWarehouse::instance().getElement(guid));
    if(dgram == 0)
    {
        return;
    }
    DiagramForm* df = new DiagramForm(m_workspace->workingVersion(), dgram, this);
    dgram->setForm(df);
    setCentralWidget(dgram->getDiagramForm());
    df->paintDiagram();
}

void MainWindow::showProcedureWithGuid(const QString &guid, bool /*focus*/)
{
    Procedure* p = dynamic_cast<Procedure*>(UidWarehouse::instance().getElement(guid));
    if(p)
    {
        ProcedureForm* pf = m_workspace->workingVersion()->getGui()->getProcedureForm(MODE_PROCEDURE);
        pf->setProcedure(p);
        setCentralWidget(pf);
        pf->showSql();
    }
}

void MainWindow::showFunctionWithGuid(const QString &guid, bool /*focus*/)
{
    Function* p = dynamic_cast<Function*>(UidWarehouse::instance().getElement(guid));
    if(p)
    {
        ProcedureForm* pf = m_workspace->workingVersion()->getGui()->getProcedureForm(MODE_FUNCTION);
        pf->setProcedure(p);
        setCentralWidget(pf);
        pf->showSql();
    }
}

void MainWindow::showTriggerWithGuid(const QString &guid, bool /*focus*/)
{
    Trigger* p = dynamic_cast<Trigger*>(UidWarehouse::instance().getElement(guid));
    if(p)
    {
        TriggerForm* pf = m_workspace->workingVersion()->getGui()->getTriggerForm();
        const QVector<Table*>& allTables = Workspace::getInstance()->workingVersion()->getTables();
        pf->feedInTables(allTables);
        pf->feedInTriggerEvents(Workspace::getInstance()->currentProjectsEngine()->getTriggerEvents());
        pf->feedInTriggerTimes(Workspace::getInstance()->currentProjectsEngine()->getTriggerTimings());
        pf->setTrigger(p);
        setCentralWidget(pf);
        pf->showSql();
    }
}

void MainWindow::showViewWithGuid(const QString& guid, bool /*focus*/)
{
    View* v = dynamic_cast<View*>(UidWarehouse::instance().getElement(guid));
    if(v)
    {
        if(v->getManual())
        {
            m_nvf = new NewViewForm(false, 0, this);

            m_nvf->setSqlSource(v);
            m_nvf->setView(v);
            m_nvf->presentSql(Workspace::getInstance()->currentProject());

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

void MainWindow::showObjectwithGuid(QTreeWidgetItem* current, showSomething s, bool f)
{
    QVariant qv = current->data(0, Qt::UserRole);
    QString uid = qv.toString();
    (this->*s)(uid,f);
}

void MainWindow::currentProjectTreeItemChanged(QTreeWidgetItem * current, QTreeWidgetItem*)
{
    if(current)
    {
        if(current == m_workspace->workingVersion()->getGui()->getTablesItem())
        {// we have clicked on the Tables item (i.e. the list of tables)
            showNamedObjectList(&MainWindow::showTableWithGuid, m_workspace->workingVersion()->getTables(),
                                Workspace::getInstance()->currentProjectIsOop()?IconFactory::getTabinstIcon():IconFactory::getTableIcon(),
                                Workspace::getInstance()->currentProjectIsOop()?"Table Templates":"Tables");
        }
        else
        if(current == m_workspace->workingVersion()->getGui()->getTableInstancesItem())
        {// we have clicked on the Table instances item (i.e. the list of table instances)
            showNamedObjectList(&MainWindow::showTableInstanceWithGuid, m_workspace->workingVersion()->getTableInstances(), IconFactory::getTabinstIcon(), "Tables");
        }
        else
        if(current == m_workspace->currentProject()->getLocation())
        {
            showProjectDetails();
        }
        else
        if(current == m_workspace->workingVersion()->getGui()->getFinalSqlItem())
        {// user clicked on the final SQL item
            SqlForm* frm = m_workspace->workingVersion()->getGui()->getSqlForm();
            frm->setSqlSource(0);
            frm->presentSql(m_workspace->currentProject());
            setCentralWidget(frm);
        }
        else
        if(current == m_workspace->workingVersion()->getGui()->getDiagramsItem())
        {// we have clicked on the Diagrams item (i.e. the list of diagrams)
            showNamedObjectList(&MainWindow::showDiagramWithGuid, m_workspace->workingVersion()->getDiagrams(), IconFactory::getDiagramIcon(), "Diagrams");
        }
        else
        if(current == m_workspace->workingVersion()->getGui()->getViewsItem())
        {// we have clicked on the Views item (i.e. the list of views)
            showNamedObjectList(&MainWindow::showViewWithGuid, m_workspace->workingVersion()->getViews(), IconFactory::getViewIcon(), "Views");
        }
        else
        if(current == m_workspace->workingVersion()->getGui()->getProceduresItem())
        {// we have clicked on the Procedures item (i.e. the list of procedures)
            showNamedObjectList(&MainWindow::showProcedureWithGuid, m_workspace->workingVersion()->getProcedures(), IconFactory::getProcedureIcon(), "Procedures");
        }
        else
        if(current == m_workspace->workingVersion()->getGui()->getFunctionsItem())
        {// we have clicked on the Procedures item (i.e. the list of procedures)
            showNamedObjectList(&MainWindow::showFunctionWithGuid, m_workspace->workingVersion()->getFunctions(), IconFactory::getFunctionTreeIcon(), "Functions");
        }
        else
        if(current == m_workspace->workingVersion()->getGui()->getTriggersItem())
        {// we have clicked on the Triggers item (i.e. the list of triggers)
            showNamedObjectList(&MainWindow::showTriggerWithGuid, m_workspace->workingVersion()->getTriggers(), IconFactory::getTriggerIcon(), "Triggers");
        }
        else
        {
            if(current->parent() && current->parent() == m_workspace->workingVersion()->getGui()->getTablesItem())
            {
                // the user clicked on a table, the name of the table is a tag
                showObjectwithGuid(current, (showSomething)&MainWindow::showTableWithGuid, false);
            }
            else
            if(current->parent() && current->parent() == m_workspace->workingVersion()->getGui()->getDiagramsItem())
            {
                // the user clicked on a diagram
                showObjectwithGuid(current, (showSomething)&MainWindow::showDiagramWithGuid, false);
            }
            else
            if(current->parent() && current->parent() == m_workspace->workingVersion()->getGui()->getTableInstancesItem())
            {
                // user clicked on a table instance
                showObjectwithGuid(current, (showSomething)&MainWindow::showTableInstanceWithGuid, false);
            }
            else
            if(current->parent() && current->parent() == m_workspace->workingVersion()->getGui()->getViewsItem())
            {
                // user clicked on a view
                showObjectwithGuid(current, (showSomething)&MainWindow::showViewWithGuid, false);
            }
            else
            if(current->parent() && current->parent() == m_workspace->workingVersion()->getGui()->getProceduresItem())
            {
                // user clicked on a procedure
                showObjectwithGuid(current, (showSomething)&MainWindow::showProcedureWithGuid, false);
            }
            else
            if(current->parent() && current->parent() == m_workspace->workingVersion()->getGui()->getFunctionsItem())
            {
                // user clicked on a function
                showObjectwithGuid(current, (showSomething)&MainWindow::showFunctionWithGuid, false);
            }
            else
            if(current->parent() && current->parent() == m_workspace->workingVersion()->getGui()->getTriggersItem())
            {
                // user clicked on a procedure
                showObjectwithGuid(current, (showSomething)&MainWindow::showTriggerWithGuid, false);
            }
            else
            if(current->parent() && current->parent() == m_workspace->workingVersion()->getGui()->getFinalSqlItem())
            {
                // user clicked on a SQL item
                SqlForm* frm = new SqlForm(m_workspace->currentProjectsEngine(), this);
                QVariant qv = current->data(0, Qt::UserRole);
                QString name = qv.toString();
                SqlSourceEntity* ent = Workspace::getInstance()->currentProject()->getWorkingVersion()->getSqlSourceEntityWithGuid(name);
                if(ent == 0)
                {   // hm.. this shouldn't be
                    return;
                }
                frm->setSqlSource(ent);
                frm->presentSql(m_workspace->currentProject(), ent, (MainWindow::showSomething)&MainWindow::showNothing);
                setCentralWidget(frm);
            }
            else    // user possibly clicked on a table which had a parent a table ...
            {
                showExistingTable(getNamedObject<Table>(current, (itemGetter)&Version::getTable));
            }
        }
    }
}



void MainWindow::onNewTable()
{
    m_ui->action_NewTable->setDisabled(true);
    NewTableForm* frm = Workspace::getInstance()->workingVersion()->getGui()->getTableFormForNewTable();
    frm->focusOnName();

    m_guiElements->getProjectTree()->setCurrentItem(0);
    setCentralWidget(frm);
    m_ui->action_NewTable->setDisabled(false);
}

void MainWindow::showNewDataTypeWindow(int a)
{
    NewDataTypeForm* frm = new NewDataTypeForm((DT_TYPE)a, m_workspace->currentProjectsEngine(), this);
    frm->focusOnName();
    m_guiElements->getProjectTree()->setCurrentItem(0);
    setCentralWidget(frm);
}

void MainWindow::onNewDataType()
{
    showNewDataTypeWindow(DT_INVALID);
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
        pudt->getLocation()->setIcon(0, IconFactory::getIconForDataType(pudt->getType()));
        pudt->setDisplayText(name);
        pudt->getLocation()->setText(1, pudt->sqlAsString());

        // updating the "data" of the tree item
        QVariant var;
        var.setValue(udt->getName());
        pudt->getLocation()->setData(0, Qt::UserRole, var);
    }
    else        // new stuff
    {
        // add to the project itself
        m_workspace->workingVersion()->addNewDataType(udt);

        // create the tree entry
        ContextMenuEnabledTreeWidgetItem* newDtItem = m_workspace->workingVersion()->getGui()->createDataTypeTreeEntry(udt);

        // set the link to the tree
        m_guiElements->getDataTypesTree()->expandItem(newDtItem);
        m_guiElements->getDataTypesTree()->scrollToItem(newDtItem);
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

    m_guiElements->getProjectTree()->expandAll();

    ProjectDetailsForm* prjDetailsForm = new ProjectDetailsForm(this);
    prjDetailsForm->setProject(m_workspace->currentSolution()->currentProject(), fileName);
    setCentralWidget(prjDetailsForm);

    setWindowTitle(tr("DDM - [") + m_workspace->currentSolution()->name() + tr("] - [") + fileName + tr("]"));
    m_lastLoadedProject = fileName;

    connectActionsFromPopupMenus();

    enableActions();

    delete m_btndlg;
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
    QObject::connect(ContextMenuCollection::getInstance()->getAction_AddTable(), SIGNAL(triggered()), this, SLOT(onNewTable()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_TableAddColumn(), SIGNAL(triggered()), this, SLOT(onTableAddColumnFromPopup()));
    if(m_workspace->currentProjectIsOop())
    {
        // the table popup
        QObject::connect(ContextMenuCollection::getInstance()->getAction_SpecializeTable(), SIGNAL(triggered()), this, SLOT(onSpecializeTableFromPopup()));
        QObject::connect(ContextMenuCollection::getInstance()->getAction_InstantiateTable(), SIGNAL(triggered()), this, SLOT(onInstantiateTableFromPopup()));

        // now the table instance popup
        QObject::connect(ContextMenuCollection::getInstance()->getAction_DeleteTableInstance(), SIGNAL(triggered()), this, SLOT(onDeleteInstanceFromPopup()));
        QObject::connect(ContextMenuCollection::getInstance()->getAction_RenameTableInstance(), SIGNAL(triggered()), this, SLOT(onRenameInstanceFromPopup()));

        // table instances popup
        QObject::connect(ContextMenuCollection::getInstance()->getAction_AddTableInstance(), SIGNAL(triggered()), this, SLOT(onNewTableInstance()));

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
    QObject::connect(ContextMenuCollection::getInstance()->getAction_AddDiagram(), SIGNAL(triggered()), this, SLOT(onNewDiagram()));
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
    QObject::connect(ContextMenuCollection::getInstance()->getAction_DeleteView(), SIGNAL(triggered()), this, SLOT(onDeleteView()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_DeleteProcedure(), SIGNAL(triggered()), this, SLOT(onDeleteProcedure()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_ReleaseMajorVersion(), SIGNAL(triggered()), this, SLOT(onReleaseMajorVersion()));
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
        Version* v = Workspace::getInstance()->currentProject()->getVersionNamed(name)        ;
        if(v)
        {
            m = dynamic_cast<MajorVersion*>(v);
            if(m)
            {
                Workspace::getInstance()->currentProject()->releaseMajorVersion();
            }
        }
    }
}

void MainWindow::onDeleteProcedure()
{
    Procedure* p = getRightClickedObject<Procedure>((itemGetter)&Version::getProcedure);
    if(p)
    {
        if(QMessageBox::question(this, tr("Are you sure?"), tr("Really delete ") + p->getName()+ "?", QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
        {
            return;
        }
        m_workspace->workingVersion()->deleteProcedure(p->getName());
        m_workspace->workingVersion()->getGui()->updateForms();

        showNamedObjectList(&MainWindow::showProcedureWithGuid, m_workspace->workingVersion()->getProcedures(), IconFactory::getProcedureIcon(), "Procedures");

        m_workspace->workingVersion()->getGui()->getProceduresItem()->treeWidget()->clearSelection();
        m_workspace->workingVersion()->getGui()->getProceduresItem()->setSelected(true);
    }
}


void MainWindow::onDeleteView()
{
    View* v = getRightClickedObject<View>((itemGetter)&Version::getView);
    if(v)
    {
        if(QMessageBox::question(this, tr("Are you sure?"), tr("Really delete ") + v->getName()+ "?", QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
        {
            return;
        }
        m_workspace->workingVersion()->deleteView(v->getName());
        m_workspace->workingVersion()->getGui()->updateForms();

        showNamedObjectList(&MainWindow::showViewWithGuid, m_workspace->workingVersion()->getViews(), IconFactory::getViewsIcon(), "Views");

        m_workspace->workingVersion()->getGui()->getViewsItem()->treeWidget()->clearSelection();
        m_workspace->workingVersion()->getGui()->getViewsItem()->setSelected(true);

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
    if(m_guiElements->getProjectTree()->getLastRightclickedItem() == 0)
    {
        return;
    }

    // TODO: Code duplication with some stuff from below (onDelete...)
    ContextMenuEnabledTreeWidgetItem* item = m_guiElements->getProjectTree()->getLastRightclickedItem();
    m_guiElements->getProjectTree()->setLastRightclickedItem(0);

    QVariant qv = item->data(0, Qt::UserRole);
    QString tabUid = qv.toString();
    Table* table =  m_workspace->workingVersion()->getTableWithUid(tabUid);
    if(table == 0)  // shouldn't be ...
    {
        return;
    }

    m_workspace->workingVersion()->getGui()->updateForms();
    TableInstance* tinst = instantiateTable(table->getName(), QStringList());
    showTableInstanceWithGuid(tinst->getObjectUid());
}

void MainWindow::onSpecializeTableFromPopup()
{
    if(m_guiElements->getProjectTree()->getLastRightclickedItem() == 0)
    {
        return;
    }

    ContextMenuEnabledTreeWidgetItem* item = m_guiElements->getProjectTree()->getLastRightclickedItem();
    m_guiElements->getProjectTree()->setLastRightclickedItem(0);

    QVariant qv = item->data(0, Qt::UserRole);
    QString tabUid = qv.toString();
    Table* table =  m_workspace->workingVersion()->getTableWithUid(tabUid);

    Table* specializedTable = new Table(m_workspace->workingVersion(), QUuid::createUuid().toString(), 0);
    specializedTable->initializeFor(Workspace::getInstance()->currentProjectsEngine(), QUuid(uidTable));

    specializedTable->setName(table->getName() + "_specialized");
    specializedTable->setParent(table);
    specializedTable->setParentUid(table->getObjectUid());

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
    if(m_guiElements->getProjectTree()->getLastRightclickedItem() != 0)
    {
        if(m_guiElements->getProjectTree()->getLastRightclickedItem() == 0)
        {
            return;
        }

        ContextMenuEnabledTreeWidgetItem* item = m_guiElements->getProjectTree()->getLastRightclickedItem();
        m_guiElements->getProjectTree()->setLastRightclickedItem(0);

        QVariant qv = item->data(0, Qt::UserRole);
        QString tabUid = qv.toString();
        Table* table =  m_workspace->workingVersion()->getTableWithUid(tabUid);
        NewTableForm* frm = showExistingTable(table);
        frm->focusOnNewColumnName();
        m_guiElements->getProjectTree()->setLastRightclickedItem(0);
    }
}

template <class T>
T* MainWindow::getNamedObject(QTreeWidgetItem* itm, itemGetter g)
{
    QVariant qv = itm->data(0, Qt::UserRole);
    QString name = qv.toString();
    T* t =  reinterpret_cast<T*>((m_workspace->getInstance()->workingVersion()->*g)(name));
    return t;
}

template <class T>
T* MainWindow::getRightClickedObject(itemGetter g)
{
    if(m_guiElements->getProjectTree()->getLastRightclickedItem() != 0)
    {
        ContextMenuEnabledTreeWidgetItem* item = m_guiElements->getProjectTree()->getLastRightclickedItem();
        m_guiElements->getProjectTree()->setLastRightclickedItem(0);

        return getNamedObject<T>(item, g);
    }
    return 0;
}

UserDataType* MainWindow::getRightClickedDatatype()
{
    if(m_guiElements->getDataTypesTree()->getLastRightclickedItem() != 0)
    {
        ContextMenuEnabledTreeWidgetItem* item = m_guiElements->getDataTypesTree()->getLastRightclickedItem();
        m_guiElements->getDataTypesTree()->setLastRightclickedItem(0);

        QVariant qv = item->data(0, Qt::UserRole);
        UserDataType* udt = static_cast<UserDataType*>(qv.data());
        return udt;
    }
    return 0;
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

void MainWindow::onDeleteTableFromPopup()
{

    // TODO: Since the introduction of the GUIDs this code repeats itself over and over
    if(m_guiElements->getProjectTree()->getLastRightclickedItem() == 0)
    {
        return;
    }

    ContextMenuEnabledTreeWidgetItem* item = m_guiElements->getProjectTree()->getLastRightclickedItem();
    m_guiElements->getProjectTree()->setLastRightclickedItem(0);

    QVariant qv = item->data(0, Qt::UserRole);
    QString tabUid = qv.toString();
    Table* tab =  m_workspace->workingVersion()->getTableWithUid(tabUid);

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
                if(ntf->getTableName() == tabName)  // show the list of tables if we deleted the current table
                {
                    showNamedObjectList(&MainWindow::showTableWithGuid, m_workspace->workingVersion()->getTables(),
                                        Workspace::getInstance()->currentProjectIsOop()?IconFactory::getTabinstIcon():IconFactory::getTableIcon(),
                                        Workspace::getInstance()->currentProjectIsOop()?"Table Templates":"Tables");
                    m_workspace->workingVersion()->getGui()->getTablesItem()->treeWidget()->clearSelection();
                    m_workspace->workingVersion()->getGui()->getTablesItem()->setSelected(true);
                }
            }
        }
        m_workspace->workingVersion()->getGui()->updateForms();
    }
}

void MainWindow::onCopyTableFromPopup()
{
    if(m_guiElements->getProjectTree()->getLastRightclickedItem() == 0)
    {
        return;
    }

    ContextMenuEnabledTreeWidgetItem* item = m_guiElements->getProjectTree()->getLastRightclickedItem();
    m_guiElements->getProjectTree()->setLastRightclickedItem(0);

    QVariant qv = item->data(0, Qt::UserRole);
    QString tabUid = qv.toString();
    Table* tab =  m_workspace->workingVersion()->getTableWithUid(tabUid);
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
            sqlItm->setData(0, Qt::UserRole, tab->getObjectUid().toString());
        }

        m_workspace->workingVersion()->getGui()->updateForms();
    }
}


void MainWindow::onDuplicateTableFromPopup()
{
    if(m_guiElements->getProjectTree()->getLastRightclickedItem() == 0)
    {
        return;
    }

    ContextMenuEnabledTreeWidgetItem* item = m_guiElements->getProjectTree()->getLastRightclickedItem();
    m_guiElements->getProjectTree()->setLastRightclickedItem(0);

    QVariant qv = item->data(0, Qt::UserRole);
    QString tabUid = qv.toString();
    Table* tab =  m_workspace->workingVersion()->getTableWithUid(tabUid);

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
            sqlItm->setData(0, Qt::UserRole, dupped->getObjectUid().toString());
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
    doDeployment(t, false);
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

    if(m_workspace && m_workspace->currentSolution() && m_workspace->currentProject() && m_workspace->workingVersion())
    {
        for(int i=0; i<m_workspace->workingVersion()->getTables().size(); i++)
        {
            QAction * actionToAdd = new QAction(this);
            actionToAdd->setText(m_workspace->workingVersion()->getTables()[i]->getName());
            QIcon icon(IconFactory::getTablesIcon());
            actionToAdd->setData(QVariant(m_workspace->workingVersion()->getTables()[i]->getName()));
            actionToAdd->setIcon(icon);
            createTableInstancesPopup->addAction(actionToAdd);
            QObject::connect(actionToAdd, SIGNAL(triggered()),
                             new DynamicActionHandlerforMainWindow(m_workspace->workingVersion()->getTables()[i]->getName(), this, (MainWindow::dynamicAction)&MainWindow::instantiateTableCallback), SLOT(called()));
        }
    }
}

void MainWindow::instantiateTableCallback(const QString &tabName)
{
    instantiateTable(tabName, QStringList());
}

TableInstance *MainWindow::instantiateTable(const QString& tabName, QStringList othersTablesBeingInstantiated, bool ref, Table* referencingTable, TableInstance* becauseOfThis)
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
                instantiateTable(tbl->getName(), othersTablesBeingInstantiated, true, tinst->table(), tinst);
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
            scw->updateSql(m_workspace->currentProject());
        }
    }
}

void MainWindow::onRenameInstanceFromPopup()
{
    if(m_guiElements->getProjectTree()->getLastRightclickedItem() == 0)
    {
        return;
    }

    ContextMenuEnabledTreeWidgetItem* item = m_guiElements->getProjectTree()->getLastRightclickedItem();
    m_guiElements->getProjectTree()->setLastRightclickedItem(0);

    QVariant qv = item->data(0, Qt::UserRole);
    QString tabUid = qv.toString();
    TableInstance* tinst =  m_workspace->workingVersion()->getTableInstanceWithUid(tabUid);

    if(tinst)
    {
        SimpleTextInputDialog* dlg = new SimpleTextInputDialog(this, tr("Enter the new name"));
        dlg->setModal(true);
        dlg->setText(tinst->getName());
        if(dlg->exec() == QDialog::Accepted)
        {
            QString t = dlg->getText();
            if(m_workspace->getInstance()->workingVersion()->getTableInstance(t))
            {
                QMessageBox::critical(this, tr("Error"), tr("You can have only one table instance called ") + t, QMessageBox::Ok);
                return;
            }
            tinst->setName(t);
            tinst->setDisplayText(t);
            tinst->getSqlLocation()->setText(0, t);
        }
    }
}

void MainWindow::onRenameDiagramFromPopup()
{
    if(m_guiElements->getProjectTree()->getLastRightclickedItem() == 0)
    {
        return;
    }

    ContextMenuEnabledTreeWidgetItem* item = m_guiElements->getProjectTree()->getLastRightclickedItem();
    m_guiElements->getProjectTree()->setLastRightclickedItem(0);

    QVariant qv = item->data(0, Qt::UserRole);
    QString tabUid = qv.toString();
    Diagram* dgr =  m_workspace->workingVersion()->getDiagramWithUid(tabUid);
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

void MainWindow::onDeleteInstanceFromPopup()
{
    if(m_guiElements->getProjectTree()->getLastRightclickedItem() == 0)
    {
        return;
    }

    ContextMenuEnabledTreeWidgetItem* item = m_guiElements->getProjectTree()->getLastRightclickedItem();
    m_guiElements->getProjectTree()->setLastRightclickedItem(0);

    QVariant qv = item->data(0, Qt::UserRole);
    QString tabUid = qv.toString();
    TableInstance* tinst =  m_workspace->workingVersion()->getTableInstanceWithUid(tabUid);

    //TableInstance* tinst = getRightClickedObject<TableInstance>((itemGetter)&Version::getTableInstance);
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

        // mark for purge
        m_workspace->workingVersion()->deleteTableInstance(tinst);

        // Now delete the tree entries and the SQL tree entry
        for(int i=0; i<m_workspace->workingVersion()->getTableInstances().size(); i++)
        {
            if(m_workspace->workingVersion()->getTableInstances().at(i)->sentenced())
            {
                QString uid = m_workspace->workingVersion()->getTableInstances().at(i)->getObjectUid();
                ContextMenuEnabledTreeWidgetItem* tabInstItem = m_workspace->workingVersion()->getGui()->getTableInstancesItem();
                for(int j=0; j<tabInstItem->childCount(); j++)
                {
                    QVariant a = tabInstItem->child(j)->data(0, Qt::UserRole);
                    if(a.toString() == uid)
                    {
                        delete tabInstItem->child(j);
                    }
                }

                ContextMenuEnabledTreeWidgetItem* sqlItem = m_workspace->workingVersion()->getGui()->getFinalSqlItem();
                for(int j=0; j<sqlItem->childCount(); j++)
                {
                    QVariant a = sqlItem->child(j)->data(0, Qt::UserRole);
                    if(a.toString() == uid)
                    {
                        delete sqlItem->child(j);
                    }
                }
            }
        }

        // and finally remove all purged table instances
        m_workspace->workingVersion()->purgeSentencedTableInstances();

        // and show a table instances list form
        showNamedObjectList(&MainWindow::showTableWithGuid, m_workspace->workingVersion()->getTables(),
                            Workspace::getInstance()->currentProjectIsOop()?IconFactory::getTabinstIcon():IconFactory::getTableIcon(),
                            Workspace::getInstance()->currentProjectIsOop()?"Table Templates":"Tables");

        m_workspace->workingVersion()->getGui()->getDiagramsItem()->treeWidget()->clearSelection();
        m_workspace->workingVersion()->getGui()->getTableInstancesItem()->setSelected(true);

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
    ContextMenuEnabledTreeWidgetItem* itm = m_guiElements->getDataTypesTree()->getLastRightclickedItem();
    if(!itm)
    {
        return;
    }
    UserDataType* udt = getRightClickedDatatype();
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
            Table* t = Workspace::getInstance()->currentProjectsEngine()->reverseEngineerTable(c, tab, Workspace::getInstance()->currentProject(), true);
            t->setName(NameGenerator::getUniqueName(Workspace::getInstance()->currentProject()->getWorkingVersion(), (itemGetter)&Version::getTable, tab));
            Workspace::getInstance()->currentProject()->getWorkingVersion()->addTable(t);
            m_workspace->workingVersion()->getGui()->createTableTreeEntry(t, m_workspace->workingVersion()->getGui()->getTablesItem());
            showTableWithGuid(t->getObjectUid());
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
        c->getEngine()->dropDatabase(c);
        c->getEngine()->createDatabase(c);
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

        InjectSqlDialog* ij = new InjectSqlDialog(0);
        ij->setModal(true);
        ij->populateConnectionDetails(c);
        if(ij->exec()  == QDialog::Accepted)
        {
            c->resetTo(ij->getName(), ij->getHost(), ij->getUser(), ij->getPassword(), ij->getDatabase(), true, ij->getAutoConnect());
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
    UserDataType* udt = getRightClickedDatatype();
    if(udt)
    {
        UserDataType* dup = m_workspace->workingVersion()->duplicateDataType(udt->getName());

        ContextMenuEnabledTreeWidgetItem* newDTItem = m_workspace->workingVersion()->getGui()->createDataTypeTreeEntry(dup);

        m_guiElements->getDataTypesTree()->expandItem(newDTItem);
        m_guiElements->getDataTypesTree()->scrollToItem(newDTItem);

        // resize to look better
        m_guiElements->getDataTypesTree()->resizeColumnToContents(0);
        m_guiElements->getDataTypesTree()->resizeColumnToContents(1);
    }
}

void MainWindow::onDeleteDiagramFromPopup()
{
    if(m_guiElements->getProjectTree()->getLastRightclickedItem() == 0)
    {
        return;
    }

    ContextMenuEnabledTreeWidgetItem* item = m_guiElements->getProjectTree()->getLastRightclickedItem();
    m_guiElements->getProjectTree()->setLastRightclickedItem(0);

    QVariant qv = item->data(0, Qt::UserRole);
    QString tabUid = qv.toString();
    Diagram* dgr =  m_workspace->workingVersion()->getDiagramWithUid(tabUid);
    if(dgr)
    {
        if(QMessageBox::question(this, tr("Are you sure?"), tr("Really delete ") + dgr->getName()+ "?", QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
        {
            return;
        }
        m_workspace->workingVersion()->deleteDiagram(dgr->getName());
        m_workspace->workingVersion()->getGui()->updateForms();

        showNamedObjectList(&MainWindow::showDiagramWithGuid, m_workspace->workingVersion()->getTables(), IconFactory::getDiagramIcon(), "Diagrams");
        m_workspace->workingVersion()->getGui()->getDiagramsItem()->treeWidget()->clearSelection();
        m_workspace->workingVersion()->getGui()->getDiagramsItem()->setSelected(true);

    }
}

void MainWindow::dtTreeItemClicked ( QTreeWidgetItem *, int)
{
    onDTTreeClicked();
}

void MainWindow::onNewStringType()
{
    showNewDataTypeWindow(DT_STRING);
}

void MainWindow::onNewNumericType()
{
    showNewDataTypeWindow(DT_NUMERIC);
}

void MainWindow::onNewBoolType()
{
    showNewDataTypeWindow(DT_BOOLEAN);
}

void MainWindow::onNewDateTimeType()
{
    showNewDataTypeWindow(DT_DATETIME);
}

void MainWindow::onNewBlobType()
{
    showNewDataTypeWindow(DT_BLOB);
}

void MainWindow::onNewMiscType()
{
    showNewDataTypeWindow(DT_MISC);
}

void MainWindow::onNewSpatialType()
{
    showNewDataTypeWindow(DT_SPATIAL);
}

void MainWindow::onGotoIssueLocation()
{
    Table* tab = 0;
    if(!m_guiElements->getIssuesTree()->getLastRightclickedItem()) return;
    QVariant a = m_guiElements->getIssuesTree()->getLastRightclickedItem()->data(0, Qt::UserRole);
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
    lblStatus->setText(QApplication::translate("MainWindow", "Deploying", 0, QApplication::UnicodeUTF8));

}

void MainWindow::onDeploy()
{
    InjectSqlDialog* injectDialog = new InjectSqlDialog(m_workspace->getInstance()->currentProjectsEngine(), this);
    injectDialog->setModal(true);
    if(injectDialog->exec() == QDialog::Accepted)
    {
        QStringList connectionNames = injectDialog->getSelectedConnections();
        doDeployment(connectionNames, injectDialog->injectMetadataRequired());
    }
}

void MainWindow::doDeployment(QStringList connectionNames, bool metadataInject)
{
    createStatusLabel();
    InjectSqlGenerator* injectSqlGen = new InjectSqlGenerator(m_workspace->workingVersion(), connectionNames, 0, metadataInject);
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
    QMap<Connection*, QStringList> sqlList = generator->getSqls();
    QStringList connectionNames = generator->getConnectionNames();
    Deployer* deployer = new Deployer(connectionNames, sqlList,
                                      generator->metadataInjectRequired(),
                                      generator->getVersion(), 0);
    m_deployers.append(deployer);
    connect(deployer, SIGNAL(done(Deployer*)), this, SLOT(onDeploymentFinished(Deployer*)));
    deployer->deploy();
}

void MainWindow::onDeploymentFinished(Deployer *d)
{
    if(d->hadErrors())
    {
        QMap<QString, QString> errors = d->getErrors();
        m_guiElements->getIssuesDock()->show();
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
    bool t = m_guiElements->getDataTypesTree()->isVisible() ;
    m_ui->action_Datatypes_Tree->setChecked(!t);
    m_guiElements->getDataTypesTree()->setVisible(!t);
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
    Project* p = m_workspace->currentProject();
    bool c = !m_revEngWizard->createDataTypesForColumns();
    DatabaseEngine* engine = m_workspace->currentProjectsEngine();

    createStatusLabel();
    lblStatus->setText(QApplication::translate("MainWindow", "Reverse engineering started", 0, QApplication::UnicodeUTF8));

    ReverseEngineerer* revEng = new ReverseEngineerer(c, engine, p, host, user, pass, db,
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
    m_nvf = new NewViewForm(false, 0, this);
    View* v = new View(true, QUuid::createUuid().toString());
    Workspace::getInstance()->workingVersion()->addView(v);
    Workspace::getInstance()->workingVersion()->getGui()->createViewTreeEntry(v);

    m_nvf->setSqlSource(v);
    m_nvf->setView(v);
    m_nvf->presentSql(Workspace::getInstance()->currentProject());

    setCentralWidget(m_nvf);
}

void MainWindow::onNewView()
{
    View* v = new View(false, QUuid::createUuid().toString());
    m_nvf = 0;
    v->getHelper()->setForm(this);
    Workspace::getInstance()->workingVersion()->addView(v);
    Workspace::getInstance()->workingVersion()->getGui()->createViewTreeEntry(v);

    rerenderQuery(v->getQuery());
}

void MainWindow::onNewProcedure()
{
    ProcedureForm* frm = Workspace::getInstance()->workingVersion()->getGui()->getProcedureForm(MODE_PROCEDURE);
    Procedure* p = new Procedure(NameGenerator::getUniqueName(Workspace::getInstance()->workingVersion(), (itemGetter)&Version::getProcedure, QString("proc")), QUuid::createUuid().toString());
    frm->setProcedure(p);
    frm->initSql();
    Workspace::getInstance()->workingVersion()->addProcedure(p);
    Workspace::getInstance()->workingVersion()->getGui()->createProcedureTreeEntry(p);

    //m_guiElements->getProjectTree()->setCurrentItem(p->getLocation());
    setCentralWidget(frm);
}

void MainWindow::onNewFunction()
{
    ProcedureForm* frm = Workspace::getInstance()->workingVersion()->getGui()->getProcedureForm(MODE_FUNCTION);
    Function* func = new Function(NameGenerator::getUniqueName(Workspace::getInstance()->workingVersion(), (itemGetter)&Version::getFunction, QString("func")), QUuid::createUuid().toString());
    frm->setProcedure(func);
    frm->initSql();
    Workspace::getInstance()->workingVersion()->addFunction(func);
    Workspace::getInstance()->workingVersion()->getGui()->createFunctionTreeEntry(func);

    //m_guiElements->getProjectTree()->setCurrentItem(p->getLocation());
    setCentralWidget(frm);
}

void MainWindow::onNewTrigger()
{
    TriggerForm* frm = Workspace::getInstance()->workingVersion()->getGui()->getTriggerForm();
    Trigger* trigger = new Trigger(NameGenerator::getUniqueName(Workspace::getInstance()->workingVersion(), (itemGetter)&Version::getTrigger, QString("trig")), QUuid::createUuid().toString());
    frm->setTrigger(trigger);
    frm->initSql();
    const QVector<Table*>& allTables = Workspace::getInstance()->workingVersion()->getTables();
    frm->feedInTables(allTables);
    frm->feedInTriggerEvents(Workspace::getInstance()->currentProjectsEngine()->getTriggerEvents());
    frm->feedInTriggerTimes(Workspace::getInstance()->currentProjectsEngine()->getTriggerTimings());
    Workspace::getInstance()->workingVersion()->addTrigger(trigger);
    Workspace::getInstance()->workingVersion()->getGui()->createTriggerTreeEntry(trigger);

    //m_guiElements->getProjectTree()->setCurrentItem(trigger->getLocation());
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

    m_nvf = new NewViewForm(true, q->getHelper(), this);
    m_nvf->setSqlSource(q->getSourceEntity());
    m_nvf->setView(dynamic_cast<View*>(q->getSourceEntity()));

    m_nvf->setGraphicsItem(q->getGraphicsItem());
    q->getHelper()->setScene(m_nvf->getScene());
    m_nvf->presentSql(Workspace::getInstance()->currentProject());
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
