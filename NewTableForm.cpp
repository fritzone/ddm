#include "NewTableForm.h"
#include "ui_NewTableForm.h"
#include "IconFactory.h"
#include "db_DatabaseEngine.h"
#include "Project.h"
#include "Version.h"
#include "Configuration.h"
#include "core_UserDataType.h"
#include "core_Column.h"
#include "Index.h"
#include "core_Table.h"
#include "MainWindow.h"
#include "ForeignKey.h"
#include "StartupValuesHelper.h"
#include "db_AbstractSQLGenerator.h"
#include "SqlHighlighter.h"
#include "InjectSqlDialog.h"
#include "SqlNamesValidator.h"
#include "Workspace.h"
#include "ContextMenuCollection.h"
#include "ClipboardFactory.h"
#include "db_Codepage.h"
#include "db_AbstractDTSupplier.h"
#include "IssueManager.h"
#include "VersionGuiElements.h"
#include "gui_HelpWindow.h"
#include "core_ConnectionManager.h"
#include "strings.h"
#include "WidgetForSpecificProperties.h"
#include "SpInstance.h"
#include "GuiElements.h"

#include <QMessageBox>
#include <QHashIterator>
#include <QHash>
#include <QKeyEvent>
#include <QtGui>
#include <QList>
#include <QMenu>

// the positions of various items in the columns view, used for icon retrieval mostly
const int COL_POS_PK = 0;
const int COL_POS_NM = 1;
const int COL_POS_DT = 2;

NewTableForm::NewTableForm(DatabaseEngine* db, Project* prj, Version* v, QWidget *parent, bool newTable) : SourceCodePresenterWidget(v, parent),
    m_ui(new Ui::NewTableForm),
    m_dbEngine(db), m_project(prj), m_table(0),
    m_currentColumn(0), m_currentIndex(0), m_foreignTable(0), m_currentForeignKey(0), m_foreignKeySelected(false),
    m_engineProviders(0), m_wspForIndex(0), m_wspForColumn(0), m_version(v)
{
    m_ui->setupUi(this);


    qDebug() << "New table form: " << v->getVersionText();
    m_ui->lstSelectedColumnsForIndex->setHeaderHidden(false);

    // now set up the Column list and the context menus for the Column list
    lstColumns = new ContextMenuEnabledTreeWidget();
    lstColumns->setObjectName(QString::fromUtf8("lstColumns"));
    lstColumns->setRootIsDecorated(false);
    lstColumns->setItemsExpandable(false);
    lstColumns->setAnimated(false);
    lstColumns->setExpandsOnDoubleClick(false);
    lstColumns->header()->setDefaultSectionSize(150);
    m_ui->columnListLayout->addWidget(lstColumns);
    QTreeWidgetItem *header = lstColumns->headerItem();
    header->setText(3, tr("SQL Type"));
    header->setText(2, tr("Type"));
    header->setText(1, tr("Name"));
    header->setText(0, tr("PK"));
    ContextMenuHandler* contextMenuHandler = new ContextMenuHandler();
    lstColumns->setItemDelegate(new ContextMenuDelegate(contextMenuHandler,lstColumns));
    // then connect the signals
    QObject::connect(lstColumns, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(onSelectColumn(QTreeWidgetItem*,int)));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_CopyColumn(), SIGNAL(triggered()), this, SLOT(onCopyColumn()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_PasteColumn(), SIGNAL(triggered()), this, SLOT(onPasteColumn()));

    highlighter = new SqlHighlighter(m_ui->txtSql->document(),
                                     Workspace::getInstance()->currentProjectsEngine()->getKeywords(),
                                     Workspace::getInstance()->currentProjectsEngine()->getDTSupplier()->numericTypes(),
                                     Workspace::getInstance()->currentProjectsEngine()->getDTSupplier()->booleanTypes(),
                                     Workspace::getInstance()->currentProjectsEngine()->getDTSupplier()->textTypes(),
                                     Workspace::getInstance()->currentProjectsEngine()->getDTSupplier()->blobTypes(),
                                     Workspace::getInstance()->currentProjectsEngine()->getDTSupplier()->dateTimeTypes(),
                                     Workspace::getInstance()->currentProjectsEngine()->getDTSupplier()->miscTypes(),
                                     v->getTables());

    const QVector<UserDataType*>& dts = v->getDataTypes();
    for(int i=0; i<dts.size(); i++)
    {
        m_ui->cmbNewColumnType->addItem(IconFactory::getIconForDataType(dts[i]->getType()), dts[i]->getName());
    }

    lstColumns->header()->resizeSection(0, 50);
    m_ui->cmbNewColumnType->setCurrentIndex(-1);

    m_ui->tabWidget->setCurrentIndex(0);

    // next two: don't change the order. This way the remove is done from the end
    if(prj->oopProject())
    {
        m_ui->tabWidget->removeTab(3);
    }

    m_ui->btnImportValues->hide();

    m_ui->txtTableName->setValidator(m_nameValidator = new SqlNamesValidator());
    m_ui->txtNewColumnName->setValidator(m_nameValidator);
    m_ui->txtForeignKeyName->setValidator(m_nameValidator);
    m_ui->txtNewIndexName->setValidator(m_nameValidator);

    if(newTable)
    {
        m_table = new Table(m_version, QUuid::createUuid().toString(), 0);
        m_table->initializeFor(db, QUuid(uidTable));

        Workspace::getInstance()->onSaveNewTable(m_table);
        m_ui->txtTableName->setText(m_table->getName());

        resetForeignTablesCombo();
        prepareSpsTabs();
    }

    m_signalMapperForCombosInColumns = new QSignalMapper(this);
    m_ui->buttons->hide();
    m_ui->frameForUnlockButton->hide();

}

NewTableForm::~NewTableForm()
{
    delete m_ui;
}

void NewTableForm::resetForeignTablesCombo()
{
    m_ui->cmbForeignTables->clear();

    // create the foreign keys screen
    const QVector<Table*>& tables = m_version->getTables();
    for(int i=0; i<tables.size(); i++)
    {
        if(tables[i]->getName() != m_table->getName())
        {
            // at this stage we can put in all the tables, this is supposed to be a new table
            m_ui->cmbForeignTables->addItem(tables[i]->getName());
        }
    }
    m_ui->cmbForeignTables->setCurrentIndex(-1);

    m_ui->lstForeignKeyAssociations->clear();
    m_ui->lstForeignTablesColumns->clear();
    m_ui->lstLocalColumn->clear();
    m_ui->txtForeignKeyName->clear();
}

ContextMenuEnabledTreeWidgetItem* NewTableForm::createTWIForForeignKey(const ForeignKey* fk)
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

    ContextMenuEnabledTreeWidgetItem* item = new ContextMenuEnabledTreeWidgetItem((ContextMenuEnabledTreeWidgetItem*)0, a);
    item->setIcon(0, IconFactory::getForeignKeyIcon());
    return item;
}

void NewTableForm::setTypeComboBoxForColumnItem(ContextMenuEnabledTreeWidgetItem *item, Column* c)
{
    QComboBox* cmbColumnType = new QComboBox(0);
    cmbColumnType->setAutoFillBackground(true);
    const QVector<UserDataType*>& dts = m_version->getDataTypes();
    int s = -1;

    // TODO: this is just horrible. FIX! FIX! FIX!
    if(item->backgroundColor(0) == Qt::lightGray)
    {
        cmbColumnType->addItem(IconFactory::getIconForDataType(c->getDataType()->getType()),c->getDataType()->getName());
        lstColumns->setItemWidget(item, 2, cmbColumnType);
        return;
    }
    for(int i=0; i<dts.size(); i++)
    {
        cmbColumnType->addItem(IconFactory::getIconForDataType(dts[i]->getType()), dts[i]->getName(), QVariant(c->getName()));
        if(dts[i]->getName() == c->getDataType()->getName()) s = i;
    }
    lstColumns->setItemWidget(item, 2, cmbColumnType);
    cmbColumnType->setCurrentIndex(s);
    connect(cmbColumnType, SIGNAL(activated(QString)), m_signalMapperForCombosInColumns, SLOT(map()));
    m_signalMapperForCombosInColumns->setMapping(cmbColumnType, c->getName());
    // TODO: This is pretty ugly, it will call the slot several times. find a better location for this in the code
    connect(m_signalMapperForCombosInColumns, SIGNAL(mapped(const QString&)), this, SLOT(onDatatypeSelectedForColumnInList(const QString&)));
}

void NewTableForm::onDatatypeSelectedForColumnInList(const QString& b)
{
    // find the column, the name is b
    Column* c = m_table->getColumn(b);
    if(!c)
    {
        return;
    }
    QComboBox* cmb = static_cast<QComboBox*>(lstColumns->itemWidget(c->getLocation(), 2));

    if(!cmb)
    {
        return;
    }
    const QVector<UserDataType*>& dts = m_version->getDataTypes();
    int i;
    for(i=0; i<dts.size(); i++)
    {
        if(dts.at(i)->getName() == cmb->currentText()) break;
    }
    c->setDataType(dts.at(i));
    c->getLocation()->setText(3, c->getDataType()->sqlAsString());

    if(m_currentColumn)
    {
        m_ui->cmbNewColumnType->setCurrentIndex(i);
    }

    // TODO: fix the index GUI too

}

ContextMenuEnabledTreeWidgetItem* NewTableForm::createTWIForColumn(const Column* col, ContextMenuEnabledTreeWidgetItem* parent )
{
    QStringList a("");
    a.append(col->getName());
    a.append(col->getDataType()->getName());
    a.append(col->getDataType()->sqlAsString());

    ContextMenuEnabledTreeWidgetItem* item = new ContextMenuEnabledTreeWidgetItem(parent, a);
    if(col->isPk())
    {
        item->setIcon(COL_POS_PK, IconFactory::getKeyIcon());
    }
    //item->setIcon(COL_POS_DT, col->getDataType()->getIcon());
    item->setPopupMenu(ContextMenuCollection::getInstance()->getColumnPopupMenu());
    QVariant var(col->getName());
    item->setData(0, Qt::UserRole, var);
    return item;
}

//void NewTableForm::prepareColumnsListWithParentItems(const Table* ctable)
//{
//    // first step: go back upwards, recursively
//    if(ctable->getParent())
//    {
//        prepareColumnsListWithParentItems(ctable->getParent());
//    }
//    // then create the column list
//    if(ctable)
//    {
//        const QVector<Column*>& columns = ctable->getColumns();
//        for(int i=0; i<columns.count(); i++)
//        {
//            static QBrush grayBrush((QColor(Qt::gray)));
//            ContextMenuEnabledTreeWidgetItem* item = createTWIForColumn(columns[i]);
//            item->setBackground(0, grayBrush);
//            lstColumns->addTopLevelItem(item);
//            columns[i]->setLocation(item);
//            setTypeComboBoxForColumnItem(item, columns[i]);
//        }
//    }
//}

void NewTableForm::selectTab(int i)
{
    m_ui->tabWidget->setCurrentIndex(i);
}

void NewTableForm::populateTable(const Table *table, bool parentTab)
{
    // first step: set the columns
    const QVector<Column*>& columns = table->getColumns();
    for(int i=0; i<columns.count(); i++)
    {
        ContextMenuEnabledTreeWidgetItem* item = createTWIForColumn(columns[i]);
        lstColumns->addTopLevelItem(item);
        columns[i]->setLocation(item);
        if(parentTab)
        {
            item->setBackground(0, QBrush(Qt::lightGray));
            item->setBackground(1, QBrush(Qt::lightGray));
            item->setBackground(2, QBrush(Qt::lightGray));
            item->setBackground(3, QBrush(Qt::lightGray));
        }
        setTypeComboBoxForColumnItem(item, columns[i]);

    }

    lstColumns->resizeColumnToContents(0);
    lstColumns->resizeColumnToContents(1);
    lstColumns->resizeColumnToContents(2);
    lstColumns->resizeColumnToContents(3);

    // second step: set up the indices
    const QVector<Index*>& indices = table->getIndices();
    for(int i=0; i<indices.count(); i++)
    {
        ContextMenuEnabledTreeWidgetItem* item = createTWIForIndex(indices[i]);
        m_ui->lstIndices->addTopLevelItem(item);
        indices[i]->setLocation(item);
        if(parentTab)
        {
            item->setBackground(0, QBrush(Qt::lightGray));
            item->setBackground(1, QBrush(Qt::lightGray));
            item->setBackground(2, QBrush(Qt::lightGray));
            item->setBackground(3, QBrush(Qt::lightGray));
        }
    }
    resetIndexGui();
    m_ui->lstIndices->resizeColumnToContents(0);
    m_ui->lstIndices->resizeColumnToContents(1);
    m_ui->lstIndices->resizeColumnToContents(2);
    m_ui->lstIndices->resizeColumnToContents(3);

    if(parentTab && !Configuration::instance().allowForeignKeyPropagation())
    {
        return;
    }

    // third step: set up the foreign keys
    const QVector<ForeignKey*> & fks = table->getFks();
    for(int i=0; i<fks.size(); i++)
    {
        ContextMenuEnabledTreeWidgetItem* item = createTWIForForeignKey(fks[i]);
        m_ui->lstForeignKeys->addTopLevelItem(item);
        fks[i]->setLocation(item);
        if(parentTab)
        {
            item->setBackground(0, QBrush(Qt::lightGray));
            item->setBackground(1, QBrush(Qt::lightGray));
            item->setBackground(2, QBrush(Qt::lightGray));
            item->setBackground(3, QBrush(Qt::lightGray));
            item->setBackground(4, QBrush(Qt::lightGray));
            item->setBackground(5, QBrush(Qt::lightGray));
        }
    }

    m_ui->lstForeignKeys->resizeColumnToContents(0);
    m_ui->lstForeignKeys->resizeColumnToContents(1);
    m_ui->lstForeignKeys->resizeColumnToContents(2);
    m_ui->lstForeignKeys->resizeColumnToContents(3);
    m_ui->lstForeignKeys->resizeColumnToContents(4);
    m_ui->lstForeignKeys->resizeColumnToContents(5);

    // set the default values
    if(! Workspace::getInstance()->currentProjectIsOop())
    {
        updateDefaultValuesTableHeader();
        for(int i=0; i<table->getDefaultValues().size(); i++)
        {
            const QVector<QString>& rowI = table->getDefaultValues()[i];
            m_ui->tableStartupValues->insertRow(i);
            for(int j=0; j<rowI.size(); j++)
            {
                QTableWidgetItem* newItem = new QTableWidgetItem(rowI[j]);
                m_ui->tableStartupValues->setItem(i, j, newItem);
            }
        }
    }
}

void NewTableForm::setTable(Table *table)
{
    m_table = table;
    const Table* ctab = table->parent();
    QStack<const Table*> tabStack;
    while(ctab)
    {
        tabStack.push(ctab);
        ctab = ctab->parent();
    }

    while(!tabStack.empty())
    {
        populateTable(tabStack.at(0), true);
        tabStack.pop_front();
    }

    populateTable(table, false);

    m_ui->txtTableName->setText(table->getName());
    m_ui->txtDescription->setText(table->getDescription());

    if(m_table) resetForeignTablesCombo();
    prepareSpsTabs();

    m_ui->btnUndelete->hide();

    // TODO: this is code duplication from the other forms
    if(!m_table->wasLocked())
    {
        if(m_table->isDeleted())
        {
            m_ui->frameForUnlockButton->show();
            m_ui->btnLock->hide();
            m_ui->btnUndelete->show();
        }
        else
        {
            m_ui->frameForUnlockButton->hide();
        }
    }
    else
    {
        m_ui->frameForUnlockButton->show();
        if(m_table->isLocked())
        {
            m_ui->btnLock->setIcon(IconFactory::getLockedIcon());
            m_ui->btnLock->blockSignals(true);
            m_ui->btnLock->setChecked(false);
            m_ui->btnLock->blockSignals(false);
            m_ui->grpContent->setEnabled(false);
            m_ui->btnLock->setToolTip(QObject::tr("This table is <b>locked</b>. Click this button to unlock it."));
        }
        else
        {
            m_ui->btnLock->setIcon(IconFactory::getUnLockedIcon());
            m_ui->btnLock->blockSignals(true);
            m_ui->btnLock->setChecked(true);
            m_ui->btnLock->blockSignals(false);
            m_ui->grpContent->setEnabled(true);
            m_ui->btnLock->setToolTip(QObject::tr("This table is <b>unlocked</b>. Click this button to lock it."));
        }

        if(m_table->isDeleted())
        {
            m_ui->btnLock->hide();
            m_ui->btnUndelete->show();
        }
    }

}

void NewTableForm::focusOnName()
{
    m_ui->txtTableName->setFocus();
}

void NewTableForm::focusOnNewColumnName()
{
    m_ui->txtNewColumnName->setFocus();
}

QString NewTableForm::getTableName() const
{
    return m_ui->txtTableName->text();
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

void NewTableForm::updateIssues()
{
    m_version->validateVersion(false);
}

void NewTableForm::onAddColumn()
{
    if(m_ui->cmbNewColumnType->currentIndex() == -1)
    {
        QMessageBox::critical (this, tr("Error"), tr("Please select a column type."), QMessageBox::Ok);
        return;
    }

    if(m_ui->txtNewColumnName->text().length() == 0)
    {
        QMessageBox::critical (this, tr("Error"), tr("Please specify a column name."), QMessageBox::Ok);
        m_ui->txtNewColumnName->setFocus();
        return;
    }

    backupDefaultValuesTable();
    if(m_currentColumn)     // we are working on a column
    {
        // check if we have changed the datatype... in this case check that:
        if(m_ui->cmbNewColumnType->currentText() != m_currentColumn->getDataType()->getName())
        {
            // in affirmative case check that:
            // 1. this column is not referencing any other columns in the DB through a foreign key
            QVector<ForeignKey*> fks = m_table->columnParticipatesInForeignKey(m_currentColumn);
            if(fks.size() > 0)
            {
                QString s = "";
                for(int i=0; i<fks.size(); i++)
                {
                    s += "\n-" + fks.at(i)->getName();
                }
                QMessageBox::critical (this, tr("Error"), tr("Cannot change the type of the columns since it's participating in Foreign Key. Please change the following foreign keys and try again.")+ s, QMessageBox::Ok);
                return;
            }
            // 2. no other column is referencing this column in the DB through a foreign key from all the tables in the version
            QVector<Table*> otherTablesReferencingThiscolumn = m_version->getTablesReferencingAColumnThroughForeignKeys(m_currentColumn);
            if(otherTablesReferencingThiscolumn.size() > 0)
            {
                QString s = "";
                for(int i=0; i<otherTablesReferencingThiscolumn.size(); i++)
                {
                    s += "\n-" + otherTablesReferencingThiscolumn.at(i)->getName();
                }
                QMessageBox::critical (this, tr("Error"), tr("Cannot change the type of the columns since it's participating in Foreign Keys. Please change the following tables and try again.")+ s, QMessageBox::Ok);
                return;
            }
        }


        if(m_currentColumn->getName() != m_ui->txtNewColumnName->text())
        {
            m_columnOperation = 3;
            m_newColumnName = m_ui->txtNewColumnName->text();
            m_oldColumnName = m_currentColumn->getName();
            m_table->tableInstancesRenameColumn(m_oldColumnName, m_newColumnName);
        }
        m_currentColumn->setName(m_ui->txtNewColumnName->text());
        m_currentColumn->setDescription(m_ui->txtColumnDescription->toPlainText());
        m_currentColumn->setPk(m_ui->chkPrimary->isChecked());

        m_currentColumn->setDataType(m_version->getDataType(m_ui->cmbNewColumnType->currentText()));

        m_currentColumn->getLocation()->setText(1, m_currentColumn->getName());
        if(m_ui->chkPrimary->isChecked())
        {
            m_currentColumn->getLocation()->setIcon(COL_POS_PK, IconFactory::getKeyIcon());
        }
        else
        {
            m_currentColumn->getLocation()->setIcon(COL_POS_PK, IconFactory::getEmptyIcon());
        }
        //m_currentColumn->getLocation()->setIcon(COL_POS_DT, m_currentColumn->getDataType()->getIcon());
        m_currentColumn->getLocation()->setText(COL_POS_DT, m_currentColumn->getDataType()->getName());

        // see if the change has done anything to the issues
        updateIssues();
        if(m_wspForColumn) m_wspForColumn->repopulateSpsOfObject(m_currentColumn);

        autoSave();
        m_currentColumn = 0;

    }
    else                    // we are not working on a column, but adding a new one
    {
        if(m_table->parentsHaveColumn(m_ui->txtNewColumnName->text()))
        {
            QMessageBox::critical (this, tr("Error"), tr("You are not allowed to update a parent tables columns from a specialized table. (If you want to enable the text fields, press the cancel button next to this button.)"), QMessageBox::Ok);
            return;
        }

        if(m_table->hasColumn(m_ui->txtNewColumnName->text()) || m_table->parentsHaveColumn(m_ui->txtNewColumnName->text()))
        {
            QMessageBox::critical (this, tr("Error"), tr("Please use a unique name for each of the columns"), QMessageBox::Ok);
            return;
        }

        UserDataType* colsDt = m_version->getDataType(m_ui->cmbNewColumnType->currentText());
        Column* col = new Column(QUuid::createUuid().toString(), m_ui->txtNewColumnName->text(), colsDt, m_ui->chkPrimary->isChecked(), colsDt->version());
        col->initializeFor(m_dbEngine, QUuid(uidColumn));
        if(m_wspForColumn) m_wspForColumn->repopulateSpsOfObject(col);

        col->setDescription(m_ui->txtColumnDescription->toPlainText());
        ContextMenuEnabledTreeWidgetItem* item = createTWIForColumn(col);
        lstColumns->addTopLevelItem(item);
        setTypeComboBoxForColumnItem(item, col);
        m_table->addColumn(col);
        // and update the table instances of the table, by adding a new table instance
        m_table->tableInstancesAddColumn(col);

        col->setLocation(item);
        m_ui->txtNewColumnName->setFocus();
        updateIssues();
        autoSave();
//        Workspace::getInstance()->workingVersion()->checkIssuesOfNewColumn(col, m_table);

    }

    m_ui->txtNewColumnName->setText("");
    m_ui->cmbNewColumnType->setCurrentIndex(-1);
    m_ui->txtColumnDescription->setText("");
    m_ui->chkPrimary->setChecked(false);
    m_ui->btnAddColumn->setIcon(IconFactory::getAddIcon());

    lstColumns->resizeColumnToContents(0);
    lstColumns->resizeColumnToContents(1);
    lstColumns->resizeColumnToContents(2);
    lstColumns->resizeColumnToContents(3);

    populateColumnsForIndices();
    updateDefaultValuesTableHeader();
    restoreDefaultValuesTable();

    autoSave();
    prepareSpsTabsForColumn(0);
    m_ui->grpColumnDetails->setTitle(tr(" New column "));

}

void NewTableForm::onCancelColumnEditing()
{
    m_currentColumn = 0;
    m_ui->txtNewColumnName->clear();
    m_ui->cmbNewColumnType->setCurrentIndex(-1);
    m_ui->chkPrimary->setChecked(false);
    m_ui->btnAddColumn->setIcon(IconFactory::getAddIcon());
    toggleColumnFieldDisableness(false);
    m_ui->grpColumnDetails->setTitle(tr(" New column "));
    m_ui->txtColumnDescription->setText("");
}

void NewTableForm::onDeleteColumn()
{
    QTreeWidgetItem* currentItem = lstColumns->currentItem() == 0?
                m_currentColumn==0?0:m_currentColumn->getLocation():lstColumns->currentItem();

    if(currentItem == 0)
    {
        QMessageBox::critical (this, tr("Error"), tr("Please make sure you have a column selected for deletion."), QMessageBox::Ok);
        return;
    }

    if(!m_table->hasColumn(currentItem->text(1)) && m_table->parentsHaveColumn(currentItem->text(1)))
    {
        QMessageBox::critical (this, tr("Error"), tr("You cannot delete the columns of a parent table from a sibling table. Go to the parent table to do this."), QMessageBox::Ok);
        return;
    }

    Column* currentColumn = m_table->getColumn(currentItem->text(1));
    if(!currentColumn)
    {
        QMessageBox::critical (this, tr("Error"), tr("[Internal Error 001] The selected column cannot be found in the table."), QMessageBox::Ok);
        return;
    }

    Index* usedIn = m_table->isColumnUsedInIndex(currentColumn);
    if(usedIn != 0)
    {
        QMessageBox::critical (this, tr("Error"), tr("This column cannot be deleted since it's used in an index. Delete this index first: ") + usedIn->getName(), QMessageBox::Ok);
        return;
    }
    backupDefaultValuesTable();
    delete currentItem;
    onCancelColumnEditing();
    m_table->removeColumn(currentColumn);
    m_table->tableInstancesRemoveColumn(currentColumn);

    Workspace::getInstance()->workingVersion()->getGui()->cleanupOrphanedIssueTableItems();
    m_currentColumn = 0;
    populateColumnsForIndices();
    updateDefaultValuesTableHeader();
    restoreDefaultValuesTable();

    updateIssues();

    autoSave();
}

void NewTableForm::onMoveColumnDown()
{
    if(lstColumns->selectedItems().size() > 0)
    {
        if(!m_table->hasColumn(lstColumns->currentItem()->text(1)))
        {
            QMessageBox::critical (this, tr("Error"), tr("You cannot move the columns of a parent table from a sibling table. Go to the parent table to do this."), QMessageBox::Ok);
            return;
        }

        QModelIndex x = lstColumns->currentIndex();
        int rc = x.row();
        if(rc < lstColumns->topLevelItemCount() - 1)
        {
            m_table->moveColumnDown(rc);

            Column* col = m_table->getColumn(lstColumns->currentItem()->text(1));
            ContextMenuEnabledTreeWidgetItem* item = createTWIForColumn(col);
            col->setLocation(item);
            delete lstColumns->currentItem();
            lstColumns->insertTopLevelItem(rc + 1, item);
            lstColumns->setCurrentItem(item);
            setTypeComboBoxForColumnItem(item, col);
            finalizeColumnMovement();
        }
    }
}

void NewTableForm::finalizeColumnMovement()
{
    populateColumnsForIndices();
    // TODO: repatriate the columns from the exchanged one
    backupDefaultValuesTable();
    updateDefaultValuesTableHeader();
    restoreDefaultValuesTable();
    autoSave();
}

void NewTableForm::onMoveColumnUp()
{
    if(lstColumns->selectedItems().size() > 0)
    {
        if(!m_table->hasColumn(lstColumns->currentItem()->text(1)))
        {
            QMessageBox::critical (this, tr("Error"), tr("You cannot move the columns of a parent table from a sibling table. Go to the parent table to do this."), QMessageBox::Ok);
            return;
        }
        QModelIndex x = lstColumns->currentIndex();
        int rc = x.row();
        if(rc > m_table->getTotalParentColumnCount())
        {
            m_table->moveColumnUp(rc - m_table->getTotalParentColumnCount());

            Column* col = m_table->getColumn(lstColumns->currentItem()->text(1));
            ContextMenuEnabledTreeWidgetItem* item = createTWIForColumn(col);
            col->setLocation(item);
            delete lstColumns->currentItem();
            lstColumns->insertTopLevelItem(rc - 1, item);
            lstColumns->setCurrentItem(item);
            setTypeComboBoxForColumnItem(item, col);
            finalizeColumnMovement();
        }
    }
}

void NewTableForm::toggleColumnFieldDisableness(bool a)
{
    m_ui->txtNewColumnName->setDisabled(a);
    m_ui->cmbNewColumnType->setDisabled(a);
    m_ui->chkPrimary->setDisabled(a);
    m_ui->txtColumnDescription->setDisabled(a);
}

void NewTableForm::showColumn(Column * c)
{
    if(!m_table->hasColumn(c->getName())) return;
    m_ui->txtNewColumnName->setText(c->getName());
    m_ui->cmbNewColumnType->setCurrentIndex(m_version->getDataTypeIndex(c->getDataType()->getName()));
    m_ui->chkPrimary->setChecked(c->isPk());
    m_ui->txtColumnDescription->setText(c->getDescription());

    m_ui->btnAddColumn->setIcon(IconFactory::getApplyIcon());
    m_ui->btnCancelColumnEditing->show();

    m_ui->grpColumnDetails->setTitle(" Column details: " + c->getName());

    prepareSpsTabsForColumn(c);

}

/**
 * Called when a column is selected. onColumnSelect onSelectColumn
 */
void NewTableForm::onSelectColumn(QTreeWidgetItem* current, int)
{
    QModelIndex x = lstColumns->currentIndex();
    m_currentColumn = m_table->getColumn(current->text(1));

    if(m_currentColumn == 0)
    {
        Column* colFromParent = m_table->getColumnFromParents(current->text(1));
        if(colFromParent == 0)
        {
            return; // shouldn't happen
        }
        showColumn(colFromParent);
        toggleColumnFieldDisableness(true);
    }
    else
    {
        showColumn(m_currentColumn);
        toggleColumnFieldDisableness(false);
    }
}

void NewTableForm::populateColumnsForIndices()
{
    m_ui->lstAvailableColumnsForIndex->clear();
    for(int i=0; i<m_table->fullColumns().size(); i++)
    {
        if(m_ui->lstSelectedColumnsForIndex->topLevelItemCount() == 0)
        {
            QListWidgetItem* qlwi = new QListWidgetItem(m_table->fullColumns()[i], m_ui->lstAvailableColumnsForIndex);
            Column* col = m_table->getColumn(m_table->fullColumns()[i]);
            if(col)
            {
                qlwi->setIcon(IconFactory::getIconForDataType(col->getDataType()->getType()));
            }
            else
            {
                col = m_table->getColumnFromParents(m_table->fullColumns()[i]);
                if(col)
                {
                    qlwi->setIcon(IconFactory::getIconForDataType(col->getDataType()->getType()));
                }
            }
        }
        else
        {
            for(int j = 0; j<m_ui->lstSelectedColumnsForIndex->topLevelItemCount(); j++)
            {
                if(m_ui->lstSelectedColumnsForIndex->topLevelItem(j)->text(0) != m_table->getColumns()[i]->getName())
                {
                    QListWidgetItem* qlwi = new QListWidgetItem(m_table->fullColumns()[i], m_ui->lstAvailableColumnsForIndex);
                    // TODO: duplication with the one above
                    Column* col = m_table->getColumn(m_table->fullColumns()[i]);
                    if(col)
                    {
                        qlwi->setIcon(IconFactory::getIconForDataType(col->getDataType()->getType()));
                    }
                    else
                    {
                        col = m_table->getColumnFromParents(m_table->fullColumns()[i]);
                        if(col)
                        {
                            qlwi->setIcon(IconFactory::getIconForDataType(col->getDataType()->getType()));
                        }
                    }
                }
            }
        }
    }
}

void NewTableForm::onSave()
{
    if(m_ui->txtTableName->text().length() == 0)
    {
        QMessageBox::critical (this, tr("Error"), tr("Please specify the name of the table!"), QMessageBox::Ok);
        return;
    }

    int dummy;
    QString tabName = m_ui->txtTableName->text();

    if(dynamic_cast<const SqlNamesValidator*>(m_ui->txtTableName->validator())->validate(tabName, dummy) == QValidator::Invalid)
    {
        QMessageBox::critical (this, tr("Error"), tr("Please specify a valid name for the table!"), QMessageBox::Ok);
        return;
    }

    Table* other = m_table->getVersion()->getTable(m_ui->txtTableName->text());
    if(other &&  other != m_table )
    {
        QMessageBox::critical(this, "Error", "Only one table with the name " + m_ui->txtTableName->text() + " is allowed in the database.", QMessageBox::Ok);
        return;
    }

    if(m_ui->tabWidget->currentIndex() == 0)    // The columns tab page is selected
    {
        if(m_ui->cmbNewColumnType->currentIndex() != -1 && m_ui->txtNewColumnName->text().length() != 0)    // if we are defining a column when pressed the "Save" button
        {
            onAddColumn();                      // add it and then save the table
        }
    }
    else
    if(m_ui->tabWidget->currentIndex() == 1)
    {
        if(m_ui->txtNewIndexName->text().length() != 0 && m_ui->lstSelectedColumnsForIndex->topLevelItemCount() != 0)
        {
            onAddIndex();
        }
    }
    else
    if(m_ui->tabWidget->currentIndex() == 2)
    {
        if(m_ui->txtForeignKeyName->text().length() > 0 &&  m_ui->lstForeignKeyAssociations->topLevelItemCount() != 0)
        {
            onBtnAddForeignKey();
        }
    }

    prepareValuesToBeSaved();
    doTheSave();
}

void NewTableForm::autoSave()
{
    prepareValuesToBeSaved();
    doTheSave();
}

void NewTableForm::doTheSave()
{
    if(m_version->hasTable(m_table))
    {
        // update the data of the table, and the tree view
        Workspace::getInstance()->onUpdateTable(m_table);
    }
    else
    {
        // create a new tree entry, add to the tree, update the m_table's tree item.
        Workspace::getInstance()->onSaveNewTable(m_table);
    }
}

void NewTableForm::prepareValuesToBeSaved()
{
    m_table->setName(m_ui->txtTableName->text());
    m_table->setDescription(m_ui->txtDescription->toPlainText());
}

void NewTableForm::onMoveColumnToRight()
{
    if(!m_ui->lstAvailableColumnsForIndex->currentItem()) return;
    QTreeWidgetItem* itm = new QTreeWidgetItem(QStringList(m_ui->lstAvailableColumnsForIndex->currentItem()->text()));
    itm->setIcon(0, m_ui->lstAvailableColumnsForIndex->currentItem()->icon());
    QString name = itm->text(0);
    delete m_ui->lstAvailableColumnsForIndex->currentItem();
    m_ui->lstSelectedColumnsForIndex->addTopLevelItem(itm);
    m_ui->lstSelectedColumnsForIndex->header()->resizeSections(QHeaderView::Stretch);
    if(m_ui->txtNewIndexName->text().length() == 0)
    {
        m_ui->txtNewIndexName->setText(QString("index_" + name));
    }
}

void NewTableForm::onMoveColumnToLeft()
{
    if(!m_ui->lstSelectedColumnsForIndex->currentItem()) return;
    delete m_ui->lstSelectedColumnsForIndex->currentItem();
    populateColumnsForIndices();
}

ContextMenuEnabledTreeWidgetItem* NewTableForm::createTWIForIndex(const Index* index)
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
    // TODO: Fetch a list of index properties from here and add them to the view
    a.append(columnsAsString);

    ContextMenuEnabledTreeWidgetItem* item = new ContextMenuEnabledTreeWidgetItem((ContextMenuEnabledTreeWidgetItem*)0, a);
    item->setIcon(0, IconFactory::getIndexIcon());
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

    if(m_ui->lstSelectedColumnsForIndex->topLevelItemCount() == 0)
    {
        QMessageBox::critical (this, tr("Error"), tr("Please specify at least one column"), QMessageBox::Ok);
        return;
    }

    if(m_currentIndex == 0)
    {
        // create the index object and populate with requried columns

        if(m_table->getIndexFromParents(m_ui->txtNewIndexName->text()) != 0)
        {
            QMessageBox::critical (this, tr("Error"), tr("You cannot modify the index of a parent table from a specialized table. (If you want to eanble the fields press the Cancel button)"), QMessageBox::Ok);
            m_ui->txtNewIndexName->setFocus();
            return;
        }

        if(m_table->hasIndex(m_ui->txtNewIndexName->text()))
        {
            QMessageBox::critical (this, tr("Error"), tr("You already have an index with this name for this table"), QMessageBox::Ok);
            m_ui->txtNewIndexName->setFocus();
            return;
        }

        Index* index = new Index(m_ui->txtNewIndexName->text(), m_table, QUuid::createUuid().toString(), m_version);
        index->initializeFor(m_dbEngine, QUuid(uidIndex));
        if(m_wspForIndex) m_wspForIndex->repopulateSpsOfObject(index);

        int cnt = m_ui->lstSelectedColumnsForIndex->topLevelItemCount();
        for(int i = 0; i< cnt; i++)
        {
            // check if this goes to a parent table or stays here
            const QTreeWidgetItem* cItem = m_ui->lstSelectedColumnsForIndex->topLevelItem(i);
            Column* col = m_table->getColumn(cItem->text(0));
            QString order = cItem->text(1);
            if(col)  // stays here
            {
                index->addColumn(col, order);
            }
            else
            {
                col = m_table->getColumnFromParents(cItem->text(0));
                if(col)
                {
                    index->addColumn(col, order);
                }
                else
                {
                    // something wrent wrong ... we shouldn't be here
                    QMessageBox::critical(this, tr("Error"), tr("Cannot fetch a column named ") + cItem->text(0) + tr(". Please contact the developers.") , QMessageBox::Ok);
                    return;
                }
            }
            // and now the SPs for the column
            for(int j=0; j<cItem->childCount(); j++)
            {
                QWidget* w = m_ui->lstSelectedColumnsForIndex->itemWidget(cItem->child(j), 1);
                if(QLineEdit* le = qobject_cast<QLineEdit*>(w))
                {
                    // now find the SP and create an instance of it
                    const QVector<Sp*> allSps = m_dbEngine->getDatabaseSpecificProperties();
                    Sp* theSp = 0;
                    for(int k=0; k<allSps.size(); k++)
                    {
                        if(allSps.at(k)->getPropertyGuiText() == cItem->child(j)->text(0))
                        {
                            theSp = allSps.at(k);
                            break;
                        }
                    }

                    if(theSp)
                    {
                        SpInstance* spi = theSp->instantiate();
                        spi->set(le->text());
                        index->addSpToColumn(col, m_dbEngine->getDatabaseEngineName(), spi);
                    }
                }
            }


        }
        m_table->addIndex(index);

        ContextMenuEnabledTreeWidgetItem* item = createTWIForIndex(index);
        m_ui->lstIndices->addTopLevelItem(item);
        index->setLocation(item);
    }
    else    // update the index with the modified data
    {
        m_currentIndex->setName(m_ui->txtNewIndexName->text());
        m_currentIndex->resetColumns();
        QString columnsAsString = "";
        int cnt = m_ui->lstSelectedColumnsForIndex->topLevelItemCount();
        for(int i = 0; i< cnt; i++)
        {
            const QTreeWidgetItem* cItem = m_ui->lstSelectedColumnsForIndex->topLevelItem(i);
            QString order = m_ui->lstSelectedColumnsForIndex->topLevelItem(i)->text(1);
            columnsAsString += m_ui->lstSelectedColumnsForIndex->topLevelItem(i)->text(0);
            if(i < cnt - 1)
            {
                columnsAsString += ", ";
            }
            Column* col = m_table->getColumn(cItem->text(0));
            if(col)  // stays here
            {
                m_currentIndex->addColumn(col, order);
            }
            else
            {
                col = m_table->getColumnFromParents(cItem->text(0));
                if(col)
                {
                    m_currentIndex->addColumn(col, order);
                }
                else
                {
                    // something wrent wrong ... we shouldn't be here
                    QMessageBox::critical(this, tr("Error"), tr("Cannot fetch a column named ") + cItem->text(0) + tr(". Please contact the developers.") , QMessageBox::Ok);
                    return;
                }
            }
            // and now the SPs for the column
            // TODO: Duplicate with code from above. Difference: destination index :(
            for(int j=0; j<cItem->childCount(); j++)
            {
                QWidget* w = m_ui->lstSelectedColumnsForIndex->itemWidget(cItem->child(j), 1);
                if(QLineEdit* le = qobject_cast<QLineEdit*>(w))
                {
                    // now find the SP and create an instance of it
                    const QVector<Sp*> allSps = m_dbEngine->getDatabaseSpecificProperties();
                    Sp* theSp = 0;
                    for(int k=0; k<allSps.size(); k++)
                    {
                        if(allSps.at(k)->getPropertyGuiText() == cItem->child(j)->text(0))
                        {
                            theSp = allSps.at(k);
                            break;
                        }
                    }

                    if(theSp)
                    {
                        SpInstance* spi = theSp->instantiate();
                        spi->set(le->text());
                        m_currentIndex->addSpToColumn(col, m_dbEngine->getDatabaseEngineName(), spi);
                    }
                }
            }
        }

        m_currentIndex->getLocation()->setText(0, m_currentIndex->getName());
        m_currentIndex->getLocation()->setText(1, columnsAsString);
    }
    resetIndexGui();

    autoSave();
}

void NewTableForm::resetIndexGui()
{
    m_ui->lstSelectedColumnsForIndex->clear();
    populateColumnsForIndices();
    m_ui->txtNewIndexName->setText("");
    m_ui->btnAddIndex->setIcon(IconFactory::getAddIcon());
    m_currentIndex = 0;
    m_ui->tabWidgetForIndex->setCurrentIndex(0);
    prepareSpsTabsForIndex(0);
    toggleIndexFieldDisableness(false);
    m_ui->cmbIndexOrderType->setCurrentIndex(0);
}

void NewTableForm::populateIndexGui(Index* idx)
{
    m_ui->txtNewIndexName->setText(idx->getName());

    // fill up the two lists for the index columns
    m_ui->lstAvailableColumnsForIndex->clear();
    m_ui->lstSelectedColumnsForIndex->clear();

    m_ui->cmbIndexOrderType->setCurrentIndex(-1);

    for(int i=0; i< m_table->fullColumns().size(); i++)
    {
        Column* column = m_table->getColumn(m_table->fullColumns()[i]);
        if(!column)
        {
            column = m_table->getColumnFromParents(m_table->fullColumns()[i]);
            if(!column) // this shouldn't be
            {
                QMessageBox::critical(this, tr("Error"), tr("An index has an invalid reference as ") + m_table->fullColumns()[i], QMessageBox::Ok);
                return;
            }
        }
        QListWidget *targetList = 0;
        if(idx->hasColumn(column))
        {
            QStringList a; a<<column->getName(); a<<idx->getOrderForColumn(column);
            QTreeWidgetItem* itm = new QTreeWidgetItem(a);
            itm->setIcon(0, IconFactory::getIconForDataType(column->getDataType()->getType()));
            m_ui->lstSelectedColumnsForIndex->addTopLevelItem(itm);

            // and now feed in the SPs for the given column
            QMap<QString, QVector<SpInstance*> > sps = idx->getSpsOfColumn(column);
            for(int j = 0; j<sps.keys().size(); j++)
            {
                QString dbName = sps.keys().at(j);
                QVector<SpInstance*> insts = sps[dbName];
                for(int k=0; k<insts.size(); k++)
                {
                    QTreeWidgetItem* spiItem = new QTreeWidgetItem(itm, QStringList(insts.at(k)->getClass()->getPropertyGuiText()));
                    m_ui->lstSelectedColumnsForIndex->addTopLevelItem(spiItem);
                    m_ui->lstSelectedColumnsForIndex->header()->resizeSections(QHeaderView::Stretch);
                    itm->setExpanded(true);

                    // TODO: when there will be more databases change this!
                    spiItem->setIcon(0, IconFactory::getMySqlIcon());

                    // TODO: set the control in column 2 according to the type of the SP
                    if(insts.at(k)->getClass()->getClassUid().toString() == uidValueSp)
                    {   // create a QLineEdit
                        QLineEdit* lstValueSp = new QLineEdit(0);
                        m_ui->lstSelectedColumnsForIndex->setItemWidget(spiItem, 1, lstValueSp);
                        lstValueSp->setText(insts.at(k)->get());
                    }
                }
            }
        }
        else
        {
            targetList = m_ui->lstAvailableColumnsForIndex;
            QListWidgetItem* qlwi = new QListWidgetItem(column->getName(), targetList);
            qlwi->setIcon(IconFactory::getIconForDataType(column->getDataType()->getType()));
        }
    }
}

void NewTableForm::toggleIndexFieldDisableness(bool a)
{
    m_ui->txtNewIndexName->setDisabled(a);
    m_ui->lstAvailableColumnsForIndex->setDisabled(a);
    m_ui->lstSelectedColumnsForIndex->setDisabled(a);

    m_ui->btnMoveColumnToLeft->setDisabled(a);
    m_ui->btnMoveColumnToRight->setDisabled(a);
    m_ui->btnMoveIndexColumnUp->setDisabled(a);
    m_ui->btnMoveIndexColumnDown->setDisabled(a);
}

void NewTableForm::onSelectIndex(QTreeWidgetItem*, int)
{
    m_currentIndex = m_table->getIndex(m_ui->lstIndices->currentItem()->text(0));

    if(m_currentIndex == 0)
    {
        Index* parentsIndex = m_table->getIndexFromParents(m_ui->lstIndices->currentItem()->text(0));
        if(parentsIndex == 0)
        {
            return;
        }
        populateIndexGui(parentsIndex);
        toggleIndexFieldDisableness(true);
    }
    else
    {
        populateIndexGui(m_currentIndex);
        toggleIndexFieldDisableness(false);
    }

    m_ui->btnAddIndex->setIcon(IconFactory::getApplyIcon());
    prepareSpsTabsForIndex(m_currentIndex);
    m_ui->lstSelectedColumnsForIndex->header()->resizeSections(QHeaderView::Stretch);
}

void NewTableForm::onCancelIndexEditing()
{
    resetIndexGui();
}

void NewTableForm::onBtnRemoveIndex()
{
    if(!m_ui->lstIndices->currentItem())
    {
        QMessageBox::critical (this, tr("Error"), tr("Please make sure you have selected an index to delete."), QMessageBox::Ok);
        return;
    }

    if(!m_table->hasIndex(m_ui->lstIndices->currentItem()->text(0)))
    {
        QMessageBox::critical (this, tr("Error"), tr("You cannot delete the index of a parent table from a sibling table. Go to the parent table to do this."), QMessageBox::Ok);
        return;
    }

    if(m_ui->lstIndices->currentItem()->text(0).startsWith(strAutoIdx))
    {
        QMessageBox::critical (this, tr("Error"), tr("You cannot delete an automatically generated index. Highly possibly the column is used in a foreign key."), QMessageBox::Ok);
        return;
    }
    delete m_ui->lstIndices->currentItem();
    resetIndexGui();
    m_table->removeIndex(m_currentIndex);
    m_currentIndex = 0;
    autoSave();
}

void NewTableForm::onDoubleClickColumnForIndex(QListWidgetItem* item)
{
    if(!item) return;
    QString name = item->text();
    QTreeWidgetItem* itm = new QTreeWidgetItem(QStringList(name));
    itm->setIcon(0, item->icon());
    delete m_ui->lstAvailableColumnsForIndex->currentItem();
    m_ui->lstSelectedColumnsForIndex->addTopLevelItem(itm);
    m_ui->lstSelectedColumnsForIndex->header()->resizeSections(QHeaderView::Stretch);
    if(m_ui->txtNewIndexName->text().length() == 0)
    {
        m_ui->txtNewIndexName->setText(QString("index_" + name));
    }
}

void NewTableForm::onMoveSelectedIndexColumnUp()
{
    // and is this item a column?
    if(!m_table->hasColumn(m_ui->lstSelectedColumnsForIndex->currentItem()->text(0)))
    {
        if(!m_table->parentsHaveColumn(m_ui->lstSelectedColumnsForIndex->currentItem()->text(0)))
        {
            QMessageBox::information(this, tr("Info"), tr("For this functionality select a column you want to move up"), QMessageBox::Ok);
            return;
        }
    }

    if(m_ui->lstSelectedColumnsForIndex->selectedItems().size() > 0)
    {
        QModelIndex x = m_ui->lstSelectedColumnsForIndex->currentIndex();
        if(x.row() > 0)
        {
            QTreeWidgetItem* c = m_ui->lstSelectedColumnsForIndex->currentItem();
            QTreeWidgetItem* w = new QTreeWidgetItem(*c);
            for(int i=0; i<c->childCount(); i++)
            {
                QTreeWidgetItem* nc = new QTreeWidgetItem(*c->child(i));
                w->addChild(nc);

                QLineEdit* lstValueSp = new QLineEdit(0);
                QLineEdit* oldLE = qobject_cast<QLineEdit*>(m_ui->lstSelectedColumnsForIndex->itemWidget(c->child(i), 1));
                if(oldLE)
                {
                    lstValueSp->setText(oldLE->text());
                }

                m_ui->lstSelectedColumnsForIndex->setItemWidget(nc, 1, lstValueSp);

            }

            delete m_ui->lstSelectedColumnsForIndex->currentItem();
            m_ui->lstSelectedColumnsForIndex->insertTopLevelItem(x.row() - 1, w);
            m_ui->lstSelectedColumnsForIndex->setCurrentItem(w);
            w->setExpanded(true);
        }
    }
}

void NewTableForm::onMoveSelectedIndexColumnDown()
{
    // and is this item a column?
    if(!m_table->hasColumn(m_ui->lstSelectedColumnsForIndex->currentItem()->text(0)))
    {
        if(!m_table->parentsHaveColumn(m_ui->lstSelectedColumnsForIndex->currentItem()->text(0)))
        {
            QMessageBox::information(this, tr("Info"), tr("For this functionality select a column you want to move down"), QMessageBox::Ok);
            return;
        }
    }

    if(m_ui->lstSelectedColumnsForIndex->selectedItems().size() > 0)
    {
        QModelIndex x = m_ui->lstSelectedColumnsForIndex->currentIndex();
        if(x.row() < m_ui->lstSelectedColumnsForIndex->topLevelItemCount() - 1)
        {
            QTreeWidgetItem* c = m_ui->lstSelectedColumnsForIndex->currentItem();
            QTreeWidgetItem* w = new QTreeWidgetItem(*c);
            for(int i=0; i<c->childCount(); i++)
            {
                QTreeWidgetItem* nc = new QTreeWidgetItem(*c->child(i));
                w->addChild(nc);

                QLineEdit* lstValueSp = new QLineEdit(0);
                QLineEdit* oldLE = qobject_cast<QLineEdit*>(m_ui->lstSelectedColumnsForIndex->itemWidget(c->child(i), 1));
                if(oldLE)
                {
                    lstValueSp->setText(oldLE->text());
                }

                m_ui->lstSelectedColumnsForIndex->setItemWidget(nc, 1, lstValueSp);

            }

            delete m_ui->lstSelectedColumnsForIndex->currentItem();
            m_ui->lstSelectedColumnsForIndex->insertTopLevelItem(x.row() + 1, w);
            m_ui->lstSelectedColumnsForIndex->setCurrentItem(w);
            w->setExpanded(true);
        }
    }
}

void NewTableForm::onForeignTableComboChange(QString selected)
{
    Table* table = m_version->getTable(selected);
    if(table == 0)
    {
        return;
    }
    m_foreignTable = table;
    QStringList foreignColumns = table->fullColumns();
    m_ui->lstForeignTablesColumns->clear();
    for(int i=0; i<foreignColumns.size(); i++)
    {
        QListWidgetItem* qlwi = new QListWidgetItem(foreignColumns[i], m_ui->lstForeignTablesColumns);
        // TODO: these few lines below are duplicates with populateColumnsForIndices
        Column* col = table->getColumn(foreignColumns[i]);
        if(col)
        {
            qlwi->setIcon(IconFactory::getIconForDataType(col->getDataType()->getType()));
        }
        else
        {
            col = table->getColumnFromParents(foreignColumns[i]);
            if(col)
            {
                qlwi->setIcon(IconFactory::getIconForDataType(col->getDataType()->getType()));
            }
        }
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

        if(foreignColumn == 0)
        {
            foreignColumn = m_foreignTable->getColumnFromParents(selectedItems[i]->text());
            if(foreignColumn == 0)
            {
                return ;
            }
        }

        QStringList parentColumns = m_table->fullColumns();
        for(int j=0; j<parentColumns.size(); j++)
        {
            Column* col = m_table->getColumn(parentColumns[j]);
            if(col)
            {
                if(col->getDataType()->getName() == foreignColumn->getDataType()->getName())
                {
                    QListWidgetItem* qlwj = new QListWidgetItem(parentColumns[j], m_ui->lstLocalColumn);
                    qlwj->setIcon(IconFactory::getIconForDataType(col->getDataType()->getType()));
                }
            }
            else
            {
                col = m_table->getColumnFromParents(parentColumns[j]);
                if(col)
                {
                    if(col->getDataType()->getName() == foreignColumn->getDataType()->getName())
                    {
                        QListWidgetItem* qlwj = new QListWidgetItem(parentColumns[j], m_ui->lstLocalColumn);
                        qlwj->setIcon(IconFactory::getIconForDataType(col->getDataType()->getType()));
                    }
                }
            }
        }

        break;
    }
}

void NewTableForm::onAddForeignKeyAssociation()
{

    QString foreignColumn, localColumn;
    const Column* cforeignColumn = NULL;
    const Column* clocalColumn = NULL;
    QList<QListWidgetItem *> selectedItems = m_ui->lstForeignTablesColumns->selectedItems();
    for(int i=0; i< selectedItems.size(); i++)
    {
        foreignColumn = selectedItems[i]->text();
        cforeignColumn = m_foreignTable->getColumn(selectedItems[i]->text());
        if(cforeignColumn == 0)
        {
            cforeignColumn = m_foreignTable->getColumnFromParents(selectedItems[i]->text());
            if(cforeignColumn == 0)
            {
                return ; // Shouldn't happen
            }
        }
        break;
    }


    selectedItems = m_ui->lstLocalColumn->selectedItems();
    for(int i=0; i< selectedItems.size(); i++)
    {
        localColumn = selectedItems[i]->text();
        clocalColumn = m_table->getColumn(selectedItems[i]->text());
        if(clocalColumn == 0)
        {
            clocalColumn = m_table->getColumnFromParents(selectedItems[i]->text());
            if(clocalColumn == 0)
            {
                return; // Neither this
            }
        }
        break;
    }

    if(localColumn.length() == 0 || foreignColumn.length() == 0)
    {
        QMessageBox::critical (this, tr("Error"), tr("Please select two columns: one from the foreign table, one from the local table"), QMessageBox::Ok);
        return;
    }


    // check if we have the same column associations once more in the asociations
    if(m_currentForeignKey && m_currentForeignKey->hasAssociation(foreignColumn, localColumn))
    {
        QMessageBox::critical (this, tr("Error"), tr("You cannot have the same column associoation two times, the database does not allow this."), QMessageBox::Ok);
        return;
    }

    // finally do some work too
    if(!m_currentForeignKey)
    {
        m_currentForeignKey = new ForeignKey();
        m_foreignKeySelected = false;
    }

    QStringList a(foreignColumn);
    a.append(localColumn);
    QTreeWidgetItem* item = new QTreeWidgetItem((QTreeWidget*)0, a);

    // sets the data, this will be used on the "on click on this item" method to correctly populate the controls
    QVariant var(m_foreignTable->getName());
    item->setData(0, Qt::UserRole, var);

    item->setIcon(0, IconFactory::getIconForDataType(cforeignColumn->getDataType()->getType()));
    item->setIcon(1, IconFactory::getIconForDataType(clocalColumn->getDataType()->getType()));

    m_ui->lstForeignKeyAssociations->addTopLevelItem(item);

    ForeignKey::ColumnAssociation* fca = new ForeignKey::ColumnAssociation(m_foreignTable, cforeignColumn, m_table, clocalColumn);
    m_currentForeignKey->addAssociation(fca);
}

void NewTableForm::onRemoveForeignKeyAssociation()
{
    if(!m_ui->lstForeignKeyAssociations->currentItem())
    {
        QMessageBox::critical (this, tr("Error"), tr("Please make sure you have a foreign key association to delete."), QMessageBox::Ok);
        return;
    }

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

void NewTableForm::onSelectAssociation(QTreeWidgetItem* current, int)
{
    QVariant qv = current->data(0, Qt::UserRole);
    QString tabName = qv.toString();
    m_foreignTable = m_version->getTable(tabName);
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
        qlwi->setIcon(IconFactory::getIconForDataType(m_foreignTable->getColumns()[i]->getDataType()->getType()));
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
    if(m_table->getForeignKeyFromParents(m_ui->txtForeignKeyName->text()))
    {
        QMessageBox::critical (this, tr("Error"), tr("You cannot modify a foreign key from the parent table in a specialized table. (If you want to enable the GUI fields press Cancel)"), QMessageBox::Ok);
        m_ui->txtForeignKeyName->setFocus();
        return;
    }

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

    if((m_table->getForeignKey(m_ui->txtForeignKeyName->text()) || m_table->getForeignKeyFromParents(m_ui->txtForeignKeyName->text()))&&!m_foreignKeySelected)
    {
        QMessageBox::critical (this, tr("Error"), tr("Duplicate foreign key name. Please use a name which cannot be found in this table, nor the parent tables."), QMessageBox::Ok);
        m_ui->txtForeignKeyName->setFocus();
        return;
    }

    // check for circular reference, meaning: if the "foreign table" has any foreign keys to this table, do not allow it
    if(m_currentForeignKey->getForeignTable()->getForeignKeyToTable(m_table) != 0)
    {
        QMessageBox::critical (this, tr("Error"), tr("Trying to create a circular reference to this table, since ") + m_currentForeignKey->getForeignTableName() +
                               tr(" already contains a foreign key to this table. This is not allowed."), QMessageBox::Ok);
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
        ContextMenuEnabledTreeWidgetItem* twi = createTWIForForeignKey(m_currentForeignKey);
        m_ui->lstForeignKeys->addTopLevelItem(twi);
        m_currentForeignKey->setLocation(twi);
        m_table->addForeignKey(m_currentForeignKey);
        // now, here create an index in the _foreign table_ which has columns the foreign columns of the index (this is required for some versions of MySQL, we'll see for the other ones)
        QString fktName = m_currentForeignKey->getForeignTableName();
        Table* tbl = m_version->getTable(fktName);
        Index* generatedIndex = tbl->createAutoIndex(m_currentForeignKey->foreignColumns());
        m_currentForeignKey->addAutogeneratedIndex(generatedIndex);
    }

    autoSave();
    resetFkGui();
}

void NewTableForm::resetFkGui()
{
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
    m_ui->btnAddForeignKey->setIcon(IconFactory::getAddIcon());
}

void NewTableForm::toggleFkFieldDisableness(bool a)
{

    m_ui->lstForeignKeyAssociations->setDisabled(a);
    m_ui->lstForeignTablesColumns->setDisabled(a);
    m_ui->cmbForeignTables->setDisabled(a);
    m_ui->txtForeignKeyName->setDisabled(a);
    m_ui->cmbFkOnDelete->setDisabled(a);
    m_ui->cmbFkOnUpdate->setDisabled(a);
    m_ui->lstLocalColumn->setDisabled(a);
    m_ui->btnAddForeignKeyAssociation->setDisabled(a);
    m_ui->btnRemoveAssociation->setDisabled(a);
}

void NewTableForm::populateFKGui(ForeignKey * fk)
{
    m_ui->lstForeignKeyAssociations->clear();
    m_ui->cmbForeignTables->setCurrentIndex(-1);
    m_foreignKeySelected = true;

    m_ui->txtForeignKeyName->setText(fk->getName());

    // create the columns associations
    QString tabName = "";
    const QVector<ForeignKey::ColumnAssociation*>& assocs = fk->getAssociations();
    for(int i=0; i<assocs.size(); i++)
    {
        tabName = assocs[i]->getForeignTable()->getName();
        QStringList a(assocs[i]->getForeignColumn()->getName());
        a.append(assocs[i]->getLocalColumn()->getName());
        QTreeWidgetItem* item = new QTreeWidgetItem((QTreeWidget*)0, a);

        // sets the data, this will be used on the "on click on this item" method to correctly populate the controls
        QVariant var(assocs[i]->getForeignTable()->getName());
        item->setData(0, Qt::UserRole, var);

        item->setIcon(0, IconFactory::getIconForDataType(assocs[i]->getForeignColumn()->getDataType()->getType()));
        item->setIcon(1, IconFactory::getIconForDataType(assocs[i]->getLocalColumn()->getDataType()->getType()));

        m_ui->lstForeignKeyAssociations->addTopLevelItem(item);
    }

    m_ui->lstForeignKeyAssociations->resizeColumnToContents(0);
    m_ui->lstForeignKeyAssociations->resizeColumnToContents(1);

    // find the foreign keys table in the combo box
    int idx = m_ui->cmbForeignTables->findText(tabName);
    Table* table = m_version->getTable(tabName);
    // populate the foreign columns list
    if(table == 0)
    {
        return;
    }
    m_foreignTable = table;
    m_ui->cmbForeignTables->setCurrentIndex(idx);
    QStringList foreignColumns = table->fullColumns();
    m_ui->lstForeignTablesColumns->clear();
    for(int i=0; i<foreignColumns.size(); i++)
    {
        QListWidgetItem* qlwi = new QListWidgetItem(foreignColumns[i], m_ui->lstForeignTablesColumns);

        Column* col = table->getColumn(foreignColumns[i]);
        if(col)
        {
            qlwi->setIcon(IconFactory::getIconForDataType(col->getDataType()->getType()));
        }
        else
        {
            col = table->getColumnFromParents(foreignColumns[i]);
            if(col)
            {
                qlwi->setIcon(IconFactory::getIconForDataType(col->getDataType()->getType()));
            }
        }
    }
    // select the on update, on delete combo boxes
    idx = m_ui->cmbFkOnDelete->findText(fk->getOnDelete());
    m_ui->cmbFkOnDelete->setCurrentIndex(idx);
    idx = m_ui->cmbFkOnUpdate->findText(fk->getOnUpdate());
    m_ui->cmbFkOnUpdate->setCurrentIndex(idx);
    m_ui->btnAddForeignKey->setIcon(IconFactory::getApplyIcon());
}

void NewTableForm::onSelectForeignKey(QTreeWidgetItem*, int)
{
    QModelIndex x = m_ui->lstForeignKeys->currentIndex();
    m_currentForeignKey = m_table->getForeignKey(m_ui->lstForeignKeys->currentItem()->text(0));

    if(m_currentForeignKey == 0)
    {
        ForeignKey* parFk = m_table->getForeignKeyFromParents(m_ui->lstForeignKeys->currentItem()->text(0));
        if(parFk == 0)
        {
            return; // Shouldn't be
        }

        populateFKGui(parFk);
        toggleFkFieldDisableness(true);
        return;
    }

    populateFKGui(m_currentForeignKey);
    toggleFkFieldDisableness(false);
}

void NewTableForm::updateDefaultValuesTableHeader()
{
    m_ui->tableStartupValues->setColumnCount(m_table->getColumns().count());
    for(int i=0; i<m_table->getColumns().count(); i++)
    {
        QTableWidgetItem *columnHeaderItem = new QTableWidgetItem(m_table->getColumns()[i]->getName());
        columnHeaderItem->setIcon(IconFactory::getIconForDataType(m_table->getColumns()[i]->getDataType()->getType()));
        columnHeaderItem->setTextAlignment(Qt::AlignVCenter);

        m_ui->tableStartupValues->setHorizontalHeaderItem(i, columnHeaderItem);
    }
}

void NewTableForm::onAddNewDefaultRow()
{
    addNewRowToTable(m_ui->tableStartupValues, m_table);
    onBtnUpdateTableWithDefaultValues();
    autoSave();
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
    bool errorFound = false;
    for(int i=0; i<m_ui->tableStartupValues->rowCount(); i++)
    {
        QVector<QString> rowI;
        for(int j=0; j<m_ui->tableStartupValues->columnCount(); j++)
        {
            if(m_ui->tableStartupValues->item(i,j))
            {
                QString cName = m_ui->tableStartupValues->horizontalHeaderItem(j)->text();
                Column *c =m_table->getColumn(cName);
                if(c == 0)
                {
                    c = m_table->getColumnFromParents(cName);
                }
                const UserDataType* dt = c->getDataType();
                QTableWidgetItem* itm = m_ui->tableStartupValues->item(i,j);
                if(dt->isValid(itm->text()))
                {
                    rowI.append(m_ui->tableStartupValues->item(i,j)->text());
                    itm->setBackgroundColor(Qt::white);
                    itm->setToolTip("");
                }
                else
                {
                    itm->setBackgroundColor(Qt::red);
                    errorFound = true;
                    itm->setToolTip("This column type does not support this value");
                }
            }
            else
            {
                rowI.append("");
            }
        }
        values.append(rowI);
    }
    if(!errorFound)
    {
        m_table->setDefaultValues(values);
    }
}

void NewTableForm::onSaveStartupValuestoCSV()
{
    saveStartupValuesToCSVFromTable(m_ui->tableStartupValues, this);
}

void NewTableForm::onLoadStartupValuesFromCSV()
{
    loadStartupValuesFromCSVIntoTable(m_ui->tableStartupValues, this);
    autoSave();
}

void NewTableForm::updateSqlDueToChange()
{
    presentSql(m_project, m_version);
}

void NewTableForm::onBtnCancelForeignKeyEditing()
{
    toggleFkFieldDisableness(false);
    resetFkGui();
    m_ui->btnAddForeignKey->setIcon(IconFactory::getAddIcon());
}

void NewTableForm::onBtnRemoveForeignKey()
{
    if(!m_ui->lstForeignKeys->currentItem())
    {
        QMessageBox::critical (this, tr("Error"), tr("Please make sure you have a foreign key selected to remove."), QMessageBox::Ok);
        return;
    }

    if(m_currentForeignKey == 0 && m_table->getForeignKeyFromParents(m_ui->lstForeignKeys->currentItem()->text(0)) != 0)
    {
        QMessageBox::critical (this, tr("Error"), tr("You cannot remove this foreign key, since it does not belong to this table. Remove it from the parent table."), QMessageBox::Ok);
        return;
    }
    m_currentForeignKey->onDelete();
    m_table->removeForeignKey(m_currentForeignKey);
    m_table->getVersion()->removeForeignKeyFromDiagrams(m_currentForeignKey);
    delete m_currentForeignKey->getLocation();
    m_currentForeignKey = 0;
    resetFkGui();
    autoSave();
}

void NewTableForm::onDeleteDefaultRow()
{
    m_ui->tableStartupValues->removeRow(m_ui->tableStartupValues->currentRow());
    onBtnUpdateTableWithDefaultValues();
    autoSave();
}

void NewTableForm::onHelp()
{
    HelpWindow* hw = HelpWindow::instance();
    hw->showHelp(QString("/doc/tabl.html"));
    hw->show();
}

void NewTableForm::onChangeTab(int idx)
{
    if(idx > 0)
    {
        if(m_ui->tabWidget->tabText(idx) == "SQL")
        {
            if(m_table)
            {
                presentSql(m_project, m_version);
            }
        }
    }
}

void NewTableForm::onChangeDescription()
{
    m_table->setDescription(m_ui->txtDescription->toPlainText());
}

void NewTableForm::onChangeName(QString a)
{

    if(!m_table) return;
    Table* another = m_table->getVersion()->getTable(a);
    QString prevName = m_table->getName();
    if(!m_table->getLocation()) return;

    QPalette pal;
    pal.setColor(QPalette::Text, Qt::black);
    m_ui->txtTableName->setPalette(pal);

    // and see if there is a table with this name already
    if(m_table->getVersion()->hasTable(a) && another && another != m_table)
    {
        QPalette pal;
        pal.setColor(QPalette::Text, Qt::red);
        m_ui->txtTableName->setPalette(pal);
    }
    else
    {
        QVariant v(a);
        m_table->setName(a);
        m_table->setDisplayText(a);
        updateSqlDueToChange();
        // and see if this was due to a new table being focused ... weird check.
        if(prevName != a) updateIssues();
    }
}

void NewTableForm::onInject()
{
    InjectSqlDialog* injectDialog = new InjectSqlDialog(Workspace::getInstance()->currentProjectsEngine(), this, 0);

    injectDialog->setModal(true);
    bool error = false;
    if(injectDialog->exec() == QDialog::Accepted)
    {
        QString tSql;
        QStringList connectionNames = injectDialog->getSelectedConnections();
        for(int i=0; i< connectionNames.size(); i++)
        {
            Connection* c = ConnectionManager::instance()->getConnection(connectionNames.at(i));
            if(c)
            {
                if(!m_dbEngine->executeSql(c, finalSql, tSql, injectDialog->getRollbackOnError()))
                {
                    QMessageBox::critical (this, tr("Error"), tr("<B>Cannot execute a query!</B><P>Reason: ") + m_dbEngine->getLastError() + tr(".<P>Query:<PRE>") + tSql+ "</PRE><P>" +
                                           (injectDialog->getRollbackOnError()?tr("Transaction was rolled back."):tr("Transaction was <font color=red><B>NOT</B></font> rolled back, you might have partial data in your database.")), QMessageBox::Ok);
                    error = true;
                }
            }
        }
        MainWindow::instance()->setStatus(QString("Creating table ") + m_table->getName() + (error?" Failed ":" Succeeded "), error);
    }
}

void NewTableForm::onSaveSql()
{
    QString name = QFileDialog::getSaveFileName(this, tr("Save sql"), "", tr("SQL files (*.sql)"));
    QFile file(name);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return;
    }
    QTextStream out(&file);
    out << m_ui->txtSql->toPlainText() << "\n";
}

void NewTableForm::presentSql(Project *, Version *v)
{
    QString fs = "";
    QHash<QString,QString> fo = Configuration::instance().sqlGenerationOptions();
    fo["FKSposition"] = "OnlyInternal";
    finalSql = m_project->getEngine()->getSqlGenerator()->generateCreateTableSql(m_table, fo, m_table->getName(), 0);
    if(!Workspace::getInstance()->currentProjectIsOop())
    {
        finalSql << m_project->getEngine()->getSqlGenerator()->generateDefaultValuesSql(m_table, fo);
    }

    for(int i=0; i< finalSql.size(); i++)
    {
        fs += finalSql[i];
    }

    m_ui->txtSql->setText(fs);
    if(m_table->getColumnCount() == 0 && m_table->getTotalParentColumnCount() == 0)
    {
        m_ui->btnInject->setEnabled(false);
    }
    else
    {
        m_ui->btnInject->setEnabled(true);
    }
}

void NewTableForm::onCopyColumn()
{
    if(lstColumns->getLastRightclickedItem() != 0)
    {
        ContextMenuEnabledTreeWidgetItem* item = lstColumns->getLastRightclickedItem();
        lstColumns->setLastRightclickedItem(0);

        QVariant qv = item->data(0, Qt::UserRole);
        QString cName = qv.toString();
        Column* c = m_table->getColumn(cName);

        c->copy();
    }
}

void NewTableForm::onPasteColumn()
{
    Column* col = ClipboardFactory::pasteColumn();
    if(!col) return;

    backupDefaultValuesTable();

    // TODO: This is duplicate with stuff from the "onAddcolumn"
    col->setName(col->getName() + "_copy");
    col->setName(m_table->generateUniqueColumnName(col->getName()));
    ContextMenuEnabledTreeWidgetItem* item = createTWIForColumn(col);
    lstColumns->addTopLevelItem(item);
    setTypeComboBoxForColumnItem(item, col);
    m_table->addColumn(col);
    // and update the table instances of the table, by adding a new table instance
    m_table->tableInstancesAddColumn(col);

    col->setLocation(item);
    m_ui->txtNewColumnName->setFocus( );
    // till here

    lstColumns->resizeColumnToContents(0);
    lstColumns->resizeColumnToContents(1);
    lstColumns->resizeColumnToContents(2);
    lstColumns->resizeColumnToContents(3);

    populateColumnsForIndices();
    updateDefaultValuesTableHeader();
    restoreDefaultValuesTable();

    autoSave();
}

void NewTableForm::onCodepageChange(QString)
{
    updateSqlDueToChange();
}

void NewTableForm::presentSql(Project*, SqlSourceEntity*, MainWindow::showSomething)
{
}

void NewTableForm::onDescriptionChange()
{
    if(m_currentColumn)
    {
        m_currentColumn->setDescription(m_ui->txtColumnDescription->toPlainText());
        autoSave();
    }
}

void NewTableForm::onColumnNameChange(QString)
{
    if(m_currentColumn)
    {
        if(m_ui->txtNewColumnName->text().length() == 0) return;
        m_currentColumn->setName(m_ui->txtNewColumnName->text());
        m_currentColumn->getLocation()->setText(1, m_currentColumn->getName());
        autoSave();
    }
}

void NewTableForm::onDatatypeComboChange(QString)
{
    if(m_ui->cmbNewColumnType->currentIndex() == -1) return;
    if(m_currentColumn)
    {
        // TODO: This is dupliation with code from onAddColumn
        if(m_ui->cmbNewColumnType->currentText() != m_currentColumn->getDataType()->getName())
        {
            // in affirmative case check that:
            // 1. this column is not referencing any other columns in the DB through a foreign key
            QVector<ForeignKey*> fks = m_table->columnParticipatesInForeignKey(m_currentColumn);
            if(fks.size() > 0)
            {
                QString s = "";
                for(int i=0; i<fks.size(); i++)
                {
                    s += "\n-" + fks.at(i)->getName();
                }
                QMessageBox::critical (this, tr("Error"), tr("Cannot change the type of the columns since it's participating in Foreign Key. Please change the following foreign keys and try again.")+ s, QMessageBox::Ok);
                return;
            }
            // 2. no other column is referencing this column in the DB through a foreign key from all the tables in the version
            QVector<Table*> otherTablesReferencingThiscolumn = Workspace::getInstance()->workingVersion()->getTablesReferencingAColumnThroughForeignKeys(m_currentColumn);
            if(otherTablesReferencingThiscolumn.size() > 0)
            {
                QString s = "";
                for(int i=0; i<otherTablesReferencingThiscolumn.size(); i++)
                {
                    s += "\n-" + otherTablesReferencingThiscolumn.at(i)->getName();
                }
                QMessageBox::critical (this, tr("Error"), tr("Cannot change the type of the columns since it's participating in Foreign Keys. Please change the following tables and try again.")+ s, QMessageBox::Ok);
                return;
            }
        }

        m_currentColumn->setDataType(m_version->getDataType(m_ui->cmbNewColumnType->currentText()));
        //m_currentColumn->getLocation()->setIcon(COL_POS_DT, m_currentColumn->getDataType()->getIcon());
        m_currentColumn->getLocation()->setText(COL_POS_DT, m_currentColumn->getDataType()->getName());

        autoSave();
        if(m_wspForColumn)
        {
                m_wspForColumn->taylorToSpecificObject(m_currentColumn);
        }
    }
    else
    {
        if(m_wspForColumn)
        {
            Column* tCol = new Column(QUuid::createUuid().toString(), "temp",(m_version->getDataType(m_ui->cmbNewColumnType->currentText())), false, m_version);
            m_wspForColumn->taylorToSpecificObject(tCol);
            delete tCol;
        }
    }

}

void NewTableForm::onPrimaryChange(bool)
{
    if(m_currentColumn)
    {
        m_currentColumn->setPk(m_ui->chkPrimary->isChecked());
        if(m_ui->chkPrimary->isChecked())
        {
            m_currentColumn->getLocation()->setIcon(COL_POS_PK, IconFactory::getKeyIcon());
        }
        else
        {
            m_currentColumn->getLocation()->setIcon(COL_POS_PK, IconFactory::getEmptyIcon());
        }

        autoSave();
    }
}

void NewTableForm::onIndexNameChange(QString)
{
    if(m_currentIndex)
    {
        if(m_ui->txtNewIndexName->text().length() == 0) return;

        m_currentIndex->setName(m_ui->txtNewIndexName->text());
        m_currentIndex->setDisplayText(m_currentIndex->getName());
        autoSave();
    }
}

void NewTableForm::onIndexOrderTypeChanged(QString newOrder)
{
    QTreeWidgetItem* selectedCol = m_ui->lstSelectedColumnsForIndex->currentItem();
    if(selectedCol)
    {
        while(selectedCol->parent()) selectedCol = selectedCol->parent();
        selectedCol->setText(1, newOrder);
    }
}

void NewTableForm::prepareSpsTabs()
{
    if(m_table)
    {
        // and now create the tab widgets for the SPs
        WidgetForSpecificProperties* wsp = new WidgetForSpecificProperties(m_dbEngine, m_table, this);
        QVector<SpInstance*> allSps = m_table->getSpInstances(m_dbEngine);
        wsp->feedInSpecificProperties(allSps, uidTable);
        m_ui->tabWidget->insertTab(4, wsp, IconFactory::getMySqlIcon(), "MySql");

        prepareSpsTabsForIndex(0);
        prepareSpsTabsForColumn(0);
    }
}

void NewTableForm::prepareSpsTabsForColumn(Column* col)
{
    // clean the tab
    while(m_ui->tabWidgetForColumnDetails->count() > 1)
    {
        m_ui->tabWidgetForColumnDetails->removeTab(1);
    }

    // TODO: this is very similar to the one from the index below
    m_wspForColumn = new WidgetForSpecificProperties(m_dbEngine, col, this);
    QVector<SpInstance*> allSps;
    if(col)
    {
        allSps = col->getSpInstances(m_dbEngine);
    }
    else
    {
        const QVector<Sp*> allSps1 = m_dbEngine->getDatabaseSpecificProperties();
        for(int i=0; i<allSps1.size(); i++)
        {
            if(allSps1.at(i)->getReferredObjectClassUid() == uidColumn)
            {
                SpInstance* spi = allSps1.at(i)->instantiate();
                allSps.append(spi);
            }
        }
    }

    m_wspForColumn->feedInSpecificProperties(allSps, uidColumn);
    m_wspForColumn->taylorToSpecificObject(col);
    m_ui->tabWidgetForColumnDetails->insertTab(1, m_wspForColumn, IconFactory::getMySqlIcon(), "MySql");

}

void NewTableForm::prepareSpsTabsForIndex(Index* idx)
{
    // clean the tab
    while(m_ui->tabWidgetForIndex->count() > 1)
    {
        m_ui->tabWidgetForIndex->removeTab(1);
    }

    // create the new tab pages
    m_wspForIndex = new WidgetForSpecificProperties(m_dbEngine, idx, this);
    QVector<SpInstance*> allSps;
    if(idx)
    {
        allSps = idx->getSpInstances(m_dbEngine);
    }
    else
    {
        const QVector<Sp*> allSps1 = m_dbEngine->getDatabaseSpecificProperties();
        for(int i=0; i<allSps1.size(); i++)
        {
            if(allSps1.at(i)->getReferredObjectClassUid() == uidIndex)
            {
                SpInstance* spi = allSps1.at(i)->instantiate();
                allSps.append(spi);
            }
        }
    }

    m_wspForIndex->feedInSpecificProperties(allSps, uidIndex);
    m_wspForIndex->taylorToSpecificObject(m_table);
    m_ui->tabWidgetForIndex->insertTab(1, m_wspForIndex, IconFactory::getMySqlIcon(), "MySql");
}

QMenu* NewTableForm::buildPopupForSpsForColumnInIndex()
{
    QMenu* menu = new QMenu(this);
    DatabaseEngine* eng = Workspace::getInstance()->currentProjectsEngine();

    QMenu* mysqlsMenu = menu->addMenu(IconFactory::getMySqlIcon(),
                  eng->getDatabaseEngineName());

    const QVector<Sp*> allSps = m_dbEngine->getDatabaseSpecificProperties();
    for(int i=0; i<allSps.size(); i++)
    {
        if(allSps.at(i)->getReferredObjectClassUid() == uidColumnOfIndex)
        {
            mysqlsMenu->addAction(allSps.at(i)->getPropertyGuiText(), this, SLOT(onTriggerSpItemForIndexesColumn()));
        }
    }
    return menu;
}

void NewTableForm::onAddSpToColumnOfIndex()
{
    QMenu* m = buildPopupForSpsForColumnInIndex();

    m->popup(m_ui->btnAddSpToColumnOfIndex->mapToGlobal(QPoint(0, m_ui->btnAddSpToColumnOfIndex->height())));
}

void NewTableForm::onTriggerSpItemForIndexesColumn()
{
    // do we have a selected item?
    if(m_ui->lstSelectedColumnsForIndex->currentItem() == 0) return;

    // and is this item a column?
    if(!m_table->hasColumn(m_ui->lstSelectedColumnsForIndex->currentItem()->text(0)))
    {
        if(!m_table->parentsHaveColumn(m_ui->lstSelectedColumnsForIndex->currentItem()->text(0)))
        {
            QMessageBox::information(this, tr("Info"), tr("In order to add an SP to a column select the column in the list"), QMessageBox::Ok);
            return;
        }
    }

    QAction *act = qobject_cast<QAction*>(sender());
    if(act)
    {
        const QVector<Sp*> allSps = m_dbEngine->getDatabaseSpecificProperties();
        for(int i=0; i<allSps.size(); i++)
        {
            if(allSps.at(i)->getReferredObjectClassUid() == uidColumnOfIndex)
            {
                if(allSps.at(i)->getPropertyGuiText() == act->text()) // does this column contain already this SP?
                {
                    for(int j=0; j<m_ui->lstSelectedColumnsForIndex->currentItem()->childCount(); j++)
                    {
                        if(m_ui->lstSelectedColumnsForIndex->currentItem()->child(j)->text(0) == act->text())
                        {
                            QMessageBox::warning(this, tr("Warning"), tr("You can add the property \"") + act->text() + "\" only once to a column", QMessageBox::Ok);
                            return;
                        }
                    }

                    // TODO: check that the selected column supports this type of SP
                    Column* c = m_table->getColumn(m_ui->lstSelectedColumnsForIndex->currentItem()->text(0));
                    if(c == 0) c = m_table->getColumnFromParents(m_ui->lstSelectedColumnsForIndex->currentItem()->text(0));
                    if(c == 0) return;
                    if(c->getDataType()->getType() != DT_STRING && c->getDataType()->getType() != DT_BLOB && allSps.at(i)->getSqlRoleUid() == uidMysqlColumnOfIndexLength)
                    {
                        QMessageBox::warning(this, tr("Warning"), tr("You can add the property \"") + act->text() + "\" to string/blob type columns only.", QMessageBox::Ok);
                        return;
                    }

                    QTreeWidgetItem* itm = new QTreeWidgetItem(m_ui->lstSelectedColumnsForIndex->currentItem(), QStringList(act->text()));
                    m_ui->lstSelectedColumnsForIndex->addTopLevelItem(itm);
                    m_ui->lstSelectedColumnsForIndex->header()->resizeSections(QHeaderView::Stretch);
                    m_ui->lstSelectedColumnsForIndex->currentItem()->setExpanded(true);

                    // TODO: when there will be more databases change this!
                    itm->setIcon(0, IconFactory::getMySqlIcon());

                    // TODO: set the control in column 2 according to the type of the SP
                    // TODO: this code is duplicate with code from WidgetForSPS
                    if(allSps.at(i)->getClassUid().toString() == uidValueSp)
                    {   // create a QLineEdit
                        QLineEdit* lstValueSp = new QLineEdit(0);
                        m_ui->lstSelectedColumnsForIndex->setItemWidget(itm, 1, lstValueSp);
                        lstValueSp->setText("0");
                    }
                }
            }
        }
    }
}

void NewTableForm::onRemoveSpsFromColumnOfIndex()
{
    if(m_ui->lstSelectedColumnsForIndex->currentItem() == 0) return;
    const QVector<Sp*> allSps = m_dbEngine->getDatabaseSpecificProperties();
    for(int i=0; i<allSps.size(); i++)
    {
        if(allSps.at(i)->getPropertyGuiText() == m_ui->lstSelectedColumnsForIndex->currentItem()->text(0))
        {
            delete m_ui->lstSelectedColumnsForIndex->currentItem();
        }
    }
}

void NewTableForm::onLockUnlock(bool checked)
{
    if(checked)
    {
        m_ui->btnLock->setIcon(IconFactory::getUnLockedIcon());
        m_ui->grpContent->setEnabled(true);
        m_table->unlock();
        m_table->updateGui();
        m_ui->btnLock->setToolTip(QObject::tr("This table is <b>unlocked</b>. Click this button to lock it."));

        MainWindow::instance()->finallyDoLockLikeOperation(false, m_table->getObjectUid());
    }
    else
    {
        m_ui->btnLock->setIcon(IconFactory::getLockedIcon());
        m_ui->grpContent->setEnabled(false);
        m_table->lock();
        m_table->updateGui();
        m_ui->btnLock->setToolTip(QObject::tr("This table is <b>locked</b>. Click this button to unlock it."));

        MainWindow::instance()->finallyDoLockLikeOperation(true, m_table->getObjectUid());
    }

}

void NewTableForm::onUndelete()
{
    if(m_version->undeleteObject(m_table->getObjectUid(), false))
    {
        MainWindow::instance()->getGuiElements()->removeItemForPatch(m_version->getWorkingPatch(), m_table->getObjectUid());
        // TODO: Duplicate from above
        if(m_table->isLocked())
        {
            m_ui->btnLock->setIcon(IconFactory::getLockedIcon());
            m_ui->btnLock->blockSignals(true);
            m_ui->btnLock->setChecked(false);
            m_ui->btnLock->blockSignals(false);
            m_ui->grpContent->setEnabled(false);
            m_ui->btnLock->setToolTip(QObject::tr("This table is <b>locked</b>. Click this button to unlock it."));
        }
        else
        {
            m_ui->btnLock->setIcon(IconFactory::getUnLockedIcon());
            m_ui->btnLock->blockSignals(true);
            m_ui->btnLock->setChecked(true);
            m_ui->btnLock->blockSignals(false);
            m_ui->grpContent->setEnabled(true);
            m_ui->btnLock->setToolTip(QObject::tr("This table is <b>unlocked</b>. Click this button to lock it."));
        }
        m_ui->btnUndelete->hide();
        m_ui->btnLock->show();
    }
}
