#ifndef NEWTABLEFORM_H
#define NEWTABLEFORM_H

#include <QWidget>
#include <QTreeWidgetItem>
#include <QAbstractButton>

namespace Ui {
    class NewTableForm;
}

class MainWindow;
class DatabaseEngine;
class Project;
class Table;
class Column;
class Index;

class NewTableForm : public QWidget {
    Q_OBJECT
public:
    NewTableForm(DatabaseEngine* engine, Project* prj, QWidget *parent = 0);
    ~NewTableForm();
    void setMainWindow(MainWindow* mw);
    void focusOnName();

    /**
     * Sets the table, also updates all the GUI elements
     */
    void setTable(Table* table);

public slots:

    // on the columns page
    void onAddColumn();
    void onDeleteColumn();
    void onMoveColumnDown();
    void onMoveColumnUp();
    void onCancelColumnEditing();
    void onItemSelected(QTreeWidgetItem*, int);             // when a column gets selected... sorry for bad naming
    // on the index page
    void onSelectIndex(QTreeWidgetItem*,int);
    void onMoveColumnToRight();
    void onMoveColumnToLeft();
    void onAddIndex();
    void onCancelIndexEditing();
    void onBtnRemoveIndex();
    void onMoveSelectedIndexColumnUp();
    void onMoveSelectedIndexColumnDown();
    // main page
    void onItemChanged(QTreeWidgetItem*,QTreeWidgetItem*); // this is not used
    void onButtonsClicked(QAbstractButton*);
    // foreign columns page
    void onForeignTableComboChange(QString);
    void onForeignTableColumnChange();
    void onAddForeignKeyAssociation();

protected:
    void changeEvent(QEvent *e);

private:

    /**
     * Populates the two listboxes on the index screen in a way that they are containing all the columns,
     * part of them in the left, part of them in the right, depending how the user assigned them to an index
     */
    void populateColumnsForIndices();
    void onSave();
    void onReset();

    void resetIndexGui();
    QTreeWidgetItem* createTWIForColumn(const Column* col);
    QTreeWidgetItem* createTWIForIndex(const Index* col);


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
    // the index that was selected in the indices list
    Index* m_currentIndex;

    Table* m_foreignTable;
};

#endif // NEWTABLEFORM_H
