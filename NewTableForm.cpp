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

#include <QMessageBox>

// the positions of various items in the columns view, used for icon retrieval mostly
const int COL_POS_PK = 0;
const int COL_POS_NM = 1;
const int COL_POS_DT = 2;

NewTableForm::NewTableForm(DatabaseEngine* db, Project* prj, QWidget *parent) : QWidget(parent), m_ui(new Ui::NewTableForm),
    m_dbEngine(db), m_project(prj), m_table(new Table()), m_currentColumn(0), m_currentIndex(0), m_foreignTable(0)
{
    m_ui->setupUi(this);

    const QVector<UserDataType*>& dts = m_project->getWorkingVersion()->getDataTypes();
    for(int i=0; i<dts.size(); i++)
    {
        m_ui->cmbNewColumnType->addItem(dts[i]->getIcon(), dts[i]->getName());
    }

    m_ui->lstColumns->header()->resizeSection(0, 50);
    m_ui->cmbNewColumnType->setCurrentIndex(-1);

    // fill in the index types combo box
    const QStringList& indexTypes = db->getIndextypeProvider()->getIndexTypes();
    QStringList::const_iterator it = indexTypes.constBegin();
    while(it != indexTypes.constEnd())
    {
        m_ui->cmbIndexType->addItem(*it);
        it ++;
    }

    // create the foreign keys screen
    const QVector<Table*>& tables = m_project->getWorkingVersion()->getTables();
    for(int i=0; i<tables.size(); i++)
    {
        m_ui->cmbForeignTables->addItem(tables[i]->getName());
    }

    m_ui->btnAdvanced->hide();
    m_ui->tabWidget->setCurrentIndex(0);
}

NewTableForm::~NewTableForm()
{
    delete m_ui;
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

    m_ui->txtTableName->setText(m_table->getName());
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

    for(int i=0; i < m_table->getColumns().size(); i++)
    {
        if(m_table->getColumn(i)->getName() == m_ui->txtNewColumnName->text())
        {
            QMessageBox::critical (this, tr("Error"), tr("Please use a unique name for each of the columns"), QMessageBox::Ok);
            return;
        }
    }

    if(m_currentColumn)     // we are working on a column
    {
        m_currentColumn->setName(m_ui->txtNewColumnName->text());
        m_currentColumn->setPk(m_ui->chkPrimary->checkState());
        m_currentColumn->setDataType(m_project->getWorkingVersion()->getDataType(m_ui->cmbNewColumnType->currentText()));

        m_currentColumn->getLocation()->setText(1, m_currentColumn->getName());
        if(m_ui->chkPrimary->checkState())
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
        m_ui->btnAdd->setIcon(IconFactory::getAddIcon());
    }
    else                    // we are not working on a column, but adding a new one
    {
        UserDataType* colsDt = m_project->getWorkingVersion()->getDataType(m_ui->cmbNewColumnType->currentText());
        Column* col = new Column(m_ui->txtNewColumnName->text(), colsDt, m_ui->chkPrimary->checkState()) ;
        QTreeWidgetItem* item = createTWIForColumn(col);
        m_ui->lstColumns->addTopLevelItem(item);
        m_table->addColumn(col);

        col->setLocation(item);
    }

    m_ui->txtNewColumnName->setText("");
    m_ui->cmbNewColumnType->setCurrentIndex(-1);

    populateColumnsForIndices();

}

void NewTableForm::onCancelColumnEditing()
{
    m_ui->txtNewColumnName->clear();
    m_ui->cmbNewColumnType->setCurrentIndex(-1);
    m_currentColumn = 0;
    m_ui->btnAdd->setIcon(IconFactory::getAddIcon());
}

void NewTableForm::onDeleteColumn()
{
    const Column* currentColumn = m_table->getColumn(m_ui->lstColumns->currentItem()->text(1));
    const Index* usedIn = m_table->isColumnUsedInIndex(currentColumn);
    if(usedIn != 0)
    {
        QMessageBox::critical (this, tr("Error"), tr("This column cannot be deleted since it's used in an index. Delete this index first: ") + usedIn->getName(), QMessageBox::Ok);
        return;
    }
    delete m_ui->lstColumns->currentItem();
    onCancelColumnEditing();
    m_table->removeColumn(const_cast<Column*>(currentColumn));
    m_currentColumn = 0;
    populateColumnsForIndices();
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
        }
    }
}

void NewTableForm::onItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{

}

/**
 * Called when a column is selected.
 */
void NewTableForm::onItemSelected(QTreeWidgetItem* current, int column)
{
    QModelIndex x = m_ui->lstColumns->currentIndex();
    m_currentColumn = m_table->getColumn(x.row());

    m_ui->txtNewColumnName->setText(m_currentColumn->getName());
    m_ui->cmbNewColumnType->setCurrentIndex(m_project->getWorkingVersion()->getDataTypeIndex(m_currentColumn->getDataType()->getName()));
    m_ui->chkPrimary->setChecked(m_currentColumn->isPk());

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
        QMessageBox::critical (this, tr("Error"), tr("Please specify a name"), QMessageBox::Ok);
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

        resetIndexGui();

        m_currentIndex = 0;
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

        resetIndexGui();

        m_currentIndex = 0;
    }
}

void NewTableForm::resetIndexGui()
{
    m_ui->lstSelectedColumnsForIndex->clear();
    populateColumnsForIndices();
    m_ui->cmbIndexType->setCurrentIndex(0);
    m_ui->txtNewIndexName->setText("");
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
    item->setIcon(0, clocalColumn->getDataType()->getIcon());

    m_ui->lstForeignKeyAssociations->addTopLevelItem(item);
}

void NewTableForm::onRemoveForeignKeyAssociation()
{
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

