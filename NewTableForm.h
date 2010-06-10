#ifndef NEWTABLEFORM_H
#define NEWTABLEFORM_H

#include <QWidget>
#include <QTreeWidgetItem>
#include <QAbstractButton>
#include <QListWidgetItem>

namespace Ui {
    class NewTableForm;
}

class MainWindow;
class DatabaseEngine;
class Project;
class Table;
class Column;
class Index;
class ForeignKey;

class NewTableForm : public QWidget {
    Q_OBJECT
public:

    NewTableForm(DatabaseEngine* engine, Project* prj, QWidget *parent = 0);
    ~NewTableForm();
    void setMainWindow(MainWindow* mw);

    /**
     * Focuses the cursor to the text edit where the user should put the name of the table
     */
    void focusOnName();

    /**
     * Sets the table, also updates all the GUI elements
     */
    void setTable(Table* table);

protected:

    virtual void keyPressEvent(QKeyEvent *);

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
    void onDoubleClickColumnForIndex(QListWidgetItem*);
    // main page
    void onItemChanged(QTreeWidgetItem*,QTreeWidgetItem*); // this is not used
    void onButtonsClicked(QAbstractButton*);
    // foreign columns page
    void onForeignTableComboChange(QString);
    void onForeignTableColumnChange();
    void onAddForeignKeyAssociation();
    void onSelectAssociation(QTreeWidgetItem*,int);
    void onRemoveForeignKeyAssociation();
    void onBtnAddForeignKey();
    void onSelectForeignKey(QTreeWidgetItem*,int);
    // default values page
    void onAddNewDefaultRow();
    void onBtnUpdateTableWithDefaultValues();
    void onSaveStartupValuestoCSV();
    void onLoadStartupValuesFromCSV();
protected:
    void changeEvent(QEvent *e);

private:

    /*
     * Populates the two listboxes on the index screen in a way that they are containing all the columns,
     * part of them in the left, part of them in the right, depending how the user assigned them to an index
     */
    void populateColumnsForIndices();

    /*
     * Prepares the list of "grayed" items that represent the columns of the parent tables
     */
    void prepareColumnsListWithParentItems(const Table*);

    void onSave();
    void onReset();

    void resetIndexGui();

    /*
     * Creates various tree widget items for Columns, Indices, ForeignKeys
     */
    QTreeWidgetItem* createTWIForColumn(const Column* col);
    QTreeWidgetItem* createTWIForIndex(const Index* col);
    QTreeWidgetItem* createTWIForForeignKey(const ForeignKey* col);

    /*
     * Used when the columns have changed, tries to do an intelligent update for the default values screen
     */
    void updateDefaultValuesTableHeader();
    void backupDefaultValuesTable();
    void restoreDefaultValuesTable();

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
    // the foreign table we have selected in the combo
    Table* m_foreignTable;
    // the foreign key we are curently working on. This is either created when selecting from the foreign key list
    // or created when we add the first assoiation in the GUI
    ForeignKey* m_currentForeignKey;
    // true if the m_currentForeignKey was created from an item from the foreign key list, false if it was created with
    // the first association
    bool m_foreignKeySelected;
    // holds the values that were saved from the startup table before a change in the tables columns happened
    QHash<QString, QVector<QString> > m_startupSaves;
    // 0 = no op, 1 = column deletion, 2 = colum insertion , 3 = column rename ... Used when updating the default values table
    int m_columnOperation;
    QString m_newColumnName;
    QString m_oldColumnName;

    // if we have changed anything at all in the screen
    bool m_changes;
};

#endif // NEWTABLEFORM_H
