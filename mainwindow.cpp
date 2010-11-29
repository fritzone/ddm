#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mainwindowbuttondialog.h"
#include "newprojectdialog.h"
#include "datatypeslistform.h"
#include "newdatatypeform.h"
#include "UserDataType.h"
#include "Project.h"
#include "DatabaseEngine.h"
#include "Version.h"
#include "AbstractDTSupplier.h"
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

#include <QtGui>

#ifdef Q_WS_X11
//Q_IMPORT_PLUGIN(qsqlmysql)
#endif

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), m_ui(new Ui::MainWindow), m_projectTreeDock(0), m_datatypesTreeDock(0), m_projectTree(0),
    m_datatypesTree(0), m_btndlg(0), m_newTableForm(0), m_createTableInstancesPopup(0), m_workspace(0), m_revEngWizard(0)
{
    m_ui->setupUi(this);

    m_createTableInstancesPopup = new QMenu();
    m_createTableInstancesPopup->clear();

    showButtonDialog();

    m_ui->action_NewTableInstance->setMenu(m_createTableInstancesPopup);
    m_ui->action_NewDataType->setMenu(ContextMenuCollection::getInstance()->getDatatypesPopupMenu());

    Configuration::instance();

    m_workspace = Workspace::getInstance();

}

MainWindow::~MainWindow()
{
    delete m_ui;
}

void MainWindow::showButtonDialog()
{
    m_btndlg = new MainWindowButtonDialog();
    m_btndlg->setMainWindow(this);

    setCentralWidget(m_btndlg);

    setWindowTitle(tr("DDM - [No Solution]"));
    resize(800, 600);
    showNormal();
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

void MainWindow::setupGuiForNewSolution()
{
    // create the dock window
    m_projectTreeDock = new QDockWidget(tr("Solution - ") + m_workspace->currentSolution()->name(), this);
    m_projectTreeDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_projectTreeDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
    m_projectTreeDock->setFloating(false);
    m_projectTreeDock->setMinimumSize(300, 340);
    m_projectTreeDock->setMaximumSize(500, 840);
    m_projectTreeDock->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

    m_projectTreeDock->resize(301,341);

    m_datatypesTreeDock = new QDockWidget(tr("DataTypes") , this);
    m_datatypesTreeDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_datatypesTreeDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
    m_datatypesTreeDock->setFloating(false);
    m_datatypesTreeDock->setMinimumSize(300, 340);
    m_datatypesTreeDock->setMaximumSize(500, 840);

    // set up the tree
    ContextMenuHandler* contextMenuHandler = new ContextMenuHandler();

    m_projectTree = new ContextMenuEnabledTreeWidget();
    m_projectTree->setAllColumnsShowFocus(true);
    m_projectTree->setSelectionMode(QAbstractItemView::SingleSelection);
    m_projectTree->setSelectionBehavior(QAbstractItemView::SelectItems);
    m_projectTree->setItemDelegate(new ContextMenuDelegate(contextMenuHandler,m_projectTree));
    m_projectTree->setColumnCount(1);
    m_projectTree->setHeaderHidden(true);
    QObject::connect(m_projectTree, SIGNAL (currentItemChanged ( QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(currentProjectTreeItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
    QObject::connect(m_projectTree, SIGNAL (itemClicked ( QTreeWidgetItem * , int ) ), this, SLOT(projectTreeItemClicked (QTreeWidgetItem*,int)));

    m_datatypesTree = new ContextMenuEnabledTreeWidget();
    m_datatypesTree ->setColumnCount(2);
    QTreeWidgetItem *hdr = m_datatypesTree->headerItem();
    hdr->setText(0, tr("Type"));
    hdr->setText(1, tr("SQL"));
    m_datatypesTree->header()->setDefaultSectionSize(200);
    m_datatypesTree->setItemDelegate(new ContextMenuDelegate(contextMenuHandler,m_datatypesTree));
    m_datatypesTree ->setHeaderHidden(false);
    QObject::connect(m_datatypesTree, SIGNAL(itemSelectionChanged()), this, SLOT(onDTTreeClicked()));
    QObject::connect(m_datatypesTree, SIGNAL (itemClicked ( QTreeWidgetItem * , int ) ), this, SLOT(dtTreeItemClicked(QTreeWidgetItem*,int)));

    m_datatypesTreeDock->setWidget(m_datatypesTree);
    m_projectTreeDock->setWidget(m_projectTree);
    addDockWidget(Qt::LeftDockWidgetArea, m_projectTreeDock);
    addDockWidget(Qt::LeftDockWidgetArea, m_datatypesTreeDock);

    showMaximized();
}

ContextMenuEnabledTreeWidgetItem* MainWindow::createDataTypeTreeEntry(UserDataType* udt)
{
    return m_workspace->workingVersion()->getGui()->createDataTypeTreeEntry(udt);
}

void MainWindow::createDatamodelProject(NewProjectDialog* nprjdlg)
{
}

void MainWindow::onNewSolution()
{
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
                return;
            }
        }
        else // we are creating a new solution, it will be appended to the solutions in the workspace, but in release 0.2 this is not supported yet
        {
            if(!m_workspace->createSolution(nprjdlg->getSolutionName()))
            {
                QMessageBox::critical (this, tr("Error"), tr("Cannot create a solution. Not enough memory?"), QMessageBox::Ok);
                return;
            }
        }

        setupGuiForNewSolution();

        Project* project = new Project(nprjdlg->getProjectName().toUpper(), m_projectTree, m_datatypesTree, nprjdlg->enableOOPFeatures());
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

        if(nprjdlg->getProjectType() == NewProjectDialog::PRJ_DATAMODEL)
        {
            createDatamodelProject(nprjdlg);
        }
        else
        {
            m_revEngWizard = new ReverseEngineerWizard(nprjdlg->getDatabaseEngine());
            m_revEngWizard->setWizardStyle(QWizard::ClassicStyle);
            m_revEngWizard->show();

            QObject::connect(m_revEngWizard, SIGNAL(currentIdChanged(int)), this, SLOT(onReverseEngineerWizardNextPage(int)));
            QObject::connect(m_revEngWizard, SIGNAL(accepted()), this, SLOT(onReverseEngineerWizardAccept()));

            /*InjectSqlDialog* dlg = new InjectSqlDialog(nprjdlg->getDatabaseEngine(), this);
            dlg->setupForReverseEngineering();
            dlg->setModal(true);
            if(dlg->exec() == QDialog::Accepted)
            {
                project->getEngine()->reverseEngineerDatabase(dlg->getHost(), dlg->getUser(), dlg->getPassword(), dlg->getDatabase(), project->getWorkingVersion());
            }*/
        }

        enableActions();
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
    }
}

void MainWindow::showTable(const QString &tabName, bool focus)
{
    Table* table =  m_workspace->workingVersion()->getTable(tabName);
    if(table == 0)  // shouldn't be ...
    {
        return;
    }

    m_newTableForm = new NewTableForm(m_workspace->currentProjectsEngine(), m_workspace->currentProject(), this, false);
    m_newTableForm->setTable(table);
    m_newTableForm->focusOnName();

    setCentralWidget(m_newTableForm);

    if(focus) m_projectTree->setCurrentItem(table->getLocation());
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
}

void MainWindow::currentProjectTreeItemChanged(QTreeWidgetItem * current, QTreeWidgetItem*)
{
    if(current)
    {
        if(current == m_workspace->workingVersion()->getGui()->getTablesItem())
        {// we have clicked on the Tables item (i.e.t. the list of tables)
            TablesListForm* tblLst = new TablesListForm(this);
            tblLst->populateTables(m_workspace->workingVersion()->getTables());
            tblLst->setOop(m_workspace->currentProjectIsOop());
            setCentralWidget(tblLst);
        }
        else
        if(current == m_workspace->workingVersion()->getGui()->getTableInstancesItem())
        {// we have clicked on the Table instances item (i.e.t. the list of table instances)
            TableInstancesListForm* tblLst = new TableInstancesListForm(this);
            tblLst->populateTableInstances(m_workspace->workingVersion()->getTableInstances());
            setCentralWidget(tblLst);
        }
        else
        if(current == m_workspace->currentProject()->getLocation())
        {
            showProjectDetails();
        }
        else
        if(current == m_workspace->workingVersion()->getGui()->getFinalSqlItem())
        {
            SqlForm* frm = new SqlForm(m_workspace->currentProjectsEngine(), this);
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

                frm->setSqlSource(ent);
                frm->presentSql(m_workspace->currentProject(), ent, m_workspace->currentProject()->getCodepage());
                setCentralWidget(frm);

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
                m_newTableForm = new NewTableForm(m_workspace->currentProjectsEngine(), m_workspace->currentProject(), this, false);
                m_newTableForm->setTable(table);
                m_newTableForm->focusOnName();

                setCentralWidget(m_newTableForm);

            }
        }
    }
}

void MainWindow::onNewTable()
{
    NewTableForm* frm = new NewTableForm(m_workspace->currentProjectsEngine(), m_workspace->currentProject(), this, true);
    frm->focusOnName();

    m_projectTree->setCurrentItem(0);
    setCentralWidget(frm);
}


void MainWindow::showNewDataTypeWindow(int a)
{
    NewDataTypeForm* frm = new NewDataTypeForm((DataType::DT_TYPE)a, m_workspace->currentProjectsEngine(), this);
    frm->focusOnName();
    frm->setMainWindow(this);
    m_projectTree->setCurrentItem(0);
    setCentralWidget(frm);
}

void MainWindow::onNewDataType()
{
    showNewDataTypeWindow(DataType::DT_INVALID);
}

bool MainWindow::onUpdateTable(Table* tbl)
{
    QTreeWidgetItem* tblsItem = tbl->getLocation();
    QVariant var(tbl->getName());
    tblsItem->setData(0, Qt::UserRole, var);
    tblsItem->setText(0, tbl->getName());
    return true;
}

bool MainWindow::onSaveNewTable(Table* tbl)
{
    m_workspace->workingVersion()->getGui()->createTableTreeEntry(tbl);
    m_workspace->workingVersion()->addTable(tbl);
    return true;
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
}

void MainWindow::onSaveProject()
{
    saveProject(false);
}

void MainWindow::populateTreeWithSolution(Solution* sol)
{
    for(int i=0; i<sol->projects().size(); i++)
    {
        sol->projects()[i]->createTreeItem(m_projectTree, m_datatypesTree);
        sol->projects()[i]->populateTreeItem();
    }
}

void MainWindow::onOpenProject()
{
    QString fileName = QFileDialog::getOpenFileName(this,  tr("Open solution"), "", tr("DDM solution files (*.dmx);;All files (*.*)"));
    if(fileName.length() == 0)
    {
        return;
    }

    if(!m_workspace->loadSolution(fileName))
    {
        QMessageBox::critical (this, tr("Error"), tr("Cannot load the solution."), QMessageBox::Ok);
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

}

void MainWindow::onAbout()
{
    AboutBoxDialog* about = new AboutBoxDialog(this);
    about->setModal(true);
    about->exec();
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

    m_projectTree->setCurrentItem(instantiateTable(table->getName()));
}

void MainWindow::onSpecializeTableFromPopup()
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

    Table* tbl = new Table(m_workspace->workingVersion());
    tbl->setName(table->getName() + "_specialized");
    tbl->setParent(table);
    tbl->setStorageEngine(table->getStorageEngine());

    // TODO: Code duplication from the "Save table"
    ContextMenuEnabledTreeWidgetItem* newTblsItem = new ContextMenuEnabledTreeWidgetItem(item, QStringList(tbl->getName())) ;

    QVariant var(tbl->getName());
    newTblsItem->setData(0, Qt::UserRole, var);
    newTblsItem->setPopupMenu(ContextMenuCollection::getInstance()->getTablePopupMenu());
    // set the icon, add to the tree
    newTblsItem->setIcon(0, IconFactory::getTablesIcon());
    m_projectTree->insertTopLevelItem(0, newTblsItem);

    // add to the project itself
    m_workspace->workingVersion()->addTable(tbl);

    // set the link to the tree
    tbl->setLocation(newTblsItem);

    // now open the new table for to show this table
    m_projectTree->setCurrentItem(newTblsItem);
    m_projectTree->scrollToItem(newTblsItem);
    if(m_newTableForm != 0)
    {
        m_newTableForm->selectTab(0);
        m_newTableForm->focusOnNewColumnName();
    }
}

void MainWindow::onTableAddColumnFromPopup()
{
    if(m_projectTree->getLastRightclickedItem() != 0)
    {
        m_projectTree->setCurrentItem(m_projectTree->getLastRightclickedItem(), 0);  // this is supposed to open the window "frm" too
        if(m_newTableForm != 0)
        {
            m_newTableForm->selectTab(0);
            m_newTableForm->focusOnNewColumnName();
            m_projectTree->setLastRightclickedItem(0);
        }
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

void MainWindow::onDeleteTableFromPopup()
{
    Table* tab = getRightclickedTable();
    QWidget* w = centralWidget();
    if(tab)
    {
        m_workspace->workingVersion()->deleteTable(tab);
    }
    if(dynamic_cast<DiagramForm*>(w))
    {
        dynamic_cast<DiagramForm*>(w)->paintDiagram();
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
    }
}


void MainWindow::onDuplicateTableFromPopup()
{
    Table* tab = getRightclickedTable();
    if(tab)
    {
        Table* dupped = m_workspace->workingVersion()->duplicateTable(tab);
        onSaveNewTable(dupped);
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
            instantiateTable(items.at(i));
        }
    }
}

void MainWindow::onNewTableInstanceHovered()
{
    if(!m_createTableInstancesPopup)
    {
        return;
    }
    m_createTableInstancesPopup->clear();

    if(m_workspace->currentSolution() && m_workspace->currentProject() && m_workspace->workingVersion())
    {
        for(int i=0; i<m_workspace->workingVersion()->getTables().size(); i++)
        {
            QAction * actionToAdd = new QAction(this);
            actionToAdd->setText(m_workspace->workingVersion()->getTables()[i]->getName());
            QIcon icon(IconFactory::getTablesIcon());
            actionToAdd->setData(QVariant(m_workspace->workingVersion()->getTables()[i]->getName()));
            actionToAdd->setIcon(icon);
            m_createTableInstancesPopup->addAction(actionToAdd);
            QObject::connect(actionToAdd, SIGNAL(activated()),
                             new DynamicActionHandlerforMainWindow(m_workspace->workingVersion()->getTables()[i]->getName(), this), SLOT(called()));
        }
    }
}

ContextMenuEnabledTreeWidgetItem* MainWindow::instantiateTable(const QString& tabName, bool ref, Table* referencingTable, TableInstance* becauseOfThis)
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
            instantiateTable(tbl->getName(), true, tinst->table(), tinst);
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
        }
    }
}

void MainWindow::onDeleteInstanceFromPopup()
{
    TableInstance* tinst = getRightclickedTableInstance();
    if(tinst)
    {
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
        TableInstancesListForm* tblLst = new TableInstancesListForm(this);
        tblLst->populateTableInstances(m_workspace->workingVersion()->getTableInstances());
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
        m_workspace->workingVersion()->deleteDiagram(dgr->getName());
    }
}

void MainWindow::projectTreeItemClicked ( QTreeWidgetItem * item, int )
{
    currentProjectTreeItemChanged(item, 0);
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

void MainWindow::onDeploy()
{
    InjectSqlDialog* injectDialog = new InjectSqlDialog(m_workspace->getInstance()->currentProjectsEngine(), this);
    injectDialog->setModal(true);
    if(injectDialog->exec() == QDialog::Accepted)
    {
        QStringList sqlList = m_workspace->workingVersion()->getSqlScript(injectDialog->getCodepage());
        QString tSql;
        if(!m_workspace->currentProjectsEngine()->injectSql(injectDialog->getHost(), injectDialog->getUser(), injectDialog->getPassword(), injectDialog->getDatabase(), sqlList, tSql, injectDialog->getRollbackOnError(), injectDialog->getCreateOnlyIfNotExist()))
        {
            QMessageBox::critical (this, tr("Error"), tr("<B>Cannot execute a query!</B><P>Reason: ") + m_workspace->currentProjectsEngine()->getLastError() + tr(".<P>Query:<PRE>") + tSql+ "</PRE><P>" +
                                   (injectDialog->getRollbackOnError()?tr("Transaction was rolled back."):tr("Transaction was <font color=red><B>NOT</B></font> rolled back, you might have partial data in your database.")), QMessageBox::Ok);
            return;
        }
        QMessageBox::information(this, tr("Deployed"), tr("The solution was deployed to ") + injectDialog->getDatabase(), QMessageBox::Ok);
    }
}

void MainWindow::onViewProjectTree()
{
    bool t = m_projectTreeDock->isVisible() ;
    m_ui->action_ProjectTree->setChecked(!t);
    m_projectTreeDock->setVisible(!t);
}

void MainWindow::onViewDatatypesTree()
{
    bool t = m_datatypesTreeDock->isVisible() ;
    m_ui->action_ProjectTree->setChecked(!t);
    m_projectTreeDock->setVisible(!t);
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
    }
}

void MainWindow::onReverseEngineerWizardAccept()
{
    QVector<QString> tabsToReverse = m_revEngWizard->getTablesToReverse();
    m_workspace->currentProjectsEngine()->reverseEngineerDatabase(m_revEngWizard->getHost(), m_revEngWizard->getUser(), m_revEngWizard->getPasword(), m_revEngWizard->getDatabase(),
                                                                  tabsToReverse, m_workspace->currentProject(), !m_revEngWizard->createDataTypesForColumns());
}
