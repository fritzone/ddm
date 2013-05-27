#ifndef NEWTABLEFORM_H
#define NEWTABLEFORM_H

#include "SourceCodePresenterWidget.h"

#include <QWidget>
#include <QTreeWidgetItem>
#include <QAbstractButton>
#include <QListWidgetItem>
#include <QSignalMapper>

namespace Ui
{
    class NewTableForm;
}

class DatabaseEngine;
class Project;
class Table;
class Column;
class Index;
class ForeignKey;
class AbstractStorageEngineListProvider;
class ContextMenuEnabledTreeWidgetItem;
class SqlHighlighter;
class ContextMenuEnabledTreeWidget;
class SqlNamesValidator;
class Issue;
class QMenu;
class WidgetForSpecificProperties;
class Version;
class QComboBox;

class NewTableForm : public SourceCodePresenterWidget
{
    Q_OBJECT
public:

    NewTableForm(DatabaseEngine* engine, Project* prj, Version *v, QWidget *parent = 0, bool newTable = false);
    ~NewTableForm();

    void focusOnName();
    void focusOnNewColumnName();

    void setTable(Table* table);
    QString getTableName() const;
    Table* getTable() const
    {
        return m_table;
    }

    void selectTab(int);
    void showColumn(Column *);

    void setCurrentColumn(Column* col)
    {
        m_currentColumn = col;
    }
    void resetForeignTablesCombo();

    virtual void presentSql(Project*, Version* v);
    virtual void presentSql(Project*, SqlSource*, MainWindow::showSomething s);

protected:

    void changeEvent(QEvent *e);
    virtual void keyPressEvent(QKeyEvent *);

public slots:

    // main page
    void onChangeTab(int);
    void onChangeName(QString);
    void onLockUnlock(bool);
    void onUndelete();
    void onInstantiate();

    // on the columns page
    void onAddColumn();
    void onDeleteColumn();
    void onMoveColumnDown();
    void onMoveColumnUp();
    void onCancelColumnEditing();
    void onSelectColumn(QTreeWidgetItem*, int);
    void onCopyColumn();
    void onPasteColumn();
    void onDescriptionChange();
    void onColumnNameChange(QString);
    void onDatatypeComboChange(QString);
    void onPrimaryChange(bool);
    void onDatatypeSelectedForColumnInList(const QString&);

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
    void onIndexNameChange(QString);
    void onIndexOrderTypeChanged(QString);
    void onAddSpToColumnOfIndex();
    void onTriggerSpItemForIndexesColumn();
    void onRemoveSpsFromColumnOfIndex();

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
    void onDeleteDefaultRow();

    // description page
    void onChangeDescription();

    // SQL page
    void onInject();
    void onSaveSql();
    void onCodepageChange(QString);

private:

    /*
     * Populates the two listboxes on the index screen in a way that they are containing all the columns,
     * part of them in the left, part of them in the right, depending how the user assigned them to an index
     */
    void populateColumnsForIndices();

    /*
     * Prepares the list of "grayed" items that represent the columns of the parent tables
     */
    //void prepareColumnsListWithParentItems(const Table*);

    void onSave();

    void autoSave();
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

    void updateSqlDueToChange();

    void prepareValuesToBeSaved();

    void doTheSave();

    void finalizeColumnMovement();

    void updateIssues();

    void setTypeComboBoxForColumnItem(ContextMenuEnabledTreeWidgetItem*, Column*);

    void prepareSpsTabs();

    void prepareSpsTabsForIndex(Index*);

    void prepareSpsTabsForColumn(Column *col);

    QMenu* buildPopupForSpsForColumnInIndex();

    void disableEditingControls(bool disabled);

private:
    Ui::NewTableForm *m_ui;
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

    AbstractStorageEngineListProvider* m_engineProviders;
    SqlHighlighter* highlighter;
    QStringList finalSql;
    SqlNamesValidator* m_nameValidator;
    ContextMenuEnabledTreeWidget* lstColumns;
    QSignalMapper* m_signalMapperForCombosInColumns;

    // TODO: with multiple db engines this will be a vector
    WidgetForSpecificProperties* m_wspForIndex;
    WidgetForSpecificProperties* m_wspForColumn;
    WidgetForSpecificProperties* m_mainWsp;

    Version* m_version;

    QMap<QString, QComboBox*> m_combos;

};

#endif // NEWTABLEFORM_H
