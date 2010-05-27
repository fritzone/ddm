#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui/QTreeWidget>


class Project;
class MainWindowButtonDialog;
class DataTypesListForm;
class NewDataTypeForm;
class AbstractDTSupplier;
class UserDataType;
class Table;

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    /**
     * Getting here from the NewDataTypeForm, onSave
     */
    bool onSaveNewDataType(const QString& name, const QString& type,
                             const QString& sqlType, const QString& size,
                             const QString& defaultValue, const QString& cp,
                             const QStringList& mvs, const QString& desc,
                             bool unsi, bool canBeNull, bool autoInc, UserDataType* pudt);

    Project* getWorkingProject();

    /**
     * This methid gets called when a new table is saved
     */
    bool onSaveNewTable(Table* tbl);

    /**
     * This method gets called when a table is updated. The only thing this must do
     * for now is to update the tree entry in the project tree and the associated tree widget
     */
    bool onUpdateTable(Table* tbl);


public slots:

    // called when the user wants to create a new project
    void onNewProject();

    // called when the user clicked somewhere on the project tree
    void onProjectTreeClicked();

    // called when the user activated on the new data type action
    void onNewDataType();

    // called when the user activated a new table action
    void onNewTable();

    void onSaveProject();

private:
    Ui::MainWindow *ui;

    // the dock window in which the project tree is to be found
    QDockWidget* dock;

    // the tree which contains everything related to the current project
    QTreeWidget* projectTree;

    // the initial three button dialog
    MainWindowButtonDialog* btndlg;

    // true if we are working on a project
    bool weHaveProject;

private:

    // to create the other dialogs needed in the app
    void createOtherDialogs();

private:
    // the vector holding the projects
    QVector<Project*> projects;

};

#endif // MAINWINDOW_H
