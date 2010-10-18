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

#include <QtGui>

#ifdef Q_WS_X11
Q_IMPORT_PLUGIN(qsqlmysql)
#endif

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), m_ui(new Ui::MainWindow), m_projectTreeDock(0), m_projectTree(0),
    m_btndlg(0), m_newTableForm(0), m_createTableInstancesPopup(0)
{
    m_ui->setupUi(this);

    m_btndlg = new MainWindowButtonDialog();
    m_btndlg->setMainWindow(this);

    setCentralWidget(m_btndlg);

    setWindowTitle(tr("DDM - [No Solution]"));
    resize(800, 600);

    m_createTableInstancesPopup = new QMenu();
    m_createTableInstancesPopup->clear();

    m_ui->action_NewTableInstance->setMenu(m_createTableInstancesPopup);

    Configuration::instance();

}

MainWindow::~MainWindow()
{
    delete m_ui;
}

void MainWindow::setupGuiForNewSolution()
{
    // create the dock window
    m_projectTreeDock = new QDockWidget(tr("Solution - ") + Workspace::getInstance()->currentSolution()->name(), this);
    m_projectTreeDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_projectTreeDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
    m_projectTreeDock->setFloating(false);
    m_projectTreeDock->setMinimumSize(300, 340);
    m_projectTreeDock->setMaximumSize(360, 840);
    m_projectTreeDock->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

    m_projectTreeDock->resize(301,341);

    m_datatypesTreeDock = new QDockWidget(tr("DataTypes") , this);
    m_datatypesTreeDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_datatypesTreeDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
    m_datatypesTreeDock->setFloating(false);
    m_datatypesTreeDock->setMinimumSize(300, 340);
    m_datatypesTreeDock->setMaximumSize(400, 840);

    // set up the tree
    m_projectTree = new ContextMenuEnabledTreeWidget();
    m_projectTree->setAllColumnsShowFocus(true);
    m_projectTree->setSelectionMode(QAbstractItemView::SingleSelection);
    m_projectTree->setSelectionBehavior(QAbstractItemView::SelectItems);

    ContextMenuHandler* contextMenuHandler = new ContextMenuHandler();

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
    QStringList itm(udt->getName());
    itm << udt->sqlAsString();
    ContextMenuEnabledTreeWidgetItem* newDTItem = new ContextMenuEnabledTreeWidgetItem(Workspace::getInstance()->workingVersion()->getDtsItem(), itm) ;

    QVariant var;
    var.setValue(*udt);
    newDTItem->setData(0, Qt::UserRole, var);
    // set the icon, add to the tree
    newDTItem->setIcon(0, udt->getIcon());
    newDTItem->setPopupMenu(ContextMenuCollection::getInstance()->getDatatypePopupMenu());
    m_datatypesTree->insertTopLevelItem(0,newDTItem);

    // set the link to the tree
    udt->setLocation(newDTItem);

    return newDTItem;
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

        if(! Workspace::getInstance()->hasCurrentSolution())
        {
            if(!Workspace::getInstance()->createCurrentSolution(nprjdlg->getSolutionName()))
            {
                QMessageBox::critical (this, tr("Error"), tr("Cannot create a solution. Not enough memory?"), QMessageBox::Ok);
                return;
            }
        }
        else // we are creating a new solution, it will be appended to the solutions in the workspace, but in release 0.2 this is not supported yet
        {
            if(!Workspace::getInstance()->createSolution(nprjdlg->getSolutionName()))
            {
                QMessageBox::critical (this, tr("Error"), tr("Cannot create a solution. Not enough memory?"), QMessageBox::Ok);
                return;
            }
        }

        if(m_projectTree)
        {
            delete m_projectTreeDock;
            m_projectTreeDock = 0;
            delete m_datatypesTreeDock;
            m_datatypesTreeDock = 0;
            delete m_projectTree;
            m_projectTree = 0;
        }

        setupGuiForNewSolution();

        Project* project = new Project(nprjdlg->getProjectName().toUpper(), m_projectTree, m_datatypesTree, nprjdlg->enableOOPFeatures());
        project->setEngine(nprjdlg->getDatabaseEngine());
        project->createMajorVersion();
        Workspace::getInstance()->addProjectToSolution(Workspace::getInstance()->currentSolution(), project);

        //
        if(nprjdlg->inheritDefaultDatatypes())
        {
            QVector<UserDataType*> dts = Workspace::getInstance()->loadDefaultDatatypesIntoCurrentSolution();

            for(int i=0; i<dts.size(); i++)
            {
                createDataTypeTreeEntry(dts.at(i));
            }

            m_datatypesTree->scrollToItem(Workspace::getInstance()->workingVersion()->getDtsItem());
            m_datatypesTree->expandAll();
        }

        // expand the tree
        m_projectTree->expandAll();

        // show the project properties window
        ProjectDetailsForm* prjDetailsForm = new ProjectDetailsForm(this);
        prjDetailsForm->setProject(project);
        setCentralWidget(prjDetailsForm);

        setWindowTitle(tr("DDM - [") + Workspace::getInstance()->currentSolution()->name() + tr("]"));

        connectActionsFromTablePopupMenu();
        if(!Workspace::getInstance()->currentProjectIsOop())
        {
            Workspace::getInstance()->workingVersion()->getTablesItem()->setText(0, tr("Tables"));
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
        if(item == Workspace::getInstance()->workingVersion()->getDtsItem())
        {// we have clicked on the Data Types item
            DataTypesListForm* dtLst = new DataTypesListForm(this);
            dtLst->feedInDataTypes(Workspace::getInstance()->workingVersion()->getDataTypes());
            setCentralWidget(dtLst);
        }
        else
        if(item->parent() && item->parent() == Workspace::getInstance()->workingVersion()->getDtsItem())
        {   // the user clicked on a Data Type item. The UserDataType class is something that can be put in a user role TAG
            QVariant qv = item->data(0, Qt::UserRole);
            UserDataType* udt = static_cast<UserDataType*>(qv.data());
            if(Workspace::getInstance()->workingVersion()->hasDataType(udt->getName()))
            {
                udt = Workspace::getInstance()->workingVersion()->getDataType(udt->getName());
            }
            NewDataTypeForm* frm = new NewDataTypeForm(Workspace::getInstance()->currentProjectsEngine(), this);
            frm->focusOnName();
            frm->setMainWindow(this);
            frm->setDataType(udt);
            setCentralWidget(frm);
        }
    }
}

void MainWindow::showTable(const QString &tabName, bool focus)
{
    Table* table =  Workspace::getInstance()->workingVersion()->getTable(tabName);
    if(table == 0)  // shouldn't be ...
    {
        return;
    }

    m_newTableForm = new NewTableForm(Workspace::getInstance()->currentProjectsEngine(), Workspace::getInstance()->currentProject(), this, false);
    m_newTableForm->setTable(table);
    m_newTableForm->focusOnName();

    setCentralWidget(m_newTableForm);

    if(focus) m_projectTree->setCurrentItem(table->getLocation());
}

void MainWindow::showTableInstance(const QString &tabName, bool focus)
{
    TableInstance* table =  Workspace::getInstance()->workingVersion()->getTableInstance(tabName);
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
    UserDataType* dt = Workspace::getInstance()->workingVersion()->getDataType(name);
    if(dt == 0)  // shouldn't be ...
    {
        return;
    }

    NewDataTypeForm* frm = new NewDataTypeForm(Workspace::getInstance()->currentProjectsEngine(), this);
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
        if(current == Workspace::getInstance()->workingVersion()->getTablesItem())
        {// we have clicked on the Tables item (i.e.t. the list of tables)
            TablesListForm* tblLst = new TablesListForm(this);
            tblLst->populateTables(Workspace::getInstance()->workingVersion()->getTables());
            tblLst->setOop(Workspace::getInstance()->currentProjectIsOop());
            setCentralWidget(tblLst);
        }
        else
        if(current == Workspace::getInstance()->workingVersion()->getTableInstancesItem())
        {// we have clicked on the Table instances item (i.e.t. the list of table instances)
            TableInstancesListForm* tblLst = new TableInstancesListForm(this);
            tblLst->populateTableInstances(Workspace::getInstance()->workingVersion()->getTableInstances());
            setCentralWidget(tblLst);
        }
        else
            if(current == Workspace::getInstance()->currentProject()->getLocation())
        {
            ProjectDetailsForm* prjDetailsForm = new ProjectDetailsForm(this);
            prjDetailsForm->setProject(Workspace::getInstance()->currentProject());
            setCentralWidget(prjDetailsForm);
        }
        else
        if(current == Workspace::getInstance()->workingVersion()->getFinalSqlItem())
        {
            SqlForm* frm = new SqlForm(Workspace::getInstance()->currentProjectsEngine(), this);
            frm->setSqlSource(0);
            frm->presentSql(Workspace::getInstance()->currentProject());
            setCentralWidget(frm);
        }
        else
        {
            if(current->parent() && current->parent() == Workspace::getInstance()->workingVersion()->getTablesItem())
            {
                // the user clicked on a table, the name of the table is a tag
                QVariant qv = current->data(0, Qt::UserRole);
                QString tabName = qv.toString();
                showTable(tabName, false);
            }
            else
            if(current->parent() && current->parent() == Workspace::getInstance()->workingVersion()->getDiagramsItem())
            {
                // the user clicked on a diagram
                QVariant qv = current->data(0, Qt::UserRole);
                QString diagramName = qv.toString();
                Diagram* dgram = Workspace::getInstance()->workingVersion()->getDiagram(diagramName);
                if(dgram == 0)
                {
                    return;
                }
                DiagramForm* df = new DiagramForm(Workspace::getInstance()->workingVersion(), dgram, this);
                dgram->setForm(df);
                setCentralWidget(dgram->getDiagramForm());
                df->paintDiagram();
            }
            else
            if(current->parent() && current->parent() == Workspace::getInstance()->workingVersion()->getTableInstancesItem())
            {
                // user clicked on a table instance
                TableInstanceForm* frm = new TableInstanceForm(this);
                QVariant qv = current->data(0, Qt::UserRole);
                QString instanceName = qv.toString();

                frm->setTableInstance(Workspace::getInstance()->workingVersion()->getTableInstance(instanceName));
                frm->createTableWithValues();
                setCentralWidget(frm);

            }
            else
            if(current->parent() && current->parent() == Workspace::getInstance()->workingVersion()->getFinalSqlItem())
            {
                // user clicked on a SQL item
                SqlForm* frm = new SqlForm(Workspace::getInstance()->currentProjectsEngine(), this);
                QVariant qv = current->data(0, Qt::UserRole);
                QString name = qv.toString();
                SqlSourceEntity* ent = NULL;

                if(Workspace::getInstance()->currentProjectIsOop())
                {
                    ent = Workspace::getInstance()->workingVersion()->getTableInstance(name);
                }
                else
                {
                    ent = Workspace::getInstance()->workingVersion()->getTable(name);
                }

                frm->setSqlSource(ent);
                frm->presentSql(Workspace::getInstance()->currentProject(), ent);
                setCentralWidget(frm);

            }
            else    // user possibly clicked on a table which had a parent a table ...
            {   // TODO: Code duplication with the "table" stuff above
                QVariant qv = current->data(0, Qt::UserRole);
                QString tabName = qv.toString();
                Table* table =  Workspace::getInstance()->workingVersion()->getTable(tabName);
                if(table == 0)  // shouldn't be ...
                {
                    return;
                }
                m_newTableForm = new NewTableForm(Workspace::getInstance()->currentProjectsEngine(), Workspace::getInstance()->currentProject(), this, false);
                m_newTableForm->setTable(table);
                m_newTableForm->focusOnName();

                setCentralWidget(m_newTableForm);

            }
        }
    }
}

void MainWindow::onNewTable()
{
    NewTableForm* frm = new NewTableForm(Workspace::getInstance()->currentProjectsEngine(), Workspace::getInstance()->currentProject(), this, true);
    frm->focusOnName();

    m_projectTree->setCurrentItem(0);
    setCentralWidget(frm);
}


void MainWindow::onNewDataType()
{
    NewDataTypeForm* frm = new NewDataTypeForm(Workspace::getInstance()->currentProjectsEngine(), this);
    frm->focusOnName();
    frm->setMainWindow(this);
    m_projectTree->setCurrentItem(0);
    setCentralWidget(frm);

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
    // create the tree entry
    ContextMenuEnabledTreeWidgetItem* newTblsItem = new ContextMenuEnabledTreeWidgetItem(Workspace::getInstance()->workingVersion()->getTablesItem(), QStringList(tbl->getName())) ;

    QVariant var(tbl->getName());
    newTblsItem->setData(0, Qt::UserRole, var);
    newTblsItem->setPopupMenu(ContextMenuCollection::getInstance()->getTablePopupMenu());
    // set the icon, add to the tree
    newTblsItem->setIcon(0, IconFactory::getTablesIcon());
    m_projectTree->addTopLevelItem(newTblsItem);

    // add to the project itself
    Workspace::getInstance()->workingVersion()->addTable(tbl);

    // set the link to the tree
    tbl->setLocation(newTblsItem);

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

    if(! Workspace::getInstance()->currentProjectsEngine()->getDTSupplier()->isValid(udt))
    {
        QMessageBox::critical (this, tr("Error"), tr("This datatype requires a length"), QMessageBox::Ok);
        return false;
    }

    UserDataType* other = Workspace::getInstance()->workingVersion()->getDataType(name);

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
        Workspace::getInstance()->workingVersion()->addNewDataType(udt);

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
    if(!Workspace::getInstance()->hasCurrentSolution())
    {
        return;
    }

    QString fileName = "";
    if( (!Workspace::getInstance()->currentSolutionWasSavedAlready() && !saveAs) || saveAs )
    {
        fileName = QFileDialog::getSaveFileName(this,  tr("Save solution"), "", tr("DDM solution files (*.dmx)"));
        if(fileName.length() == 0)
        {
            return;
        }
    }

    Workspace::getInstance()->saveCurrentSolution(fileName);
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

    if(!Workspace::getInstance()->loadSolution(fileName))
    {
        QMessageBox::critical (this, tr("Error"), tr("Cannot load the solution."), QMessageBox::Ok);
        return;
    }

    onCloseSolution();

    setupGuiForNewSolution();

    populateTreeWithSolution(Workspace::getInstance()->currentSolution());

    if(!Workspace::getInstance()->currentProjectIsOop())
    {
        Workspace::getInstance()->workingVersion()->getTablesItem()->setText(0, tr("Tables"));
    }

    m_projectTree->expandAll();

    ProjectDetailsForm* prjDetailsForm = new ProjectDetailsForm(this);
    prjDetailsForm->setProject(Workspace::getInstance()->currentSolution()->currentProject());
    setCentralWidget(prjDetailsForm);

    setWindowTitle(tr("DDM - [") + Workspace::getInstance()->currentSolution()->name() + tr("]"));

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
    if(Workspace::getInstance()->currentProjectIsOop())
    {
        m_ui->action_NewTableInstance->setEnabled(true);
    }
    else
    {
        m_ui->action_NewTableInstance->setVisible(false);

        m_ui->action_NewTable->setText(tr("New Table"));
        m_ui->action_NewTable->setToolTip(tr("Create a new Table"));

        ContextMenuEnabledTreeWidgetItem* item = Workspace::getInstance()->workingVersion()->getTableInstancesItem();
        item->setHidden(true);

    }
}

void MainWindow::connectActionsFromTablePopupMenu()
{
    QObject::connect(ContextMenuCollection::getInstance()->getAction_RemoveTable(), SIGNAL(activated()), this, SLOT(onDeleteTableFromPopup()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_TableAddColumn(), SIGNAL(activated()), this, SLOT(onTableAddColumnFromPopup()));
    if(Workspace::getInstance()->currentProjectIsOop())
    {
        // the table popup
        QObject::connect(ContextMenuCollection::getInstance()->getAction_SpecializeTable(), SIGNAL(activated()), this, SLOT(onSpecializeTableFromPopup()));
        QObject::connect(ContextMenuCollection::getInstance()->getAction_InstantiateTable(), SIGNAL(activated()), this, SLOT(onInstantiateTableFromPopup()));

        // now the table instance popup
        QObject::connect(ContextMenuCollection::getInstance()->getAction_DeleteTableInstance(), SIGNAL(activated()), this, SLOT(onDeleteInstanceFromPopup()));
        QObject::connect(ContextMenuCollection::getInstance()->getAction_RenameTableInstance(), SIGNAL(activated()), this, SLOT(onRenameInstanceFromPopup()));
    }
    else
    {
        ContextMenuCollection::getInstance()->getAction_SpecializeTable()->setVisible(false);
        ContextMenuCollection::getInstance()->getAction_InstantiateTable()->setVisible(false);
    }
    QObject::connect(ContextMenuCollection::getInstance()->getAction_DuplicateTable(), SIGNAL(activated()), this, SLOT(onDuplicateTableFromPopup()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_DeleteDataType(), SIGNAL(activated()), this, SLOT(onDeleteDatatypeFromPopup()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_DuplicateDataType(), SIGNAL(activated()), this, SLOT(onDuplicateDatatypeFromPopup()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_DeleteDiagram(), SIGNAL(activated()), this, SLOT(onDeleteDiagramFromPopup()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_RenameDiagram(), SIGNAL(activated()), this, SLOT(onRenameDiagramFromPopup()));
}

void MainWindow::onAbout()
{
    AboutBoxDialog* about = new AboutBoxDialog(this);
    about->setModal(true);
    about->exec();
}

void MainWindow::onNewDiagram()
{
    Diagram* dgram = new Diagram(Workspace::getInstance()->workingVersion());
    DiagramForm* df = new DiagramForm(Workspace::getInstance()->workingVersion(), dgram, this);
    Workspace::getInstance()->workingVersion()->addDiagram(dgram);
    setCentralWidget(df);
    dgram->setForm(df);
    onSaveDiagram(dgram);
}

bool MainWindow::onSaveDiagram(Diagram* dgram)
{
    if(!dgram->isSaved())
    {
        ContextMenuEnabledTreeWidgetItem* newDgramItem = new ContextMenuEnabledTreeWidgetItem(Workspace::getInstance()->workingVersion()->getDiagramsItem(), QStringList(dgram->getName())) ;
        QVariant var(dgram->getName());
        newDgramItem->setData(0, Qt::UserRole, var);
        newDgramItem->setIcon(0, IconFactory::getDiagramIcon());
        newDgramItem->setPopupMenu(ContextMenuCollection::getInstance()->getDiagramPopupMenu());
        dgram->setLocation(newDgramItem);
        dgram->setSaved(true);
        m_projectTree->addTopLevelItem(newDgramItem);
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
    Table* table =  Workspace::getInstance()->workingVersion()->getTable(tabName);
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
    Table* table =  Workspace::getInstance()->workingVersion()->getTable(tabName);
    if(table == 0)  // shouldn't be ...
    {
        return;
    }

    Table* tbl = new Table(Workspace::getInstance()->workingVersion());
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
    Workspace::getInstance()->workingVersion()->addTable(tbl);

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
        Table* table =  Workspace::getInstance()->workingVersion()->getTable(tabName);
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
        TableInstance* table =  Workspace::getInstance()->workingVersion()->getTableInstance(tabName);
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
        Diagram* dgr =  Workspace::getInstance()->workingVersion()->getDiagram(dgrName);
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
    if(tab)
    {
        Workspace::getInstance()->workingVersion()->deleteTable(tab);
    }
}

void MainWindow::onDuplicateTableFromPopup()
{
    Table* tab = getRightclickedTable();
    if(tab)
    {
        Table* dupped = Workspace::getInstance()->workingVersion()->duplicateTable(tab);
        onSaveNewTable(dupped);
        if(tab->getParent())
        {
            QTreeWidgetItem* p = dupped->getLocation();
            p->parent()->removeChild(p);
            tab->getParent()->getLocation()->addChild(p);
        }
        // add the SQL item but only if it's not an oop project
        if(!Workspace::getInstance()->currentProjectIsOop())
        {
            ContextMenuEnabledTreeWidgetItem* sqlItm = new ContextMenuEnabledTreeWidgetItem(Workspace::getInstance()->workingVersion()->getFinalSqlItem(), QStringList(dupped->getName() + ".sql"));
            sqlItm->setIcon(0, IconFactory::getTablesIcon());
            sqlItm->setData(0, Qt::UserRole, dupped->getName());

        }
    }
}

void MainWindow::onNewTableInstance()
{
    CreateTableInstancesDialog* newDialog = new CreateTableInstancesDialog(this);
    for(int i=0; i<Workspace::getInstance()->workingVersion()->getTables().size(); i++)
    {
        newDialog->addTable(Workspace::getInstance()->workingVersion()->getTables()[i]->getName());
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

    if(Workspace::getInstance()->currentSolution() && Workspace::getInstance()->currentProject() && Workspace::getInstance()->workingVersion())
    {
        for(int i=0; i<Workspace::getInstance()->workingVersion()->getTables().size(); i++)
        {
            QAction * actionToAdd = new QAction(this);
            actionToAdd->setText(Workspace::getInstance()->workingVersion()->getTables()[i]->getName());
            QIcon icon(IconFactory::getTablesIcon());
            actionToAdd->setData(QVariant(Workspace::getInstance()->workingVersion()->getTables()[i]->getName()));
            actionToAdd->setIcon(icon);
            m_createTableInstancesPopup->addAction(actionToAdd);
            QObject::connect(actionToAdd, SIGNAL(activated()),
                             new DynamicActionHandlerforMainWindow(Workspace::getInstance()->workingVersion()->getTables()[i]->getName(), this), SLOT(called()));
        }
    }
}

ContextMenuEnabledTreeWidgetItem* MainWindow::instantiateTable(const QString& tabName, bool ref, Table* referencingTable, TableInstance* becauseOfThis)
{
    Version* cVersion = Workspace::getInstance()->workingVersion();
    TableInstance* tinst = cVersion->instantiateTable(cVersion->getTable(tabName), ref);
    ContextMenuEnabledTreeWidgetItem* itm = new ContextMenuEnabledTreeWidgetItem(cVersion->getTableInstancesItem(), QStringList(tinst->getName()));
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

    ContextMenuEnabledTreeWidgetItem* sqlItm = new ContextMenuEnabledTreeWidgetItem(cVersion->getFinalSqlItem(), QStringList(tinst->getName()));
    sqlItm->setIcon(0, IconFactory::getTabinstIcon());
    sqlItm->setData(0, Qt::UserRole, a);
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
            scw->updateSql(Workspace::getInstance()->currentProject());
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
        Workspace::getInstance()->workingVersion()->deleteTableInstance(tinst);

        // Now delete the tree entries and the SQL tree entry
        for(int i=0; i<Workspace::getInstance()->workingVersion()->getTableInstances().size(); i++)
        {
            if(Workspace::getInstance()->workingVersion()->getTableInstances().at(i)->sentenced())
            {
                QString name = Workspace::getInstance()->workingVersion()->getTableInstances().at(i)->getName();

                ContextMenuEnabledTreeWidgetItem* tabInstItem = Workspace::getInstance()->workingVersion()->getTableInstancesItem();
                for(int j=0; j<tabInstItem->childCount(); j++)
                {
                    QVariant a = tabInstItem->child(j)->data(0, Qt::UserRole);
                    if(a.toString() == name)
                    {
                        delete tabInstItem->child(j);
                    }
                }

                ContextMenuEnabledTreeWidgetItem* sqlItem = Workspace::getInstance()->workingVersion()->getFinalSqlItem();
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
        Workspace::getInstance()->workingVersion()->purgeSentencedTableInstances();

        // and show a table instances list form
        TableInstancesListForm* tblLst = new TableInstancesListForm(this);
        tblLst->populateTableInstances(Workspace::getInstance()->workingVersion()->getTableInstances());
        setCentralWidget(tblLst);
    }
}

void MainWindow::onCloseSolution()
{
    if(!Workspace::getInstance()->hasCurrentSolution())
    {
        return;
    }

    Workspace::getInstance()->onCloseSolution();

    delete m_projectTreeDock;
    delete m_datatypesTreeDock;

    m_projectTreeDock = 0;
    m_datatypesTreeDock = 0;
    m_projectTree = 0;

    m_btndlg = new MainWindowButtonDialog();
    m_btndlg->setMainWindow(this);
    setCentralWidget(m_btndlg);
    setWindowTitle(tr("DDM - [No Solution]"));

    resize(800, 600);
    showNormal();
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
            const QVector<Table*>& allTables = Workspace::getInstance()->workingVersion()->getTables();
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

            Workspace::getInstance()->workingVersion()->deleteDataType(dtName);
            delete itm;
        }
    }
}

void MainWindow::onDuplicateDatatypeFromPopup()
{
    UserDataType* udt = getRightclickedDatatype();
    if(udt)
    {
        UserDataType* dup = Workspace::getInstance()->workingVersion()->duplicateDataType(udt->getName());

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
        Workspace::getInstance()->workingVersion()->deleteDiagram(dgr->getName());
    }
}

void MainWindow::projectTreeItemClicked ( QTreeWidgetItem * item, int column )
{
    currentProjectTreeItemChanged(item, 0);
}


void MainWindow::dtTreeItemClicked ( QTreeWidgetItem * item, int column )
{
    onDTTreeClicked();
}
