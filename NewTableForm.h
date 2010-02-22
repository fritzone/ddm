#ifndef NEWTABLEFORM_H
#define NEWTABLEFORM_H

#include <QWidget>
#include <QTreeWidgetItem>

namespace Ui {
    class NewTableForm;
}

class MainWindow;
class DatabaseEngine;
class Project;
class Table;
class Column;

class NewTableForm : public QWidget {
    Q_OBJECT
public:
    NewTableForm(DatabaseEngine* engine, Project* prj, QWidget *parent = 0);
    ~NewTableForm();
    void setMainWindow(MainWindow* mw);
    void focusOnName();

public slots:

    void onAddColumn();
    void onDeleteColumn();
    void onMoveColumnDown();
    void onMoveColumnUp();
    void onItemChanged(QTreeWidgetItem*,QTreeWidgetItem*);
    void onItemSelected(QTreeWidgetItem*, int);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::NewTableForm *m_ui;
    // the main window
    MainWindow* m_mw;
    // the DB engine
    DatabaseEngine* m_dbEngine;
    // the project we are working on
    Project* m_project;
    // the table we are working on
    Table* m_table;
    // the column which is currenty selected (NULL if none)
    Column* m_currentColumn;
};

#endif // NEWTABLEFORM_H
