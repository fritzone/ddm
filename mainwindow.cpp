#include "MainWindow.h"
#include "ui_mainwindow.h"
#include "MainWindowButtonDialog.h"
#include "NewProjectDialog.h"
#include "DatatypesListForm.h"
#include "NewDataTypeForm.h"
#include "UserDataType.h"
#include "Project.h"
#include "DatabaseEngine.h"
#include "Version.h"
#include "AbstractDTSupplier.h"

#include <QtGui>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), dock(0), projectTree(0),
    btndlg(0), weHaveProject(false)
{
    ui->setupUi(this);

    createOtherDialogs();

    btndlg = new MainWindowButtonDialog();
    btndlg->setMainWindow(this);

    setCentralWidget(btndlg);

    this->ui->mainToolBar->actions().at(3)->setVisible(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createOtherDialogs()
{

}





void MainWindow::onNewProject()
{
    NewProjectDialog* nprjdlg = new NewProjectDialog();
    nprjdlg->setModal(true);
    nprjdlg->focusOnEditField();
    nprjdlg->exec();

    if(nprjdlg->getProjectName().size() > 0)
    {
        // create the dock window
        dock = new QDockWidget(tr("Project"), this);
        dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
        dock->setFeatures(QDockWidget::AllDockWidgetFeatures);
        dock->setFloating(false);

        // set up the tree
        projectTree = new QTreeWidget();
        projectTree->setColumnCount(1);
        projectTree->setHeaderHidden(true);

        QObject::connect(projectTree, SIGNAL(itemSelectionChanged()), this, SLOT(onProjectTreeClicked()));

        dock->setWidget(projectTree);
        addDockWidget(Qt::LeftDockWidgetArea, dock);

        // we have a new project add the requried tree entries
        // icons


        // the project name will be uppercase
        QString prjName = nprjdlg->getProjectName();
        prjName = prjName.toUpper();


        Project* project = new Project(prjName, projectTree);
        projects.append(project);
        project->setEngine(nprjdlg->getDatabaseEngine());
        project->createMajorVersion();


        // expand the tree
        projectTree->expandAll();
        weHaveProject = true;

    }
}

void MainWindow::onProjectTreeClicked()
{
    QList<QTreeWidgetItem*> selectedItems = projectTree->selectedItems();
    if(selectedItems.length() == 1)
    {
        QTreeWidgetItem* item = selectedItems[0];

        // see if we have clicked on the Data Types item
        if(item == getWorkingProject()->getWorkingVersion()->getDtsItem())
        {
            DataTypesListForm* dtLst = new DataTypesListForm(this);
            dtLst->feedInDataTypes(getWorkingProject()->getWorkingVersion()->getDataTypes());
            setCentralWidget(dtLst);

            this->ui->mainToolBar->actions().at(3)->setVisible(true);
        }
        else
        if(item == getWorkingProject()->getWorkingVersion()->getTablesItem())
        {
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
            {   // the user clicked on a Data Type item
                QVariant qv = item->data(0, Qt::UserRole);
                UserDataType* udt = static_cast<UserDataType*>(qv.data());
                NewDataTypeForm* frm = new NewDataTypeForm(getWorkingProject()->getEngine(),  this);
                frm->focusOnName();
                frm->setMainWindow(this);
                frm->setDataType(udt);
                setCentralWidget(frm);
            }

        }


    }
}

void MainWindow::onNewDataType()
{
    NewDataTypeForm* frm = new NewDataTypeForm(getWorkingProject()->getEngine(),
                                               this);
    frm->focusOnName();
    frm->setMainWindow(this);
    projectTree->setCurrentItem(0);
    setCentralWidget(frm);

}

bool MainWindow::onSaveNewDataType(const QString& name, const QString& type,
                             const QString& sqlType, const QString& size,
                             const QString& defaultValue, const QString& cp,
                             const QStringList& mvs, bool unsi)
{
    if(name.length() == 0 || type.length() == 0 || sqlType.length() == 0)
    {
        QMessageBox::critical (this, tr("Error"),
                               tr("Please specify all the required data"),
                               QMessageBox::Ok);
        return false;
    }

    UserDataType* udt = new UserDataType(name, type, sqlType,
                                         size, defaultValue, cp, mvs, unsi);
    if(! getWorkingProject()->getEngine()->getDTSupplier()->isValid(udt))
    {
        QMessageBox::critical (this, tr("Error"),
                               tr("This datatype requires a length"),
                               QMessageBox::Ok);
        return false;
    }

    QTreeWidgetItem* newDTItem = new QTreeWidgetItem(
            getWorkingProject()->getWorkingVersion()->getDtsItem()
            , QStringList(name)) ;

    QVariant var;
    var.setValue(*udt);
    newDTItem->setData(0, Qt::UserRole, var);
    newDTItem->setIcon(0, udt->getIcon());
    projectTree->insertTopLevelItem(0,newDTItem);
    getWorkingProject()->getWorkingVersion()->addNewDataType(udt);

    return true;
}

Project* MainWindow::getWorkingProject()
{
    if(projects.size() > 0)
    {
        return projects[0];
    }

    return 0;
}
