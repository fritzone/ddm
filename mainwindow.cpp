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

#include <QtGui>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), dock(0), projectTree(0),
    btndlg(0), weHaveProject(false), m_currentSolution(0), frm(0), m_createTableInstancesPopup(0)
{

//    QApplication::setStyle(new QCleanlooksStyle);

    ui->setupUi(this);

    createOtherDialogs();

    btndlg = new MainWindowButtonDialog();
    btndlg->setMainWindow(this);

    setCentralWidget(btndlg);

    setWindowTitle("DBM - [No Solution]");
    resize(800, 600);

    m_createTableInstancesPopup = new QMenu();
    m_createTableInstancesPopup->clear();

    ui->action_NewTableInstance->setMenu(m_createTableInstancesPopup);

    Configuration::instance();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createOtherDialogs()
{

}

ContextMenuEnabledTreeWidget* MainWindow::setupGuiForNewSolution()
{
    // create the dock window
    dock = new QDockWidget(tr("Solution - ") + m_currentSolution->name(), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dock->setFeatures(QDockWidget::AllDockWidgetFeatures);
    dock->setFloating(false);
    dock->setMinimumSize(300, 340);
    dock->setMaximumSize(360, 840);
    dock->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

    dock->resize(301,341);

    dockdt = new QDockWidget(tr("DataTypes") , this);
    dockdt->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dockdt->setFeatures(QDockWidget::AllDockWidgetFeatures);
    dockdt->setFloating(false);
    dockdt->setMinimumSize(300, 340);
    dockdt->setMaximumSize(400, 840);

    // set up the tree
    projectTree = new ContextMenuEnabledTreeWidget();
    projectTree->setAllColumnsShowFocus(true);
    projectTree->setSelectionMode(QAbstractItemView::SingleSelection);
    projectTree->setSelectionBehavior(QAbstractItemView::SelectItems);

    ContextMenuHandler* contextMenuHandler = new ContextMenuHandler();

    projectTree->setItemDelegate(new ContextMenuDelegate(contextMenuHandler,projectTree));
    projectTree->setColumnCount(1);
    projectTree->setHeaderHidden(true);
    QObject::connect(projectTree, SIGNAL (currentItemChanged ( QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(currentProjectTreeItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));

    dataypesTree = new ContextMenuEnabledTreeWidget();
    dataypesTree ->setColumnCount(2);
    QTreeWidgetItem *hdr = dataypesTree->headerItem();
    hdr->setText(0, tr("Type"));
    hdr->setText(1, tr("SQL"));
    dataypesTree->header()->setDefaultSectionSize(200);

    dataypesTree ->setHeaderHidden(false);
    QObject::connect(dataypesTree, SIGNAL(itemSelectionChanged()), this, SLOT(onDTTreeClicked()));

    dockdt->setWidget(dataypesTree);
    dock->setWidget(projectTree);
    addDockWidget(Qt::LeftDockWidgetArea, dock);
    addDockWidget(Qt::LeftDockWidgetArea, dockdt);

    // set a normal size
    showMaximized();

    return projectTree;
}

void MainWindow::onNewSolution()
{
    NewProjectDialog* nprjdlg = new NewProjectDialog();
    nprjdlg->setModal(true);
    nprjdlg->focusOnEditField();
    nprjdlg->exec();

    if(nprjdlg->getProjectName().length() > 0 && nprjdlg->getSolutionName().length() > 0)
    {
        if(!m_currentSolution)
        {
            m_currentSolution = new Solution(nprjdlg->getSolutionName());
            m_solutions.append(m_currentSolution);
        }
        else
        {
            m_solutions.clear();
            m_currentSolution = new Solution(nprjdlg->getSolutionName());
            m_solutions.append(m_currentSolution);
        }

        if(projectTree)
        {
            delete dock;
        }

        projectTree = setupGuiForNewSolution();

        Project* project = new Project(nprjdlg->getProjectName().toUpper(), projectTree, dataypesTree, nprjdlg->enableOOPFeatures());
        project->setEngine(nprjdlg->getDatabaseEngine());
        project->createMajorVersion();

        m_currentSolution->addProject(project);

        // expand the tree
        projectTree->expandAll();
        weHaveProject = true;

        // show the project properties window
        ProjectDetailsForm* prjDetailsForm = new ProjectDetailsForm(this);
        prjDetailsForm->setProject(project);
        setCentralWidget(prjDetailsForm);

        setWindowTitle("DDM - [" + m_currentSolution->name() + "]");

        connectActionsFromTablePopupMenu();
        if(!currentSolution()->currentProject()->oopProject())
        {
            currentSolution()->currentProject()->getWorkingVersion()->getTablesItem()->setText(0, tr("Tables"));
            //delete currentSolution()->currentProject()->getWorkingVersion()->getTableInstancesItem();
        }

        enableActions();
    }
}

void MainWindow::onDTTreeClicked()
{
    QList<QTreeWidgetItem*> selectedItems = dataypesTree->selectedItems();
    if(selectedItems.length() == 1)
    {
        QTreeWidgetItem* item = selectedItems[0];
        if(item == getWorkingProject()->getWorkingVersion()->getDtsItem())
        {// we have clicked on the Data Types item
            DataTypesListForm* dtLst = new DataTypesListForm(this);
            dtLst->feedInDataTypes(getWorkingProject()->getWorkingVersion()->getDataTypes());
            setCentralWidget(dtLst);
        }
        else
        if(item->parent() && item->parent() == getWorkingProject()->getWorkingVersion()->getDtsItem())
        {   // the user clicked on a Data Type item. The UserDataType class is something that can be put in a user role TAG
            QVariant qv = item->data(0, Qt::UserRole);
            UserDataType* udt = static_cast<UserDataType*>(qv.data());
            if(getWorkingProject()->getWorkingVersion()->hasDataType(udt->getName()))
            {
                udt = getWorkingProject()->getWorkingVersion()->getDataType(udt->getName());
            }
            NewDataTypeForm* frm = new NewDataTypeForm(getWorkingProject()->getEngine(), this);
            frm->focusOnName();
            frm->setMainWindow(this);
            frm->setDataType(udt);
            setCentralWidget(frm);
        }
    }
}

void MainWindow::showTable(const QString &tabName, bool focus)
{
    Table* table =  getWorkingProject()->getWorkingVersion()->getTable(tabName);
    if(table == 0)  // shouldn't be ...
    {
        return;
    }

    frm = new NewTableForm(getWorkingProject()->getEngine(), getWorkingProject(), this, false);
    frm->setTable(table);
    frm->focusOnName();

    setCentralWidget(frm);

    if(focus) projectTree->setCurrentItem(table->getLocation());
}

void MainWindow::showTableInstance(const QString &tabName, bool focus)
{
    TableInstance* table =  getWorkingProject()->getWorkingVersion()->getTableInstance(tabName);
    if(table == 0)  // shouldn't be ...
    {
        return;
    }

    TableInstanceForm* frmTinst = new TableInstanceForm(this);

    frmTinst->setTableInstance(table);
    frmTinst->createTableWithValues();
    setCentralWidget(frmTinst);

    if(focus) projectTree->setCurrentItem(table->getLocation());
}


void MainWindow::currentProjectTreeItemChanged(QTreeWidgetItem * current, QTreeWidgetItem*)
{
    if(current)
    {
        if(current == getWorkingProject()->getWorkingVersion()->getTablesItem())
        {// we have clicked on the Tables item (i.e.t. the list of tables)
            TablesListForm* tblLst = new TablesListForm(this);
            tblLst->populateTables(getWorkingProject()->getWorkingVersion()->getTables());
            setCentralWidget(tblLst);
        }
        else
        if(current == getWorkingProject()->getWorkingVersion()->getTableInstancesItem())
        {// we have clicked on the Table instances item (i.e.t. the list of table instances)
            TableInstancesListForm* tblLst = new TableInstancesListForm(this);
            tblLst->populateTableInstances(getWorkingProject()->getWorkingVersion()->getTableInstances());
            setCentralWidget(tblLst);
        }
        else
        if(current == getWorkingProject()->getLocation())
        {
            ProjectDetailsForm* prjDetailsForm = new ProjectDetailsForm(this);
            prjDetailsForm->setProject(m_currentSolution->currentProject());
            setCentralWidget(prjDetailsForm);
        }
        else
        if(current == getWorkingProject()->getWorkingVersion()->getFinalSqlItem())
        {
            SqlForm* frm = new SqlForm(getWorkingProject()->getEngine(), this);
            frm->setSqlSource(0);
            frm->presentSql(getWorkingProject());
            setCentralWidget(frm);
        }
        else
        {
            if(current->parent() && current->parent() == getWorkingProject()->getWorkingVersion()->getTablesItem())
            {
                // the user clicked on a table, the name of the table is a tag
                QVariant qv = current->data(0, Qt::UserRole);
                QString tabName = qv.toString();
                showTable(tabName, false);
            }
            else
            if(current->parent() && current->parent() == getWorkingProject()->getWorkingVersion()->getDiagramsItem())
            {
                // the user clicked on a diagram
                QVariant qv = current->data(0, Qt::UserRole);
                QString diagramName = qv.toString();
                Diagram* dgram = getWorkingProject()->getWorkingVersion()->getDiagram(diagramName);
                if(dgram == 0)
                {
                    return;
                }
                DiagramForm* df = new DiagramForm(getWorkingProject()->getWorkingVersion(), dgram, this);
                dgram->setForm(df);
                setCentralWidget(dgram->getDiagramForm());
                df->paintDiagram();
            }
            else
            if(current->parent() && current->parent() == getWorkingProject()->getWorkingVersion()->getTableInstancesItem())
            {
                // user clicked on a table instance
                TableInstanceForm* frm = new TableInstanceForm(this);
                QVariant qv = current->data(0, Qt::UserRole);
                QString instanceName = qv.toString();

                frm->setTableInstance(getWorkingProject()->getWorkingVersion()->getTableInstance(instanceName));
                frm->createTableWithValues();
                setCentralWidget(frm);

            }
            else
            if(current->parent() && current->parent() == getWorkingProject()->getWorkingVersion()->getFinalSqlItem())
            {
                // user clicked on a SQL item
                SqlForm* frm = new SqlForm(getWorkingProject()->getEngine(), this);
                QVariant qv = current->data(0, Qt::UserRole);
                QString name = qv.toString();
                SqlSourceEntity* ent = NULL;

                if(getWorkingProject()->oopProject())
                {
                    ent = getWorkingProject()->getWorkingVersion()->getTableInstance(name);
                }
                else
                {
                    ent = getWorkingProject()->getWorkingVersion()->getTable(name);
                }

                frm->setSqlSource(ent);
                frm->presentSql(getWorkingProject(), ent);
                setCentralWidget(frm);

            }
            else    // user possibly clicked on a table which had a parent a table ...
            {   // TODO: Code duplication with the "table" stuff above
                QVariant qv = current->data(0, Qt::UserRole);
                QString tabName = qv.toString();
                Table* table =  getWorkingProject()->getWorkingVersion()->getTable(tabName);
                if(table == 0)  // shouldn't be ...
                {
                    return;
                }
                frm = new NewTableForm(getWorkingProject()->getEngine(), getWorkingProject(), this, false);
                frm->setTable(table);
                frm->focusOnName();

                setCentralWidget(frm);

            }
        }
    }
}

void MainWindow::onNewTable()
{
    NewTableForm* frm = new NewTableForm(getWorkingProject()->getEngine(), getWorkingProject(), this, true);
    frm->focusOnName();

    projectTree->setCurrentItem(0);
    setCentralWidget(frm);
}


void MainWindow::onNewDataType()
{
    NewDataTypeForm* frm = new NewDataTypeForm(getWorkingProject()->getEngine(), this);
    frm->focusOnName();
    frm->setMainWindow(this);
    projectTree->setCurrentItem(0);
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
    ContextMenuEnabledTreeWidgetItem* newTblsItem = new ContextMenuEnabledTreeWidgetItem(getWorkingProject()->getWorkingVersion()->getTablesItem(), QStringList(tbl->getName())) ;

    QVariant var(tbl->getName());
    newTblsItem->setData(0, Qt::UserRole, var);
    newTblsItem->setPopupMenu(getWorkingProject()->getWorkingVersion()->getTablePopupMenu());
    // set the icon, add to the tree
    newTblsItem->setIcon(0, IconFactory::getTablesIcon());
    projectTree->addTopLevelItem(newTblsItem);

    // add to the project itself
    getWorkingProject()->getWorkingVersion()->addTable(tbl);

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

    if(! getWorkingProject()->getEngine()->getDTSupplier()->isValid(udt))
    {
        QMessageBox::critical (this, tr("Error"), tr("This datatype requires a length"), QMessageBox::Ok);
        return false;
    }

    UserDataType* other = getWorkingProject()->getWorkingVersion()->getDataType(name);

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
        // create the tree entry
        QStringList itm(name);
        itm << udt->sqlAsString();
        ContextMenuEnabledTreeWidgetItem* newDTItem = new ContextMenuEnabledTreeWidgetItem(getWorkingProject()->getWorkingVersion()->getDtsItem(), itm) ;

        QVariant var;
        var.setValue(*udt);
        newDTItem->setData(0, Qt::UserRole, var);
        // set the icon, add to the tree
        newDTItem->setIcon(0, udt->getIcon());
        dataypesTree->insertTopLevelItem(0,newDTItem);

        // add to the project itself
        getWorkingProject()->getWorkingVersion()->addNewDataType(udt);

        // set the link to the tree
        udt->setLocation(newDTItem);
        dataypesTree->expandItem(newDTItem);
        dataypesTree->scrollToItem(newDTItem);

        return true;

    }

    return false;
}

Project* MainWindow::getWorkingProject()
{
    return m_currentSolution->currentProject();
}

Solution* MainWindow::currentSolution()
{
    return m_currentSolution;
}

void MainWindow::saveProject(bool saveAs)
{
    if(!m_currentSolution)
    {
        return;
    }

    QDomDocument doc("DBM");
    QDomElement root = doc.createElement("Solutions");
    currentSolution()->serialize(doc, root);

    doc.appendChild(root);
    QString xml = doc.toString();

    if((m_currentSolution->savedFile().length() == 0 && !saveAs) || saveAs )
    {
        QString fileName = QFileDialog::getSaveFileName(this,  tr("Save solution"), "", tr("DBM solution files (*.dmx)"));
        if(fileName.length() == 0)
        {
            return;
        }
        m_currentSolution->setFile(fileName);
    }

    QFile f1(m_currentSolution->savedFile());
    if (!f1.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return;
    }
    f1.write(xml.toAscii());
    f1.close();

}

void MainWindow::onSaveProject()
{
    saveProject(false);
}

void MainWindow::populateTreeWithSolution(Solution* sol)
{
    for(int i=0; i<sol->projects().size(); i++)
    {
        sol->projects()[i]->createTreeItem(projectTree, dataypesTree);
        sol->projects()[i]->populateTreeItem();
    }
}

void MainWindow::onOpenProject()
{
    QString fileName = QFileDialog::getOpenFileName(this,  tr("Open solution"), "", tr("DBM solution files (*.dmx)"));
    if(fileName.length() == 0)
    {
        return;
    }

    QDomDocument doc ("DBM");
    QFile file (fileName);
    if (!file.open(QIODevice::ReadOnly)) return;
    if (!doc.setContent(&file))
    {
        file.close();
        return;
    }
    file.close();

    QDomElement docElem = doc.documentElement();
    m_currentSolution = DeserializationFactory::createSolution(doc, docElem.firstChild().toElement());
    m_currentSolution->setFile(fileName);
    setupGuiForNewSolution();

    populateTreeWithSolution(m_currentSolution);

    if(!currentSolution()->currentProject()->oopProject())
    {
        currentSolution()->currentProject()->getWorkingVersion()->getTablesItem()->setText(0, tr("Tables"));
    }

    projectTree->expandAll();

    ProjectDetailsForm* prjDetailsForm = new ProjectDetailsForm(this);
    prjDetailsForm->setProject(m_currentSolution->currentProject());
    setCentralWidget(prjDetailsForm);

    setWindowTitle("DBM - [" + m_currentSolution->name() + "]");

    connectActionsFromTablePopupMenu();

    enableActions();
}

void MainWindow::enableActions()
{
    ui->action_NewDataType->setEnabled(true);
    ui->action_NewTable->setEnabled(true);
    ui->action_NewDiagram->setEnabled(true);
    ui->action_Save->setEnabled(true);
    ui->action_SaveAs->setEnabled(true);
    if(currentSolution()->currentProject()->oopProject())
    {
        ui->action_NewTableInstance->setEnabled(true);
    }
    else
    {
        ui->action_NewTableInstance->setVisible(false);

        ui->action_NewTable->setText(tr("New Table"));
        ui->action_NewTable->setToolTip(tr("Create a new Table"));

        ContextMenuEnabledTreeWidgetItem* item = getWorkingProject()->getWorkingVersion()->getTableInstancesItem();
        item->setHidden(true);

    }
}

void MainWindow::connectActionsFromTablePopupMenu()
{
    QObject::connect(getWorkingProject()->getWorkingVersion()->getAction_RemoveTable(), SIGNAL(activated()), this, SLOT(onDeleteTableFromPopup()));
    QObject::connect(getWorkingProject()->getWorkingVersion()->getAction_TableAddColumn(), SIGNAL(activated()), this, SLOT(onTableAddColumnFromPopup()));
    if(currentSolution()->currentProject()->oopProject())
    {
        // the table popup
        QObject::connect(getWorkingProject()->getWorkingVersion()->getAction_SpecializeTable(), SIGNAL(activated()), this, SLOT(onSpecializeTableFromPopup()));
        QObject::connect(getWorkingProject()->getWorkingVersion()->getAction_InstantiateTable(), SIGNAL(activated()), this, SLOT(onInstantiateTableFromPopup()));

        // now the table instance popup
        QObject::connect(getWorkingProject()->getWorkingVersion()->getAction_DeleteTableInstance(), SIGNAL(activated()), this, SLOT(onDeleteInstanceFromPopup()));
        QObject::connect(getWorkingProject()->getWorkingVersion()->getAction_RenameTableInstance(), SIGNAL(activated()), this, SLOT(onRenameInstanceFromPopup()));
    }
    else
    {
        getWorkingProject()->getWorkingVersion()->getAction_SpecializeTable()->setVisible(false);
        getWorkingProject()->getWorkingVersion()->getAction_InstantiateTable()->setVisible(false);
    }
    QObject::connect(getWorkingProject()->getWorkingVersion()->getAction_DuplicateTable(), SIGNAL(activated()), this, SLOT(onDuplicateTableFromPopup()));
}

void MainWindow::onAbout()
{
    AboutBoxDialog* about = new AboutBoxDialog(this);
    about->setModal(true);
    about->exec();
}

void MainWindow::onNewDiagram()
{
    Diagram* dgram = new Diagram(getWorkingProject()->getWorkingVersion());
    DiagramForm* df = new DiagramForm(getWorkingProject()->getWorkingVersion(), dgram, this);
    setCentralWidget(df);
    dgram->setForm(df);
    onSaveDiagram(dgram);
}

bool MainWindow::onSaveDiagram(Diagram* dgram)
{
    if(!dgram->isSaved())
    {
        ContextMenuEnabledTreeWidgetItem* newDgramItem = new ContextMenuEnabledTreeWidgetItem(m_currentSolution->currentProject()->getWorkingVersion()->getDiagramsItem(), QStringList(dgram->getName())) ;
        QVariant var(dgram->getName());
        newDgramItem->setData(0, Qt::UserRole, var);
        newDgramItem->setIcon(0, IconFactory::getDiagramIcon());
        dgram->setLocation(newDgramItem);
        dgram->setSaved(true);
        projectTree->addTopLevelItem(newDgramItem);
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
    if(projectTree->getLastRightclickedItem() == 0)
    {
        return;
    }

    // TODO: Code duplication with some stuff from below (onDelete...)
    ContextMenuEnabledTreeWidgetItem* item = projectTree->getLastRightclickedItem();
    projectTree->setLastRightclickedItem(0);

    QVariant qv = item->data(0, Qt::UserRole);
    QString tabName = qv.toString();
    Table* table =  getWorkingProject()->getWorkingVersion()->getTable(tabName);
    if(table == 0)  // shouldn't be ...
    {
        return;
    }

    projectTree->setCurrentItem(instantiateTable(table->getName()));
}

void MainWindow::onSpecializeTableFromPopup()
{
    if(projectTree->getLastRightclickedItem() == 0)
    {
        return;
    }

    // TODO: Code duplication with some stuff from below (onDelete...)
    ContextMenuEnabledTreeWidgetItem* item = projectTree->getLastRightclickedItem();
    projectTree->setLastRightclickedItem(0);

    QVariant qv = item->data(0, Qt::UserRole);
    QString tabName = qv.toString();
    Table* table =  getWorkingProject()->getWorkingVersion()->getTable(tabName);
    if(table == 0)  // shouldn't be ...
    {
        return;
    }

    Table* tbl = new Table(getWorkingProject()->getWorkingVersion());
    tbl->setName(table->getName() + "_specialized");
    tbl->setParent(table);
    tbl->setStorageEngine(table->getStorageEngine());

    // TODO: Code duplication from the "Save table"
    ContextMenuEnabledTreeWidgetItem* newTblsItem = new ContextMenuEnabledTreeWidgetItem(item, QStringList(tbl->getName())) ;

    QVariant var(tbl->getName());
    newTblsItem->setData(0, Qt::UserRole, var);
    newTblsItem->setPopupMenu(getWorkingProject()->getWorkingVersion()->getTablePopupMenu());
    // set the icon, add to the tree
    newTblsItem->setIcon(0, IconFactory::getTablesIcon());
    projectTree->insertTopLevelItem(0, newTblsItem);

    // add to the project itself
    getWorkingProject()->getWorkingVersion()->addTable(tbl);

    // set the link to the tree
    tbl->setLocation(newTblsItem);

    // now open the new table for to show this table
    projectTree->setCurrentItem(newTblsItem);
    projectTree->scrollToItem(newTblsItem);
    if(frm != 0)
    {
        frm->selectTab(0);
        frm->focusOnNewColumnName();
    }
}

void MainWindow::onTableAddColumnFromPopup()
{
    if(projectTree->getLastRightclickedItem() != 0)
    {
        projectTree->setCurrentItem(projectTree->getLastRightclickedItem(), 0);  // this is supposed to open the window "frm" too
        if(frm != 0)
        {
            frm->selectTab(0);
            frm->focusOnNewColumnName();
            projectTree->setLastRightclickedItem(0);
        }
    }
}

Table* MainWindow::getRightclickedTable()
{
    if(projectTree->getLastRightclickedItem() != 0)
    {
        ContextMenuEnabledTreeWidgetItem* item = projectTree->getLastRightclickedItem();
        projectTree->setLastRightclickedItem(0);

        QVariant qv = item->data(0, Qt::UserRole);
        QString tabName = qv.toString();
        Table* table =  getWorkingProject()->getWorkingVersion()->getTable(tabName);
        return table;
    }
    return 0;
}

TableInstance* MainWindow::getRightclickedTableInstance()
{
    if(projectTree->getLastRightclickedItem() != 0)
    {
        ContextMenuEnabledTreeWidgetItem* item = projectTree->getLastRightclickedItem();
        projectTree->setLastRightclickedItem(0);

        QVariant qv = item->data(0, Qt::UserRole);
        QString tabName = qv.toString();
        TableInstance* table =  getWorkingProject()->getWorkingVersion()->getTableInstance(tabName);
        return table;
    }
    return 0;
}


void MainWindow::onDeleteTableFromPopup()
{
    Table* tab = getRightclickedTable();
    if(tab)
    {
        getWorkingProject()->getWorkingVersion()->deleteTable(tab);
    }
}

void MainWindow::onDuplicateTableFromPopup()
{
    Table* tab = getRightclickedTable();
    if(tab)
    {
        Table* dupped = getWorkingProject()->getWorkingVersion()->duplicateTable(tab);
        onSaveNewTable(dupped);
        if(tab->getParent())
        {
            QTreeWidgetItem* p = dupped->getLocation();
            p->parent()->removeChild(p);
            tab->getParent()->getLocation()->addChild(p);
        }
        // add the SQL item but only if it's not an oop project
        if(!getWorkingProject()->oopProject())
        {
            ContextMenuEnabledTreeWidgetItem* sqlItm = new ContextMenuEnabledTreeWidgetItem(currentSolution()->currentProject()->getWorkingVersion()->getFinalSqlItem(), QStringList(dupped->getName() + ".sql"));
            sqlItm->setIcon(0, IconFactory::getTablesIcon());
            sqlItm->setData(0, Qt::UserRole, dupped->getName());

        }
    }
}

void MainWindow::onNewTableInstance()
{
    CreateTableInstancesDialog* newDialog = new CreateTableInstancesDialog(this);
    for(int i=0; i<currentSolution()->currentProject()->getWorkingVersion()->getTables().size(); i++)
    {
        newDialog->addTable(currentSolution()->currentProject()->getWorkingVersion()->getTables()[i]->getName());
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

    if(currentSolution() && currentSolution()->currentProject() && currentSolution()->currentProject()->getWorkingVersion())
    {
        for(int i=0; i<currentSolution()->currentProject()->getWorkingVersion()->getTables().size(); i++)
        {
            QAction * actionToAdd = new QAction(this);
            actionToAdd->setText(currentSolution()->currentProject()->getWorkingVersion()->getTables()[i]->getName());
            QIcon icon(IconFactory::getTablesIcon());
            actionToAdd->setData(QVariant(currentSolution()->currentProject()->getWorkingVersion()->getTables()[i]->getName()));
            actionToAdd->setIcon(icon);
            m_createTableInstancesPopup->addAction(actionToAdd);
            QObject::connect(actionToAdd, SIGNAL(activated()),
                             new DynamicActionHandlerforMainWindow(currentSolution()->currentProject()->getWorkingVersion()->getTables()[i]->getName(), this), SLOT(called()));
        }
    }
}

ContextMenuEnabledTreeWidgetItem* MainWindow::instantiateTable(const QString& tabName, bool ref, Table* referencingTable, TableInstance* becauseOfThis)
{
    Version* cVersion = currentSolution()->currentProject()->getWorkingVersion();
    TableInstance* tinst = cVersion->instantiateTable(cVersion->getTable(tabName), ref);
    ContextMenuEnabledTreeWidgetItem* itm = new ContextMenuEnabledTreeWidgetItem(cVersion->getTableInstancesItem(), QStringList(tinst->getName()));
    itm->setPopupMenu(cVersion->getTableInstancePopupMenu());
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
            scw->updateSql(getWorkingProject());
        }
    }
}

void MainWindow::onRenameInstanceFromPopup()
{
    TableInstance* tinst = getRightclickedTableInstance();
    if(tinst)
    {
        SimpleTextInputDialog* dlg = new SimpleTextInputDialog(this, "Enter the new name");
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
                projectTree->setLastRightclickedItem(0);
                return;
            }
        }

        // mark for purge
        getWorkingProject()->getWorkingVersion()->deleteTableInstance(tinst);

        // Now delete the tree entries and the SQL tree entry
        for(int i=0; i<getWorkingProject()->getWorkingVersion()->getTableInstances().size(); i++)
        {
            if(getWorkingProject()->getWorkingVersion()->getTableInstances().at(i)->sentenced())
            {
                QString name = getWorkingProject()->getWorkingVersion()->getTableInstances().at(i)->getName();

                ContextMenuEnabledTreeWidgetItem* tabInstItem = getWorkingProject()->getWorkingVersion()->getTableInstancesItem();
                for(int j=0; j<tabInstItem->childCount(); j++)
                {
                    QVariant a = tabInstItem->child(j)->data(0, Qt::UserRole);
                    if(a.toString() == name)
                    {
                        delete tabInstItem->child(j);
                    }
                }

                ContextMenuEnabledTreeWidgetItem* sqlItem = getWorkingProject()->getWorkingVersion()->getFinalSqlItem();
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
        getWorkingProject()->getWorkingVersion()->purgeSentencedTableInstances();

        // and show a table instances list form
        TableInstancesListForm* tblLst = new TableInstancesListForm(this);
        tblLst->populateTableInstances(getWorkingProject()->getWorkingVersion()->getTableInstances());
        setCentralWidget(tblLst);
    }
}
