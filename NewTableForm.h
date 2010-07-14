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
class AbstractStorageEngine;
class AbstractStorageEngineListProvider;
class ContextMenuEnabledTreeWidgetItem;

class NewTableForm : public QWidget {
    Q_OBJECT
public:

    NewTableForm(DatabaseEngine* engine, Project* prj, QWidget *parent = 0);
    ~NewTableForm();
    void setMainWindow(MainWindow* mw);

    void focusOnName();
    void focusOnNewColumnName();

    void setTable(Table* table);

    void selectTab(int);

protected:

    virtual void keyPressEvent(QKeyEvent *);

public slots:

    // main page
    void onStorageEngineChange(QString);
    void onItemChanged(QTreeWidgetItem*,QTreeWidgetItem*); // this is not used
    void onButtonsClicked(QAbstractButton*);

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

    // foreign columns page
    void onForeignTableComboChange(QString);
    void onForeignTableColumnChange();
    void onAddForeignKeyAssociation();
    void onSelectAssociation(QTreeWidgetItem*,int);
    void onRemoveForeignKeyAssociation();
    void onBtnAddForeignKey();
    void onBtnCancelForeignKeyEditing();
    void onSelectForeignKey(QTreeWidgetItem*,int);
    void onBtnRemoveForeignKey();

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
    ContextMenuEnabledTreeWidgetItem* createTWIForColumn(const Column* col, ContextMenuEnabledTreeWidgetItem* parent = 0);
    ContextMenuEnabledTreeWidgetItem* createTWIForIndex(const Index* col);
    ContextMenuEnabledTreeWidgetItem* createTWIForForeignKey(const ForeignKey* col);

    /*
     * Used when the columns have changed, tries to do an intelligent update for the default values screen
     */
    void updateDefaultValuesTableHeader();
    void backupDefaultValuesTable();
    void restoreDefaultValuesTable();

    void populateIndexTypesDependingOnStorageEngine();
    void enableForeignKeysDependingOnStorageEngine();

    /*
     * Populates the gui with the given table. current has the following role: if this is the "bottom" of a specialization chain then the fields are editable (white background)
     * otherwise they are disabled (grey background)
     */
    void populateTable(const Table* table, bool);

    void toggleColumnFieldDisableness(bool);

    void populateIndexGui(Index* idx);

    void toggleIndexFieldDisableness(bool);

    void populateFKGui(ForeignKey*);

    void toggleFkFieldDisableness(bool);

    void resetFkGui();

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
    AbstractStorageEngine* m_currentStorageEngine;
    AbstractStorageEngineListProvider* m_engineProviders;
};

#endif // NEWTABLEFORM_H
