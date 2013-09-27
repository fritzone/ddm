#ifndef GUI_MAINWINDOWSLOTHOUSE_H
#define GUI_MAINWINDOWSLOTHOUSE_H

#include <QObject>

class MainWindow;
class DeleteHelper;

class MainWindowSlotHouse : public QObject
{
    Q_OBJECT
public:

    static MainWindowSlotHouse* instance();

    ~MainWindowSlotHouse();

    void connectSignalsToSlots();

public slots:
    void onNewStringType();
    void onNewNumericType();
    void onNewBoolType();
    void onNewDateTimeType();
    void onNewBlobType();
    void onNewMiscType();
    void onNewSpatialType();

    void onNewProcedure();  // defaults to guided from now on: v01.k
    void onNewSqlProcedure();
    void onNewFunction();   // defaults to guided from now on: v01.k
    void onNewSqlFunction();

    void onNewProcedureFromPopup();
    void onNewFunctionFromPopup();
    void onNewTableFromPopup();
    void onNewDiagramFromPopup();

    void onDeploy();        // deploy the current working version
    void onDeployVersion(); // deploy a rightclicked version

    void onUnlockSomething();
    void onRelockSomething();

    void onDeleteFunction();
    void onDeleteTrigger();
    void onDeleteViewFromPopup();
    void onDeleteProcedure();
    void onDeleteDiagramFromPopup();


private:

    explicit MainWindowSlotHouse(QObject *parent);


private:

    MainWindow* m_mainWindow;
    DeleteHelper* m_dh;

    static MainWindowSlotHouse* m_instance;
};

#endif // GUI_MAINWINDOWSLOTHOUSE_H
