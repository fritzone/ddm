#include "NewTableForm.h"
#include "ui_NewTableForm.h"
#include "IconFactory.h"
#include "DatabaseEngine.h"
#include "Project.h"
#include "Version.h"
#include "UserDataType.h"
#include "Column.h"
#include "Index.h"
#include "Table.h"
#include "mainwindow.h"
#include "AbstractIndextypeProvider.h"
#include "AbstractStorageEngineListProvider.h"
#include "ForeignKey.h"

#include <QMessageBox>
#include <QHashIterator>
#include <QHash>
#include <QKeyEvent>
#include <QtGui>

// the positions of various items in the columns view, used for icon retrieval mostly
const int COL_POS_PK = 0;
const int COL_POS_NM = 1;
const int COL_POS_DT = 2;

NewTableForm::NewTableForm(DatabaseEngine* db, Project* prj, QWidget *parent) : QWidget(parent), m_ui(new Ui::NewTableForm),
    m_dbEngine(db), m_project(prj), m_table(new Table()), m_currentColumn(0), m_currentIndex(0), m_foreignTable(0),
    m_currentForeignKey(0), m_foreignKeySelected(false), m_changes(false), m_currentStorageEngine(0), m_engineProviders(0)
{
    m_ui->setupUi(this);

    const QVector<UserDataType*>& dts = m_project->getWorkingVersion()->getDataTypes();
    for(int i=0; i<dts.size(); i++)
    {
        m_ui->cmbNewColumnType->addItem(dts[i]->getIcon(), dts[i]->getName());
    }

    m_ui->lstColumns->header()->resizeSection(0, 50);
    m_ui->cmbNewColumnType->setCurrentIndex(-1);

    // fill in the storage engine types
    m_engineProviders = db->getStorageEngineListProviders();
    if(m_engineProviders)
    {
        const QVector<AbstractStorageEngine*>& storageEngines = m_engineProviders->getStorageEngines();
        for(int i=0; i<storageEngines.size(); i++)
        {
            m_ui->cmbStorageEngine->addItem(storageEngines.at(i)->name());
            if(i==0)
            {
                m_currentStorageEngine = storageEngines.at(i);
            }
        }
    }
    else
    {
        m_ui->lblStorageEngine->hide();
        m_ui->cmbStorageEngine->hide();
    }

    // fill in the index types combo box depending on the storage engine if applicable
    populateIndexTypesDependingOnStorageEngine();
    enableForeignKeysDependingOnStorageEngine();
    m_table->setStorageEngine(m_currentStorageEngine);

    // create the foreign keys screen
    const QVector<Table*>& tables = m_project->getWorkingVersion()->getTables();
    for(int i=0; i<tables.size(); i++)
    {
        if(tables[i]->getName() != m_table->getName())
        {
            m_ui->cmbForeignTables->addItem(tables[i]->getName());
        }
    }
    m_ui->cmbForeignTables->setCurrentIndex(-1);

    m_ui->btnAdvanced->hide();

    m_ui->lblCharacterSet->hide();
    m_ui->cmbCharacterSets->hide();

    if(!db->supportsEngines())
    {
        m_ui->lblStorageEngine->hide();
        m_ui->cmbStorageEngine->hide();
    }
    else
    {

    }

    m_ui->tabWidget->setCurrentIndex(0);
}

NewTableForm::~NewTableForm()
{
    delete m_ui;
}

void NewTableForm::populateIndexTypesDependingOnStorageEngine()
{
    const QStringList& indexTypes = m_dbEngine->getIndextypeProvider()->getIndexTypes();
    QStringList::const_iterator it = indexTypes.constBegin();

    m_ui->cmbIndexType->clear();
    while(it != indexTypes.constEnd())
    {
        if(m_engineProviders)
        {
            if(m_currentStorageEngine && m_currentStorageEngine->supportsIndexType(*it))
            {
                m_ui->cmbIndexType->addItem(*it);
            }
        }
        else
        {
            m_ui->cmbIndexType->addItem(*it);
        }

        it ++;
    }
}

void NewTableForm::enableForeignKeysDependingOnStorageEngine()
{
    m_ui->grpForeignKeysList->setEnabled(true);
    m_ui->grpNewForeignKey->setEnabled(true);
    m_ui->lblForeignKeysNotAllowed->setVisible(false);
    if(m_engineProviders && m_currentStorageEngine)
    {
        if(!m_currentStorageEngine->supportsForeignKeys())
        {
            m_ui->grpForeignKeysList->setEnabled(false);
            m_ui->grpNewForeignKey->setEnabled(false);
            m_ui->lblForeignKeysNotAllowed->setVisible(true);
        }
    }
}

QTreeWidgetItem* NewTableForm::createTWIForForeignKey(const ForeignKey* fk)
{
    QStringList a(fk->getName());
    const QVector<ForeignKey::ColumnAssociation*>& assocs = fk->getAssociations();
    QString tabName = "";
    QString foreignColumns = "";
    QString localColumns = "";
    for(int i=0; i<assocs.size(); i++)
    {
        tabName = assocs[i]->getForeignTable()->getName();
        foreignColumns += assocs[i]->getForeignColumn()->getName();
        if(i < assocs.size()-1) foreignColumns += ", ";
        localColumns += assocs[i]->getLocalColumn()->getName();
        if(i < assocs.size()-1) localColumns += ", ";
    }

    a.append(tabName);
    a.append(foreignColumns);
    a.append(localColumns);
    a.append(fk->getOnUpdate());
    a.append(fk->getOnDelete());

    QTreeWidgetItem* item = new QTreeWidgetItem((QTreeWidget*)0, a);
    return item;
}

QTreeWidgetItem* NewTableForm::createTWIForColumn(const Column* col)
{
    QStringList a("");
    a.append(col->getName());
    a.append(col->getDataType()->getName());
    a.append(col->getDataType()->sqlAsString());

    QTreeWidgetItem* item = new QTreeWidgetItem((QTreeWidget*)0, a);
    if(col->isPk())
    {
        item->setIcon(COL_POS_PK, IconFactory::getKeyIcon());
    }
    item->setIcon(COL_POS_DT, col->getDataType()->getIcon());
    return item;
}

void NewTableForm::prepareColumnsListWithParentItems(const Table* ctable)
{
    // first step: go back upwards, recursively
    if(ctable->getParent())
    {
        prepareColumnsListWithParentItems(ctable->getParent());
    }
    // then create the column list
    if(ctable)
    {
        const QVector<Column*>& columns = ctable->getColumns();
        for(int i=0; i<columns.count(); i++)
        {
            static QBrush grayBrush(QColor(Qt::gray));
            QTreeWidgetItem* item = createTWIForColumn(columns[i]);
            item->setBackground(0, grayBrush);
            m_ui->lstColumns->addTopLevelItem(item);
            columns[i]->setLocation(item);
        }
    }

}

void NewTableForm::setTable(Table *table)
{
    m_table = table;

    // first step: set the columns
    const QVector<Column*>& columns = m_table->getColumns();
    for(int i=0; i<columns.count(); i++)
    {
        QTreeWidgetItem* item = createTWIForColumn(columns[i]);
        m_ui->lstColumns->addTopLevelItem(item);
        columns[i]->setLocation(item);
    }

    // second step: set up the indices
    const QVector<Index*>& indices = m_table->getIndices();
    for(int i=0; i<indices.count(); i++)
    {
        QTreeWidgetItem* item = createTWIForIndex(indices[i]);
        m_ui->lstIndices->addTopLevelItem(item);
        indices[i]->setLocation(item);
    }
    resetIndexGui();

    // third step: set up the foreign keys
    const QVector<ForeignKey*> & fks = m_table->getFks();
    for(int i=0; i<fks.size(); i++)
    {
        QTreeWidgetItem* item = createTWIForForeignKey(fks[i]);
        m_ui->lstForeignKeys->addTopLevelItem(item);
        fks[i]->setLocation(item);
    }

    // set the default values
    updateDefaultValuesTableHeader();
    for(int i=0; i<m_table->getStartupValues().size(); i++)
    {
        const QVector<QString>& rowI = m_table->getStartupValues()[i];
        m_ui->tableStartupValues->insertRow(i);
        for(int j=0; j<rowI.size(); j++)
        {
            int rc = m_ui->tableStartupValues->rowCount();
            int cc = m_ui->tableStartupValues->columnCount();
            QTableWidgetItem* newItem = new QTableWidgetItem(rowI[j]);
            m_ui->tableStartupValues->setItem(i, j, newItem);
        }
    }
    m_ui->txtTableName->setText(m_table->getName());
    m_ui->chkPersistent->setChecked(m_table->isPersistent());
    m_ui->chkTemporary->setChecked(m_table->isTemporary());
}

void NewTableForm::focusOnName()
{
    m_ui->txtTableName->setFocus();
}

void NewTableForm::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void NewTableForm::setMainWindow(MainWindow *mw)
{
    this->m_mw = mw;
}

void NewTableForm::onAddColumn()
{
    if(m_ui->cmbNewColumnType->currentIndex() == -1)
    {
        QMessageBox::critical (this, tr("Error"), tr("Please select a type"), QMessageBox::Ok);
        return;
    }

    if(m_ui->txtNewColumnName->text().length() == 0)
    {
        QMessageBox::critical (this, tr("Error"), tr("Please specify a name"), QMessageBox::Ok);
        return;
    }

    backupDefaultValuesTable();
    if(m_currentColumn)     // we are working on a column
    {
        if(m_currentColumn->getName() != m_ui->txtNewColumnName->text())
        {
            m_columnOperation = 3;
            m_newColumnName = m_ui->txtNewColumnName->text();
            m_oldColumnName = m_currentColumn->getName();
        }
        m_currentColumn->setName(m_ui->txtNewColumnName->text());
        m_currentColumn->setDescription(m_ui->txtColumnDescription->toPlainText());
        m_currentColumn->setPk(m_ui->chkPrimary->isChecked());
        m_currentColumn->setAutoIncrement(m_ui->chkAutoInc->isChecked());
        m_currentColumn->setDataType(m_project->getWorkingVersion()->getDataType(m_ui->cmbNewColumnType->currentText()));

        m_currentColumn->getLocation()->setText(1, m_currentColumn->getName());
        if(m_ui->chkPrimary->isChecked())
        {
            m_currentColumn->getLocation()->setIcon(COL_POS_PK, IconFactory::getKeyIcon());
        }
        else
        {
            m_currentColumn->getLocation()->setIcon(COL_POS_PK, IconFactory::getEmptyIcon());
        }
        m_currentColumn->getLocation()->setIcon(COL_POS_DT, m_currentColumn->getDataType()->getIcon());
        m_currentColumn->getLocation()->setText(COL_POS_DT, m_currentColumn->getDataType()->getName());
        m_currentColumn = 0;
    }
    else                    // we are not working on a column, but adding a new one
    {
        for(int i=0; i < m_table->getColumns().size(); i++)
        {
            if(m_table->getColumn(i)->getName() == m_ui->txtNewColumnName->text())
            {
                QMessageBox::critical (this, tr("Error"), tr("Please use a unique name for each of the columns"), QMessageBox::Ok);
                return;
            }
        }

        UserDataType* colsDt = m_project->getWorkingVersion()->getDataType(m_ui->cmbNewColumnType->currentText());
        Column* col = new Column(m_ui->txtNewColumnName->text(), colsDt, m_ui->chkAutoInc->isChecked(), m_ui->chkPrimary->isChecked()) ;
        col->setDescription(m_ui->txtColumnDescription->toPlainText());
        QTreeWidgetItem* item = createTWIForColumn(col);
        m_ui->lstColumns->addTopLevelItem(item);
        m_table->addColumn(col);

        col->setLocation(item);
        m_ui->txtNewColumnName->setFocus( );
    }

    m_ui->txtNewColumnName->setText("");
    m_ui->cmbNewColumnType->setCurrentIndex(-1);
    m_ui->txtColumnDescription->setText("");
    m_ui->chkPrimary->setChecked(false);
    m_ui->chkAutoInc->setChecked(false);
    m_ui->btnAdd->setIcon(IconFactory::getAddIcon());

    populateColumnsForIndices();
    updateDefaultValuesTableHeader();
    restoreDefaultValuesTable();

    m_changes = true;
}

void NewTableForm::onCancelColumnEditing()
{
    m_ui->txtNewColumnName->clear();
    m_ui->cmbNewColumnType->setCurrentIndex(-1);
    m_currentColumn = 0;
    m_ui->chkPrimary->setChecked(false);
    m_ui->chkAutoInc->setChecked(false);
    m_ui->btnAdd->setIcon(IconFactory::getAddIcon());
}

void NewTableForm::onDeleteColumn()
{
    QTreeWidgetItem* currentItem = m_ui->lstColumns->currentItem();
    if(currentItem == 0)
    {
        QMessageBox::critical (this, tr("Error"), tr("Internal Error: Nothing is selected?"), QMessageBox::Ok);
        return;
    }
    const Column* currentColumn = m_table->getColumn(currentItem->text(1));
    const Index* usedIn = m_table->isColumnUsedInIndex(currentColumn);
    if(usedIn != 0)
    {
        QMessageBox::critical (this, tr("Error"), tr("This column cannot be deleted since it's used in an index. Delete this index first: ") + usedIn->getName(), QMessageBox::Ok);
        return;
    }
    backupDefaultValuesTable();
    delete m_ui->lstColumns->currentItem();
    onCancelColumnEditing();
    m_table->removeColumn(const_cast<Column*>(currentColumn));
    m_currentColumn = 0;
    populateColumnsForIndices();
    updateDefaultValuesTableHeader();
    restoreDefaultValuesTable();

    m_changes = true;
}

void NewTableForm::onMoveColumnDown()
{
    if(m_ui->lstColumns->selectedItems().size() > 0)
    {
        QModelIndex x = m_ui->lstColumns->currentIndex();
        if(x.row() < m_ui->lstColumns->topLevelItemCount() - 1)
        {
            m_table->moveColumnDown(x.row());
            QTreeWidgetItem* w = new QTreeWidgetItem(m_ui->lstColumns->currentItem()->type());
            *w = *m_ui->lstColumns->currentItem();
            delete m_ui->lstColumns->currentItem();
            m_ui->lstColumns->insertTopLevelItem(x.row() + 1, w);
            m_ui->lstColumns->setCurrentItem(w);
            populateColumnsForIndices();
            // TODO: repatriate the columns from the exchanged one
            backupDefaultValuesTable();
            updateDefaultValuesTableHeader();
            restoreDefaultValuesTable();

            m_changes = true;
        }
    }
}

void NewTableForm::onMoveColumnUp()
{
    if(m_ui->lstColumns->selectedItems().size() > 0)
    {
        QModelIndex x = m_ui->lstColumns->currentIndex();
        if(x.row() > 0)
        {
            m_table->moveColumnUp(x.row());
            QTreeWidgetItem* w = new QTreeWidgetItem(m_ui->lstColumns->currentItem()->type());
            *w = *m_ui->lstColumns->currentItem();
            delete m_ui->lstColumns->currentItem();
            m_ui->lstColumns->insertTopLevelItem(x.row() - 1, w);
            m_ui->lstColumns->setCurrentItem(w);
            populateColumnsForIndices();
            // TODO: repatriate the columns from the exchanged one
            backupDefaultValuesTable();
            updateDefaultValuesTableHeader();
            restoreDefaultValuesTable();
            m_changes = true;
        }
    }
}

void NewTableForm::onItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{

}

/**
 * Called when a column is selected. onColumnSelect onSelectColumn
 */
void NewTableForm::onItemSelected(QTreeWidgetItem* current, int column)
{
    QModelIndex x = m_ui->lstColumns->currentIndex();
    m_currentColumn = m_table->getColumn(x.row());

    m_ui->txtNewColumnName->setText(m_currentColumn->getName());
    m_ui->cmbNewColumnType->setCurrentIndex(m_project->getWorkingVersion()->getDataTypeIndex(m_currentColumn->getDataType()->getName()));
    m_ui->chkPrimary->setChecked(m_currentColumn->isPk());
    m_ui->chkAutoInc->setChecked(m_currentColumn->hasAutoIncrement());
    m_ui->txtColumnDescription->setText(m_currentColumn->getDescription());

    m_ui->btnAdd->setIcon(IconFactory::getApplyIcon());
    m_ui->btnCancelColumnEditing->show();
}


void NewTableForm::populateColumnsForIndices()
{
    m_ui->lstAvailableColumnsForIndex->clear();
    for(int i=0; i<m_table->getColumns().size(); i++)
    {
        if(m_ui->lstSelectedColumnsForIndex->count() == 0)
        {
            QListWidgetItem* qlwi = new QListWidgetItem(m_table->getColumns()[i]->getName(), m_ui->lstAvailableColumnsForIndex);
            qlwi->setIcon(m_table->getColumns()[i]->getDataType()->getIcon());
        }
        else
        {
            for(int j = 0; j<m_ui->lstSelectedColumnsForIndex->count(); j++)
            {
                if(m_ui->lstSelectedColumnsForIndex->item(j)->text() != m_table->getColumns()[i]->getName())
                {
                    QListWidgetItem* qlwi = new QListWidgetItem(m_table->getColumns()[i]->getName(), m_ui->lstAvailableColumnsForIndex);
                    qlwi->setIcon(m_table->getColumns()[i]->getDataType()->getIcon());
                }
            }
        }
    }
}

void NewTableForm::onButtonsClicked(QAbstractButton* btn)
{
    if(btn == m_ui->buttons->buttons().at(0)) // Seems very strange, but works like this... Save is the First, after Reset
    {
        onSave();
    }
    else
    {
        onReset();
    }
}

void NewTableForm::onSave()
{
    if(m_ui->txtTableName->text().length() == 0)
    {
        QMessageBox::critical (this, tr("Error"), tr("Please specify at least the name of the table!"), QMessageBox::Ok);
        return;
    }

    m_table->setName(m_ui->txtTableName->text());
    m_table->setPersistent(m_ui->chkPersistent->isChecked());
    m_table->setTemporary(m_ui->chkTemporary->isChecked());
    m_table->setDescription(m_ui->txtDescription->toPlainText());
    m_table->setStorageEngine(m_currentStorageEngine);

    if(m_project->getWorkingVersion()->hasTable(m_table))
    {
        // update the data of the table, and the tree view
        m_mw->onUpdateTable(m_table);
    }
    else
    {
        // create a new tree entry, add to the tree, update the m_table's tree item.
        m_mw->onSaveNewTable(m_table);
    }

    m_changes = false;
}

void NewTableForm::onReset()
{
}

void NewTableForm::onMoveColumnToRight()
{
    if(!m_ui->lstAvailableColumnsForIndex->currentItem()) return;
    QListWidgetItem* itm = new QListWidgetItem(*m_ui->lstAvailableColumnsForIndex->currentItem());
    delete m_ui->lstAvailableColumnsForIndex->currentItem();
    m_ui->lstSelectedColumnsForIndex->addItem(itm);
}

void NewTableForm::onMoveColumnToLeft()
{
    if(!m_ui->lstSelectedColumnsForIndex->currentItem()) return;
    delete m_ui->lstSelectedColumnsForIndex->currentItem();
    populateColumnsForIndices();
}

QTreeWidgetItem* NewTableForm::createTWIForIndex(const Index* index)
{
    // create the tree widget item
    QString columnsAsString = "";

    const QVector<const Column*>& indexColumns = index->getColumns();
    int cnt = indexColumns.size();
    for(int i = 0; i < cnt; i++)
    {
        columnsAsString += indexColumns[i]->getName();
        if(i < cnt - 1)
        {
            columnsAsString += ", ";
        }
    }

    // create the listview entry
    QStringList a(index->getName());
    a.append(index->getType());
    a.append(columnsAsString);

    QTreeWidgetItem* item = new QTreeWidgetItem((QTreeWidget*)0, a);
    return item;
}

void NewTableForm::onAddIndex()
{
    // check the prerequisites: a valid and unique name, and at least one column
    if(m_ui->txtNewIndexName->text().length() == 0)
    {
        QMessageBox::critical (this, tr("Error"), tr("Please specify a name for the index"), QMessageBox::Ok);
        m_ui->txtNewIndexName->setFocus();
        return;
    }

    if(m_ui->lstSelectedColumnsForIndex->count() == 0)
    {
        QMessageBox::critical (this, tr("Error"), tr("Please specify at least one column"), QMessageBox::Ok);
        return;
    }

    if(m_currentIndex == 0)
    {
        // create the index object and populate with requried columns
        Index* index = new Index(m_ui->txtNewIndexName->text(), m_ui->cmbIndexType->currentText());
        int cnt = m_ui->lstSelectedColumnsForIndex->count();
        for(int i = 0; i< cnt; i++)
        {
            index->addColumn(m_table->getColumn(m_ui->lstSelectedColumnsForIndex->item(i)->text()));
        }
        m_table->addIndex(index);

        QTreeWidgetItem* item = createTWIForIndex(index);
        m_ui->lstIndices->addTopLevelItem(item);
        index->setLocation(item);
    }
    else    // update the index with the modified data
    {
        m_currentIndex->setName(m_ui->txtNewIndexName->text());
        m_currentIndex->setType(m_ui->cmbIndexType->currentText());
        m_currentIndex->resetColumns();
        // TODO: "almost" duplicate code, consider refactoring
        QString columnsAsString = "";
        int cnt = m_ui->lstSelectedColumnsForIndex->count();
        for(int i = 0; i< cnt; i++)
        {
            m_currentIndex->addColumn(m_table->getColumn(m_ui->lstSelectedColumnsForIndex->item(i)->text()));
            columnsAsString += m_ui->lstSelectedColumnsForIndex->item(i)->text();
            if(i < cnt - 1)
            {
                columnsAsString += ", ";
            }
        }

        m_currentIndex->getLocation()->setText(0, m_currentIndex->getName());
        m_currentIndex->getLocation()->setText(1, m_currentIndex->getType());
        m_currentIndex->getLocation()->setText(2, columnsAsString);
    }
    resetIndexGui();

    m_currentIndex = 0;

    m_changes = true;
}

void NewTableForm::resetIndexGui()
{
    m_ui->lstSelectedColumnsForIndex->clear();
    populateColumnsForIndices();
    m_ui->cmbIndexType->setCurrentIndex(0);
    m_ui->txtNewIndexName->setText("");
    m_ui->btnAddIndex->setIcon(IconFactory::getAddIcon());
}

void NewTableForm::onSelectIndex(QTreeWidgetItem* current, int column)
{
    QModelIndex x = m_ui->lstIndices->currentIndex();
    m_currentIndex = m_table->getIndex(x.row());

    m_ui->txtNewIndexName->setText(m_currentIndex->getName());

    // fill up the two lists for the index columns
    m_ui->lstAvailableColumnsForIndex->clear();
    m_ui->lstSelectedColumnsForIndex->clear();

    for(int i=0; i< m_table->getColumns().size(); i++)
    {
        Column* column = m_table->getColumns()[i];
        QListWidget *targetList = 0;
        if(m_currentIndex->hasColumn(column))
        {
            targetList = m_ui->lstSelectedColumnsForIndex;
        }
        else
        {
            targetList = m_ui->lstAvailableColumnsForIndex;
        }
        QListWidgetItem* qlwi = new QListWidgetItem(column->getName(), targetList);
        qlwi->setIcon(column->getLocation()->icon(COL_POS_DT));
    }

    // select the required combo box item
    const QStringList& indexTypes = m_dbEngine->getIndextypeProvider()->getIndexTypes();
    QStringList::const_iterator it = indexTypes.constBegin();
    int indx = 0;
    while(it != indexTypes.constEnd())
    {
        if(*it == m_currentIndex->getType())
        {
            break;
        }
        indx ++;
        it ++;
    }
    m_ui->cmbIndexType->setCurrentIndex(indx);
    m_ui->btnAddIndex->setIcon(IconFactory::getApplyIcon());

}

void NewTableForm::onCancelIndexEditing()
{
    resetIndexGui();
}

void NewTableForm::onBtnRemoveIndex()
{
    delete m_ui->lstIndices->currentItem();
    resetIndexGui();
    m_table->removeIndex(m_currentIndex);
    m_currentIndex = 0;
    m_changes = true;
}

void NewTableForm::onDoubleClickColumnForIndex(QListWidgetItem* item)
{
    if(!item) return;
    QListWidgetItem* itm = new QListWidgetItem(*item);
    delete m_ui->lstAvailableColumnsForIndex->currentItem();
    m_ui->lstSelectedColumnsForIndex->addItem(itm);

}

void NewTableForm::onMoveSelectedIndexColumnUp()
{
    if(m_ui->lstSelectedColumnsForIndex->selectedItems().size() > 0)
    {
        QModelIndex x = m_ui->lstSelectedColumnsForIndex->currentIndex();
        if(x.row() > 0)
        {
            QListWidgetItem* w = new QListWidgetItem(*m_ui->lstSelectedColumnsForIndex->currentItem());
            delete m_ui->lstSelectedColumnsForIndex->currentItem();
            m_ui->lstSelectedColumnsForIndex->insertItem(x.row() - 1, w);
            m_ui->lstSelectedColumnsForIndex->setCurrentItem(w);
        }
    }
}

void NewTableForm::onMoveSelectedIndexColumnDown()
{
    if(m_ui->lstSelectedColumnsForIndex->selectedItems().size() > 0)
    {
        QModelIndex x = m_ui->lstSelectedColumnsForIndex->currentIndex();
        if(x.row() < m_ui->lstSelectedColumnsForIndex->count() - 1)
        {
            QListWidgetItem* w = new QListWidgetItem(*m_ui->lstSelectedColumnsForIndex->currentItem());
            delete m_ui->lstSelectedColumnsForIndex->currentItem();
            m_ui->lstSelectedColumnsForIndex->insertItem(x.row() + 1, w);
            m_ui->lstSelectedColumnsForIndex->setCurrentItem(w);
        }
    }
}

void NewTableForm::onForeignTableComboChange(QString selected)
{
    Table* table = m_project->getWorkingVersion()->getTable(selected);
    if(table == 0)
    {
        return;
    }
    m_foreignTable = table;
    const QVector<Column*> & foreignColumns = table->getColumns();
    m_ui->lstForeignTablesColumns->clear();
    for(int i=0; i<foreignColumns.size(); i++)
    {
        QListWidgetItem* qlwi = new QListWidgetItem(table->getColumns()[i]->getName(), m_ui->lstForeignTablesColumns);
        qlwi->setIcon(table->getColumns()[i]->getDataType()->getIcon());
    }
    // now clear the current stuff, it's not valid keeping foreign keys from other tables, when moving to a new table
    m_ui->lstForeignKeyAssociations->clear();
    m_currentForeignKey = 0;
}

void NewTableForm::onForeignTableColumnChange()
{
    m_ui->lstLocalColumn->clear();
    QList<QListWidgetItem *> selectedItems = m_ui->lstForeignTablesColumns->selectedItems();
    for(int i=0; i< selectedItems.size(); i++)
    {
        const Column* foreignColumn = m_foreignTable->getColumn(selectedItems[i]->text());
        for(int j=0; j<m_table->getColumns().size(); j++)
        {
            if(m_table->getColumns()[j]->getDataType()->getName() == foreignColumn->getDataType()->getName())
            {
                QListWidgetItem* qlwj = new QListWidgetItem(m_table->getColumns()[j]->getName(), m_ui->lstLocalColumn);
                qlwj->setIcon(m_table->getColumns()[j]->getDataType()->getIcon());
            }
        }
        break;
    }
}

void NewTableForm::onAddForeignKeyAssociation()
{
    if(!m_currentForeignKey)
    {
        m_currentForeignKey = new ForeignKey();
        m_foreignKeySelected = false;
    }

    QString foreignColumn, localColumn;
    const Column* cforeignColumn, *clocalColumn;
    QList<QListWidgetItem *> selectedItems = m_ui->lstForeignTablesColumns->selectedItems();
    for(int i=0; i< selectedItems.size(); i++)
    {
        foreignColumn = selectedItems[i]->text();
        cforeignColumn = m_foreignTable->getColumn(selectedItems[i]->text());
        break;
    }

    selectedItems = m_ui->lstLocalColumn->selectedItems();
    for(int i=0; i< selectedItems.size(); i++)
    {
        localColumn = selectedItems[i]->text();
        clocalColumn = m_table->getColumn(selectedItems[i]->text());
        break;
    }

    if(localColumn.length() == 0 || foreignColumn.length() == 0)
    {
        QMessageBox::critical (this, tr("Error"), tr("Please select two columns: one from the foreign table, one from the local table"), QMessageBox::Ok);
        return;
    }

    QStringList a(foreignColumn);
    a.append(localColumn);
    QTreeWidgetItem* item = new QTreeWidgetItem((QTreeWidget*)0, a);

    // sets the data, this will be used on the "on click on this item" method to correctly populate the controls
    QVariant var(m_foreignTable->getName());
    item->setData(0, Qt::UserRole, var);

    item->setIcon(0, cforeignColumn->getDataType()->getIcon());
    item->setIcon(1, clocalColumn->getDataType()->getIcon());

    m_ui->lstForeignKeyAssociations->addTopLevelItem(item);

    ForeignKey::ColumnAssociation* fca = new ForeignKey::ColumnAssociation(m_foreignTable, cforeignColumn, m_table, clocalColumn);
    m_currentForeignKey->addAssociation(fca);

}

void NewTableForm::onRemoveForeignKeyAssociation()
{

    // from the foreign key (if it was selected) delete the FK Association in the current item
    QString localColumn = m_ui->lstForeignKeyAssociations->currentItem()->text(1);
    QString foreignColumn = m_ui->lstForeignKeyAssociations->currentItem()->text(0);
    if(m_foreignKeySelected && m_currentForeignKey)
    {
        m_currentForeignKey->removeAssociation(foreignColumn, localColumn);
    }
    // this deletes only the current entry in the list
    delete m_ui->lstForeignKeyAssociations->currentItem();

}

void NewTableForm::onSelectAssociation(QTreeWidgetItem* current, int column)
{
    QVariant qv = current->data(0, Qt::UserRole);
    QString tabName = qv.toString();
    m_foreignTable = m_project->getWorkingVersion()->getTable(tabName);
    if(m_foreignTable == 0)
    {
        QMessageBox::critical (this, tr("Internal Error (1)"), tr("Please contact the developers"), QMessageBox::Ok);
        return;
    }
    // populate the first list view with the foreign table columns
    const QVector<Column*> & foreignColumns = m_foreignTable->getColumns();
    m_ui->lstForeignTablesColumns->clear();
    for(int i=0; i<foreignColumns.size(); i++)
    {
        QListWidgetItem* qlwi = new QListWidgetItem(m_foreignTable->getColumns()[i]->getName(), m_ui->lstForeignTablesColumns);
        qlwi->setIcon(m_foreignTable->getColumns()[i]->getDataType()->getIcon());
    }

    int foundIndex = m_ui->cmbForeignTables->findText(tabName);
    if(foundIndex == -1)
    {
        QMessageBox::critical (this, tr("Internal Error"), tr("Please contact the developers"), QMessageBox::Ok);
        return;
    }

    m_ui->cmbForeignTables->setCurrentIndex(foundIndex);

    // select in the foreign tables column
    for(int i=0; i<m_ui->lstForeignTablesColumns->count(); i++)
    {
        if(m_ui->lstForeignTablesColumns->item(i)->text() == current->text(0))
        {
            m_ui->lstForeignTablesColumns->setCurrentItem(m_ui->lstForeignTablesColumns->item(i));
        }
    }
    // select in the working tables column
    for(int i=0; i<m_ui->lstLocalColumn->count(); i++)
    {
        if(m_ui->lstLocalColumn->item(i)->text() == current->text(1))
        {
            m_ui->lstLocalColumn->setCurrentItem(m_ui->lstLocalColumn->item(i));
        }
    }

}

void NewTableForm::onBtnAddForeignKey()
{
    if(m_currentForeignKey == 0 || m_ui->lstForeignKeyAssociations->topLevelItemCount() == 0)
    {
        QMessageBox::critical (this, tr("Error"), tr("Please specify at least 1 foreign key association"), QMessageBox::Ok);
        return;
    }

    if(m_ui->txtForeignKeyName->text().length() == 0)
    {
        QMessageBox::critical (this, tr("Error"), tr("Please specify a name for the foreign key"), QMessageBox::Ok);
        m_ui->txtForeignKeyName->setFocus();
        return;
    }

    m_currentForeignKey->setName(m_ui->txtForeignKeyName->text());
    m_currentForeignKey->setOnUpdate(m_ui->cmbFkOnUpdate->currentText());
    m_currentForeignKey->setOnDelete(m_ui->cmbFkOnDelete->currentText());

    if(m_foreignKeySelected)
    {
        m_currentForeignKey->getLocation()->setText(0, m_currentForeignKey->getName());
        const QVector<ForeignKey::ColumnAssociation*>& assocs = m_currentForeignKey->getAssociations();
        /* TODO : Duplicate code, consider refactoring */
        QString tabName = "";
        QString foreignColumns = "";
        QString localColumns = "";
        for(int i=0; i<assocs.size(); i++)
        {
            tabName = assocs[i]->getForeignTable()->getName();
            foreignColumns += assocs[i]->getForeignColumn()->getName();
            if(i < assocs.size()-1) foreignColumns += ", ";
            localColumns += assocs[i]->getLocalColumn()->getName();
            if(i < assocs.size()-1) localColumns += ", ";
        }

        m_currentForeignKey->getLocation()->setText(1,tabName);
        m_currentForeignKey->getLocation()->setText(2,foreignColumns);
        m_currentForeignKey->getLocation()->setText(3,localColumns);

    }
    else
    {
        QTreeWidgetItem* twi = createTWIForForeignKey(m_currentForeignKey);
        m_ui->lstForeignKeys->addTopLevelItem(twi);
        m_currentForeignKey->setLocation(twi);
        m_table->addForeignKey(m_currentForeignKey);
    }

    m_currentForeignKey = 0;
    m_foreignKeySelected = false;
    // empty the FK ui fields
    m_ui->cmbForeignTables->setCurrentIndex(-1);
    m_ui->lstForeignKeyAssociations->clear();
    m_ui->lstForeignTablesColumns->clear();
    m_ui->lstLocalColumn->clear();
    m_ui->txtForeignKeyName->clear();
    m_ui->cmbFkOnDelete->setCurrentIndex(-1);
    m_ui->cmbFkOnUpdate->setCurrentIndex(-1);
    m_ui->btnAdd->setIcon(IconFactory::getAddIcon());
    m_changes = true;
}

void NewTableForm::onSelectForeignKey(QTreeWidgetItem* itm, int col)
{
    QModelIndex x = m_ui->lstForeignKeys->currentIndex();
    m_currentForeignKey = m_table->getForeignKey(x.row());

    m_ui->lstForeignKeyAssociations->clear();
    m_ui->cmbForeignTables->setCurrentIndex(-1);
    m_ui->txtForeignKeyName->setText(m_currentForeignKey->getName());

    m_foreignKeySelected = true;

    // create the columns associations
    QString tabName = "";
    const QVector<ForeignKey::ColumnAssociation*>& assocs = m_currentForeignKey->getAssociations();
    for(int i=0; i<assocs.size(); i++)
    {
        tabName = assocs[i]->getForeignTable()->getName();
        QStringList a(assocs[i]->getForeignColumn()->getName());
        a.append(assocs[i]->getLocalColumn()->getName());
        QTreeWidgetItem* item = new QTreeWidgetItem((QTreeWidget*)0, a);

        // sets the data, this will be used on the "on click on this item" method to correctly populate the controls
        QVariant var(assocs[i]->getForeignTable()->getName());
        item->setData(0, Qt::UserRole, var);

        item->setIcon(0, assocs[i]->getForeignColumn()->getDataType()->getIcon());
        item->setIcon(1, assocs[i]->getLocalColumn()->getDataType()->getIcon());

        m_ui->lstForeignKeyAssociations->addTopLevelItem(item);
    }
    // find the foreign keys table in the combo box
    int idx = m_ui->cmbForeignTables->findText(tabName);
    Table* table = m_project->getWorkingVersion()->getTable(tabName);
    // populate the foreign columns list
    if(table == 0)
    {
        return;
    }
    m_foreignTable = table;
    m_ui->cmbForeignTables->setCurrentIndex(idx);
    const QVector<Column*> & foreignColumns = table->getColumns();
    m_ui->lstForeignTablesColumns->clear();
    for(int i=0; i<foreignColumns.size(); i++)
    {
        QListWidgetItem* qlwi = new QListWidgetItem(table->getColumns()[i]->getName(), m_ui->lstForeignTablesColumns);
        qlwi->setIcon(table->getColumns()[i]->getDataType()->getIcon());
    }
    // select the on update, on delete combo boxes
    idx = m_ui->cmbFkOnDelete->findText(m_currentForeignKey->getOnDelete());
    m_ui->cmbFkOnDelete->setCurrentIndex(idx);
    idx = m_ui->cmbFkOnUpdate->findText(m_currentForeignKey->getOnUpdate());
    m_ui->cmbFkOnUpdate->setCurrentIndex(idx);
    m_ui->btnAddForeignKey->setIcon(IconFactory::getApplyIcon());
}

void NewTableForm::updateDefaultValuesTableHeader()
{
    m_ui->tableStartupValues->setColumnCount(m_table->getColumns().count());
    for(int i=0; i<m_table->getColumns().count(); i++)
    {
        QTableWidgetItem *columnHeaderItem = new QTableWidgetItem(m_table->getColumns()[i]->getName());
        columnHeaderItem->setIcon(m_table->getColumns()[i]->getDataType()->getIcon());
        columnHeaderItem->setTextAlignment(Qt::AlignVCenter);

        m_ui->tableStartupValues->setHorizontalHeaderItem(i, columnHeaderItem);
    }
}

void NewTableForm::onAddNewDefaultRow()
{
    int curRowC = m_ui->tableStartupValues->rowCount();
    m_ui->tableStartupValues->insertRow(curRowC);
    for(int i=0; i<m_table->getColumns().count(); i++)
    {
        QWidget *defWidget = m_table->getColumns()[i]->getDataType()->getDefaultsTableWidget();
        if(defWidget != 0)
        {
            m_ui->tableStartupValues->setCellWidget(curRowC, i, defWidget);
        }
    }
}

void NewTableForm::backupDefaultValuesTable()
{
    QHash<QString, QVector<QString> > backupData;
    for(int i=0; i<m_ui->tableStartupValues->columnCount(); i++)
    {
        QVector <QString> columnI;
        for(int j=0; j<m_ui->tableStartupValues->rowCount(); j++)
        {
            if(m_ui->tableStartupValues->item(j,i))
            {
                columnI.append(m_ui->tableStartupValues->item(j,i)->text());
                m_ui->tableStartupValues->item(j,i)->setText("");
            }
            else
            {
                columnI.append("");
            }            
        }
        backupData[m_ui->tableStartupValues->horizontalHeaderItem(i)->text()] = columnI;
    }
    m_startupSaves = backupData;
}

void NewTableForm::restoreDefaultValuesTable()
{
    QHashIterator<QString, QVector <QString> > it(m_startupSaves);
    while(it.hasNext())
    {
        it.next();
        QString colName = it.key();
        // the value below is determined from the last operation
        QString finalColumnName = colName;
        if(m_columnOperation == 3 && colName == m_oldColumnName)
        {
            m_oldColumnName = "";
            finalColumnName = m_newColumnName;
        }
        QVector <QString> columnI = it.value();
        int foundColumn = -1;
        int hc = m_ui->tableStartupValues->horizontalHeader()->count();
        for(int i=0; i<hc; i++)
        {
            QTableWidgetItem *headerI = m_ui->tableStartupValues->horizontalHeaderItem(i);
            if(headerI && (headerI->text() == finalColumnName))
            {
                for(int j=0; j<columnI.size(); j++)
                {
                    if(m_ui->tableStartupValues->item(j,i))
                    {
                        m_ui->tableStartupValues->item(j,i)->setText(columnI[j]);
                    }
                    else
                    {
                        QTableWidgetItem* newItem = new QTableWidgetItem(columnI[j]);
                        m_ui->tableStartupValues->setItem(j, i, newItem);
                    }
                }
            }
        }
    }
    m_columnOperation = -1;
}


void NewTableForm::keyPressEvent(QKeyEvent *evt)
{
    if(evt->key() == Qt::Key_Return)
    {
        if(m_ui->tabWidget->currentIndex() == 0)    // Columns page
        {
            if(m_ui->txtNewColumnName->hasFocus() || m_ui->cmbNewColumnType->hasFocus() || m_ui->chkPrimary->hasFocus())
            {
                onAddColumn();
            }
        }
    }
}

void NewTableForm::onBtnUpdateTableWithDefaultValues()
{
    QVector <QVector <QString > > values;
    for(int i=0; i<m_ui->tableStartupValues->rowCount(); i++)
    {
        QVector<QString> rowI;
        for(int j=0; j<m_ui->tableStartupValues->columnCount(); j++)
        {
            rowI.append(m_ui->tableStartupValues->item(i,j)->text());
        }
        values.append(rowI);
    }

    m_table->setDefaultValues(values);
}

void NewTableForm::onSaveStartupValuestoCSV()
{
    QVector<QString> rows;

    for(int i=0; i<m_ui->tableStartupValues->rowCount(); i++)
    {
        QString rowI;
        for(int j=0; j<m_ui->tableStartupValues->columnCount(); j++)
        {
            rowI += m_ui->tableStartupValues->item(i,j)->text();
            if(j<m_ui->tableStartupValues->columnCount() - 1)
            {
                rowI += ",";
            }
        }
        rows.append(rowI);
    }
    QString fileName = QFileDialog::getSaveFileName(this,  tr("Save startup values"), "", tr("CSV files (*.csv)"));
    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return;
    }
    QTextStream out(&file);
    for(int i=0; i<rows.size(); i++)
    {
        out << rows[i] << "\n";
    }
}

void NewTableForm::onLoadStartupValuesFromCSV()
{
    QString fileName = QFileDialog::getOpenFileName(this,  tr("Load startup values"), "", tr("CSV files (*.csv)"));
    if(fileName.length() == 0)
    {
        return;
    }

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    m_ui->tableStartupValues->clearContents();
    m_ui->tableStartupValues->setRowCount(0);

    int i = 0;
    QTextStream in(&file);
    while (!in.atEnd())
    {
        m_ui->tableStartupValues->insertRow(i);
        QString line = in.readLine();
        int j = 0, cc = 0;
        QString cellV = "";
        while(j<line.length())
        {
            if(line.at(j) !=',')
            {
                cellV += line.at(j);
            }
            else
            {
                QTableWidgetItem *twl = new QTableWidgetItem(cellV);
                m_ui->tableStartupValues->setItem(i, cc, twl);
                cc ++;
                cellV = "";
            }
            j++;
        }
        // last value
        QTableWidgetItem *twl = new QTableWidgetItem(cellV);
        m_ui->tableStartupValues->setItem(i, cc, twl);
        i++;
    }
}


void NewTableForm::onStorageEngineChange(QString name)
{
    const QVector<AbstractStorageEngine*>& storageEngines = m_engineProviders->getStorageEngines();
    for(int i=0; i<storageEngines.size(); i++)
    {
        if(storageEngines.at(i)->name() == name)
        {
            m_currentStorageEngine = storageEngines.at(i);
        }
    }

    m_table->setStorageEngine(m_currentStorageEngine);

    populateIndexTypesDependingOnStorageEngine();
    enableForeignKeysDependingOnStorageEngine();
}
