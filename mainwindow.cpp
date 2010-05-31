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
#include "TablesListForm.h"
#include "ProjectDetailsForm.h"
#include "Solution.h"
#include "DeserializationFactory.h"

#include <QtGui>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), dock(0), projectTree(0),
    btndlg(0), weHaveProject(false), m_currentSolution(0)
{
    ui->setupUi(this);

    createOtherDialogs();

    btndlg = new MainWindowButtonDialog();
    btndlg->setMainWindow(this);

    setCentralWidget(btndlg);

    //this->ui->mainToolBar->actions().at(3)->setVisible(false);
    //this->ui->mainToolBar->actions().at(4)->setVisible(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createOtherDialogs()
{

}

QTreeWidget* MainWindow::setupGuiForNewSolution()
{
    // create the dock window
    dock = new QDockWidget(tr("Solution - ") + m_currentSolution->name(), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dock->setFeatures(QDockWidget::AllDockWidgetFeatures);
    dock->setFloating(false);
    dock->setMinimumSize(300, 640);

    // set up the tree
    projectTree = new QTreeWidget();
    projectTree->setColumnCount(1);
    projectTree->setHeaderHidden(true);
    QObject::connect(projectTree, SIGNAL(itemSelectionChanged()), this, SLOT(onProjectTreeClicked()));

    dock->setWidget(projectTree);
    addDockWidget(Qt::LeftDockWidgetArea, dock);

    // show all the icons
    ui->mainToolBar->actions().at(3)->setVisible(true);
    ui->mainToolBar->actions().at(4)->setVisible(true);

    // set a normal size
    resize(800, 600);

    return projectTree;
}

void MainWindow::onNewProject()
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

        projectTree = setupGuiForNewSolution();

        Project* project = new Project(nprjdlg->getProjectName().toUpper(), projectTree);
        project->setEngine(nprjdlg->getDatabaseEngine());
        project->createMajorVersion();

        m_currentSolution->addProject(project);

        // expand the tree
        projectTree->expandAll();
        weHaveProject = true;

        // show the project properties window
        ProjectDetailsForm* prjDetailsForm = new ProjectDetailsForm(this);
        setCentralWidget(prjDetailsForm);
    }
}

void MainWindow::onProjectTreeClicked()
{
    QList<QTreeWidgetItem*> selectedItems = projectTree->selectedItems();
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
        if(item == getWorkingProject()->getWorkingVersion()->getTablesItem())
        {// we have clicked on the Tables item
            TablesListForm* tblLst = new TablesListForm(this);
            tblLst->populateTables(getWorkingProject()->getWorkingVersion()->getTables());
            setCentralWidget(tblLst);
        }
        else
        if(item == getWorkingProject()->getWorkingVersion()->getViewsItem())
        {
        }
        else
        if(item == getWorkingProject()->getWorkingVersion()->getVersionItem())
        {
        }
        else// here means the user clicked on something which has a "TAG"
        {
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
            else
            if(item->parent() && item->parent() == getWorkingProject()->getWorkingVersion()->getTablesItem())
            {
                // the user clicked on a table, the name of the table is a tag
                QVariant qv = item->data(0, Qt::UserRole);
                QString tabName = qv.toString();
                Table* table =  getWorkingProject()->getWorkingVersion()->getTable(tabName);
                NewTableForm* frm = new NewTableForm(getWorkingProject()->getEngine(), getWorkingProject(), this);
                frm->setTable(table);
                frm->focusOnName();
                frm->setMainWindow(this);
                projectTree->setCurrentItem(0);
                setCentralWidget(frm);

            }
        }
    }
}

void MainWindow::onNewTable()
{
    NewTableForm* frm = new NewTableForm(getWorkingProject()->getEngine(), getWorkingProject(), this);
    frm->focusOnName();
    frm->setMainWindow(this);
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
    QTreeWidgetItem* newTblsItem = new QTreeWidgetItem(getWorkingProject()->getWorkingVersion()->getTablesItem(), QStringList(tbl->getName())) ;

    QVariant var(tbl->getName());
    newTblsItem->setData(0, Qt::UserRole, var);
    // set the icon, add to the tree
    newTblsItem->setIcon(0, IconFactory::getTablesIcon());
    projectTree->insertTopLevelItem(0, newTblsItem);

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

    // check if this is Saving an existing data type or updating a new one
    if(pudt)    // saving
    {
        *pudt = *udt;
        pudt->getLocation()->setIcon(0, pudt->getIcon());
        pudt->getLocation()->setText(0, name);

        // updating the "data" of the tree item
        QVariant var;
        var.setValue(*udt);
        pudt->getLocation()->setData(0, Qt::UserRole, var);
    }
    else        // new stuff
    {
        // create the tree entry
        QTreeWidgetItem* newDTItem = new QTreeWidgetItem(getWorkingProject()->getWorkingVersion()->getDtsItem(), QStringList(name)) ;

        QVariant var;
        var.setValue(*udt);
        newDTItem->setData(0, Qt::UserRole, var);
        // set the icon, add to the tree
        newDTItem->setIcon(0, udt->getIcon());
        projectTree->insertTopLevelItem(0,newDTItem);

        // add to the project itself
        getWorkingProject()->getWorkingVersion()->addNewDataType(udt);

        // set the link to the tree
        udt->setLocation(newDTItem);

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

void MainWindow::onSaveProject()
{
    if(!m_currentSolution) return;
    QDomDocument doc("DBM");
    QDomElement root = doc.createElement("Solutions");
    currentSolution()->serialize(doc, root);

    doc.appendChild(root);
    QString xml = doc.toString();

    QString fileName = QFileDialog::getSaveFileName(this,  tr("Save solution"), "", tr("DBM solution files (*.dmx)"));
    if(fileName.length() == 0)
    {
        return;
    }

    QFile f1(fileName);
    if (!f1.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return;
    }
    f1.write(xml.toAscii());
    f1.close();
}

void MainWindow::populateTreeWithSolution(Solution* sol)
{
    for(int i=0; i<sol->projects().size(); i++)
    {
        sol->projects()[i]->createTreeItem(projectTree);
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
    setupGuiForNewSolution();

    populateTreeWithSolution(m_currentSolution);

    projectTree->expandAll();

}
