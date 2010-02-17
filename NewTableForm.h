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
    MainWindow* m_mw;
    DatabaseEngine* m_dbEngine;
    Project* m_project;
    Table* m_table;
};

#endif // NEWTABLEFORM_H
