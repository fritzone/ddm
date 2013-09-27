#include "gui_MainWindowSlotHouse.h"
#include "MainWindow.h"
#include "core_enums.h"
#include "Workspace.h"
#include "gui_DeleteHelper.h"
#include "ContextMenuCollection.h"

MainWindowSlotHouse* MainWindowSlotHouse::m_instance = 0;

MainWindowSlotHouse::MainWindowSlotHouse(QObject *parent) : QObject(0), m_dh(new DeleteHelper)
{
    m_mainWindow = dynamic_cast<MainWindow*>(parent);
}

MainWindowSlotHouse* MainWindowSlotHouse::instance()
{
    if(m_instance == 0)
    {
        static MainWindowSlotHouse inst(MainWindow::instance());
        m_instance = &inst;
    }

    return m_instance;
}

MainWindowSlotHouse::~MainWindowSlotHouse()
{
    delete m_dh;
}

void MainWindowSlotHouse::connectSignalsToSlots()
{
    QObject::connect(ContextMenuCollection::getInstance()->getAction_DeployVersion(), SIGNAL(triggered()),  this, SLOT(onDeployVersion()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_unlock(), SIGNAL(triggered()), this, SLOT(onUnlockSomething()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_relock(), SIGNAL(triggered()), this, SLOT(onRelockSomething()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_AddString(), SIGNAL(triggered()), this, SLOT(onNewStringType()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_AddNumeric(), SIGNAL(triggered()), this, SLOT(onNewNumericType()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_AddBool(), SIGNAL(triggered()), this, SLOT(onNewBoolType()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_AddDateType(), SIGNAL(triggered()), this, SLOT(onNewDateTimeType()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_AddMisc(), SIGNAL(triggered()), this, SLOT(onNewMiscType()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_AddBlob(), SIGNAL(triggered()), this, SLOT(onNewBlobType()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_AddSpatial(), SIGNAL(triggered()), this, SLOT(onNewSpatialType()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_CreateGuidedProcedure(), SIGNAL(triggered()), this, SLOT(onNewProcedure()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_CreateGuidedFunction(), SIGNAL(triggered()), this, SLOT(onNewFunction()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_CreateSqlProcedure(), SIGNAL(triggered()), this, SLOT(onNewSqlProcedure()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_CreateSqlFunction(), SIGNAL(triggered()), this, SLOT(onNewSqlFunction()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_DeleteFunction(), SIGNAL(triggered()),  this, SLOT(onDeleteFunction()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_DeleteTrigger(), SIGNAL(triggered()),  this, SLOT(onDeleteTrigger()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_DeleteView(), SIGNAL(triggered()),  this, SLOT(onDeleteViewFromPopup()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_DeleteProcedure(), SIGNAL(triggered()),  this, SLOT(onDeleteProcedure()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_DeleteDiagram(), SIGNAL(triggered()),  this, SLOT(onDeleteDiagramFromPopup()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_AddProcedure(), SIGNAL(triggered()), this, SLOT(onNewProcedureFromPopup()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_AddFunction(), SIGNAL(triggered()), this, SLOT(onNewFunctionFromPopup()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_AddTable(), SIGNAL(triggered()), this, SLOT(onNewTableFromPopup()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_AddDiagram(), SIGNAL(triggered()), this, SLOT(onNewDiagramFromPopup()));
}

void MainWindowSlotHouse::onNewStringType()
{
    m_mainWindow->showNewDataTypeWindow(DT_STRING,
                           m_mainWindow->getVersionOfLastRightClickedElement()
                           );
}


void MainWindowSlotHouse::onNewNumericType()
{
    m_mainWindow->showNewDataTypeWindow(DT_NUMERIC,
                                        m_mainWindow->getVersionOfLastRightClickedElement());
}

void MainWindowSlotHouse::onNewBoolType()
{
    m_mainWindow->showNewDataTypeWindow(DT_BOOLEAN,
                                        m_mainWindow->getVersionOfLastRightClickedElement());
}

void MainWindowSlotHouse::onNewDateTimeType()
{
    m_mainWindow->showNewDataTypeWindow(DT_DATETIME,
                                        m_mainWindow->getVersionOfLastRightClickedElement());
}

void MainWindowSlotHouse::onNewBlobType()
{
    m_mainWindow->showNewDataTypeWindow(DT_BLOB,
                                        m_mainWindow->getVersionOfLastRightClickedElement());
}

void MainWindowSlotHouse::onNewMiscType()
{
    m_mainWindow->showNewDataTypeWindow(DT_MISC,
                                        m_mainWindow->getVersionOfLastRightClickedElement());
}

void MainWindowSlotHouse::onNewSpatialType()
{
    m_mainWindow->showNewDataTypeWindow(DT_SPATIAL,
                                        m_mainWindow->getVersionOfLastRightClickedElement());
}

void MainWindowSlotHouse::onNewFunction()
{
    m_mainWindow->createStoredFunction(true, Workspace::getInstance()->workingVersion());
}

void MainWindowSlotHouse::onNewSqlFunction()
{
    m_mainWindow->createStoredFunction(false, Workspace::getInstance()->workingVersion());
}

void MainWindowSlotHouse::onNewProcedure()
{
    m_mainWindow->createStoredProcedureInVersion(true, Workspace::getInstance()->workingVersion());
}

void MainWindowSlotHouse::onNewSqlProcedure()
{
    m_mainWindow->createStoredProcedureInVersion(false, Workspace::getInstance()->workingVersion());
}

void MainWindowSlotHouse::onDeploy()
{
    Workspace::getInstance()->deployVersion(Workspace::getInstance()->workingVersion());
}

void MainWindowSlotHouse::onDeployVersion()
{
    Workspace::getInstance()->deployVersion(m_mainWindow->getRightClickedVersion());
}

void MainWindowSlotHouse::onUnlockSomething()
{
    m_mainWindow->doLockLikeOperation(false);
}

void MainWindowSlotHouse::onRelockSomething()
{
    m_mainWindow->doLockLikeOperation(true);
}

void MainWindowSlotHouse::onDeleteFunction()
{
    m_dh->genericDeleter<Function>();
}

void MainWindowSlotHouse::onDeleteTrigger()
{
    m_dh->genericDeleter<Trigger>();
}

void MainWindowSlotHouse::onDeleteProcedure()
{
     m_dh->genericDeleter<Procedure>();
}

void MainWindowSlotHouse::onDeleteViewFromPopup()
{
     m_dh->genericDeleter<View>();
}

void MainWindowSlotHouse::onDeleteDiagramFromPopup()
{
     m_dh->genericDeleter<Diagram>();
}

void MainWindowSlotHouse::onNewProcedureFromPopup()
{
    m_mainWindow->createStoredProcedureInVersion(true,
                                    m_mainWindow->getLastRightClickedVersion());
}

void MainWindowSlotHouse::onNewFunctionFromPopup()
{
    m_mainWindow->createStoredFunction(true, m_mainWindow->getLastRightClickedVersion());
}

void MainWindowSlotHouse::onNewTableFromPopup()
{
    m_mainWindow->newTableInVersion(m_mainWindow->getLastRightClickedVersion());
}

void MainWindowSlotHouse::onNewDiagramFromPopup()
{
    m_mainWindow->newDiagramInVersion(m_mainWindow->getLastRightClickedVersion());
}
