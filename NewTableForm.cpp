#include "NewTableForm.h"
#include "ui_NewTableForm.h"
#include "IconFactory.h"
#include "db_DatabaseEngine.h"
#include "Project.h"
#include "Version.h"
#include "Configuration.h"
#include "UserDataType.h"
#include "Column.h"
#include "Index.h"
#include "Table.h"
#include "MainWindow.h"
#include "db_AbstractIndextypeProvider.h"
#include "db_AbstractStorageEngineListProvider.h"
#include "ForeignKey.h"
#include "StartupValuesHelper.h"
#include "db_AbstractSQLGenerator.h"
#include "SqlHighlighter.h"
#include "InjectSqlDialog.h"
#include "SqlNamesValidator.h"
#include "Workspace.h"
#include "ContextMenuCollection.h"
#include "ClipboardFactory.h"
#include "Codepage.h"
#include "db_AbstractCodepageSupplier.h"
#include "db_AbstractDTSupplier.h"
#include "IssueManager.h"
#include "VersionGuiElements.h"
#include "gui_HelpWindow.h"
#include "core_ConnectionManager.h"

#include <QMessageBox>
#include <QHashIterator>
#include <QHash>
#include <QKeyEvent>
#include <QtGui>
#include <QList>

// the positions of various items in the columns view, used for icon retrieval mostly
const int COL_POS_PK = 0;
const int COL_POS_NM = 1;
const int COL_POS_DT = 2;

NewTableForm::NewTableForm(DatabaseEngine* db, Project* prj, QWidget *parent, bool newTable) : SourceCodePresenterWidget(parent), m_ui(new Ui::NewTableForm),
    m_mw(dynamic_cast<MainWindow*>(parent)), m_dbEngine(db), m_project(prj), m_table(new Table(prj->getWorkingVersion())),
    m_currentColumn(0), m_currentIndex(0), m_foreignTable(0), m_currentForeignKey(0), m_foreignKeySelected(false),
    m_currentStorageEngine(0), m_engineProviders(0)
{
    m_ui->setupUi(this);

    // now set up the Column list and the context menus for the Column list
    lstColumns = new ContextMenuEnabledTreeWidget();
    lstColumns->setObjectName(QString::fromUtf8("lstColumns"));
    lstColumns->setRootIsDecorated(false);
    lstColumns->setItemsExpandable(false);
    lstColumns->setAnimated(false);
    lstColumns->setExpandsOnDoubleClick(false);
    lstColumns->header()->setDefaultSectionSize(150);
    m_ui->columnListLayout->addWidget(lstColumns);
    QTreeWidgetItem *___qtreewidgetitem = lstColumns->headerItem();
    ___qtreewidgetitem->setText(3, QApplication::translate("NewTableForm", "SQL Type", 0, QApplication::UnicodeUTF8));
    ___qtreewidgetitem->setText(2, QApplication::translate("NewTableForm", "Type", 0, QApplication::UnicodeUTF8));
    ___qtreewidgetitem->setText(1, QApplication::translate("NewTableForm", "Name", 0, QApplication::UnicodeUTF8));
    ___qtreewidgetitem->setText(0, QApplication::translate("NewTableForm", "PK", 0, QApplication::UnicodeUTF8));
    ContextMenuHandler* contextMenuHandler = new ContextMenuHandler();
    lstColumns->setItemDelegate(new ContextMenuDelegate(contextMenuHandler,lstColumns));

    // then connect the signals
    QObject::connect(lstColumns, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(onItemSelected(QTreeWidgetItem*,int)));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_CopyColumn(), SIGNAL(activated()), this, SLOT(onCopyColumn()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_PasteColumn(), SIGNAL(activated()), this, SLOT(onPasteColumn()));

    highlighter = new SqlHighlighter(m_ui->txtSql->document(),
                                     Workspace::getInstance()->currentProjectsEngine()->getKeywords(),
                                     Workspace::getInstance()->currentProjectsEngine()->getDTSupplier()->numericTypes(),
                                     Workspace::getInstance()->currentProjectsEngine()->getDTSupplier()->booleanTypes(),
                                     Workspace::getInstance()->currentProjectsEngine()->getDTSupplier()->textTypes(),
                                     Workspace::getInstance()->currentProjectsEngine()->getDTSupplier()->blobTypes(),
                                     Workspace::getInstance()->currentProjectsEngine()->getDTSupplier()->dateTimeTypes(),
                                     Workspace::getInstance()->currentProjectsEngine()->getDTSupplier()->miscTypes(),
                                     Workspace::getInstance()->workingVersion()->getTables());

    const QVector<UserDataType*>& dts = m_project->getWorkingVersion()->getDataTypes();
    for(int i=0; i<dts.size(); i++)
    {
        m_ui->cmbNewColumnType->addItem(dts[i]->getIcon(), dts[i]->getName());
    }

    lstColumns->header()->resizeSection(0, 50);
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
            // at this stage we can put in all the tables, this is supposed to be a new table
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

    // next two: don't change the order. This way the remove is done from the end
    m_ui->tabWidget->removeTab(4);
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
        Workspace::getInstance()->onSaveNewTable(m_table);
        m_ui->txtTableName->setText(m_table->getName());
    }

    populateCodepageCombo();

}

NewTableForm::~NewTableForm()
{
    delete m_ui;
}

void NewTableForm::populateCodepageCombo()
{
    QVector<Codepage*> cps = m_dbEngine->getCodepageSupplier()->getCodepages();
    QListWidget* lw = new QListWidget(this);
    for(int i=0; i<cps.size(); i++)
    {
        QString name = cps[i]->getName();
        bool header = false;
        if(cps[i]->getName().startsWith(QString("--")))
        {
            header = true;
            name = name.right(name.length() - 2);
        }
        QString iconName = "";

        if(!header)
        {
        // dig out the second string
            QStringList ls = name.split("_");
            if(ls.size() > 1)
            {

                if(ls[1] != "bin" && ls[1] != "unicode" && ls[1] != "general")
                {
                    iconName = ":/images/actions/images/small/flag_" + ls[1] + ".png";
                }
                else
                {
                    if(ls[0] == "greek")
                    {
                        iconName = ":/images/actions/images/small/flag_greek.png";
                    }
                    else
                    if(ls[0] == "armscii8")
                    {
                        iconName = ":/images/actions/images/small/flag_armenia.png";
                    }
                    else
                    if(ls[0] == "hebrew")
                    {
                        iconName = ":/images/actions/images/small/flag_israel.png";
                    }
                    else
                    {
                        iconName = ":/images/actions/images/small/flag_" + ls[1] + ".png";
                    }
                }

                ls[1][0] = ls[1][0].toUpper();

                name = ls[1] + " (" + ls[0];
                if(ls.size() > 2)
                {
                    name += ", " + ls[2];
                }
                name += ")";
            }
        }

        // create the lw object
        QListWidgetItem* lwi = new QListWidgetItem(name);
        QFont font = lwi->font();
        if(iconName.length() > 0)
        {
            lwi->setIcon(QIcon(iconName));
        }

        if(header)
        {
            font.setBold(true);
            font.setItalic(true);
            font.setPointSize(font.pointSize() + 1);
        }

        lwi->setFont(font);
        lwi->setData(Qt::UserRole, QVariant(cps[i]->getName()));

        lw->addItem(lwi);
    }

    m_ui->cmbCharSetForSql->setModel(lw->model());
    m_ui->cmbCharSetForSql->setView(lw);
    m_ui->cmbCharSetForSql->setCurrentIndex(-1);

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
            //m_ui->grpForeignKeysList->setEnabled(false);
            //m_ui->grpNewForeignKey->setEnabled(false);
            m_ui->lblForeignKeysNotAllowed->setVisible(true);
        }
    }
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
    return item;
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
    item->setIcon(COL_POS_DT, col->getDataType()->getIcon());
    item->setPopupMenu(ContextMenuCollection::getInstance()->getColumnPopupMenu());
    QVariant var(col->getName());
    item->setData(0, Qt::UserRole, var);
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
            static QBrush grayBrush((QColor(Qt::gray)));
            ContextMenuEnabledTreeWidgetItem* item = createTWIForColumn(columns[i]);
            item->setBackground(0, grayBrush);
            lstColumns->addTopLevelItem(item);
            columns[i]->setLocation(item);
        }
    }
}

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

        if(indices[i]->getName().startsWith("autoidx"))
        {
            item->setBackground(0, QBrush(Qt::gray));
            item->setBackground(1, QBrush(Qt::gray));
            item->setBackground(2, QBrush(Qt::gray));
            item->setBackground(3, QBrush(Qt::gray));
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
    m_ui->chkPersistent->setChecked(table->isPersistent());
    m_ui->chkTemporary->setChecked(table->isTemporary());

    m_currentStorageEngine = table->getStorageEngine();
    if(table->getStorageEngine())
    {
        m_ui->cmbStorageEngine->setCurrentIndex(m_ui->cmbStorageEngine->findText(table->getStorageEngine()->name()));
    }
    populateIndexTypesDependingOnStorageEngine();
    enableForeignKeysDependingOnStorageEngine();
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
    Workspace::getInstance()->workingVersion()->validateVersion(false);
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

        // see if the change has done anything to the issues
        updateIssues();
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

        UserDataType* colsDt = m_project->getWorkingVersion()->getDataType(m_ui->cmbNewColumnType->currentText());
        Column* col = new Column(m_ui->txtNewColumnName->text(), colsDt, m_ui->chkPrimary->isChecked(), m_ui->chkAutoInc->isChecked());
        col->setDescription(m_ui->txtColumnDescription->toPlainText());
        ContextMenuEnabledTreeWidgetItem* item = createTWIForColumn(col);
        lstColumns->addTopLevelItem(item);

        m_table->addColumn(col);
        // and update the table instances of the table, by adding a new table instance
        m_table->tableInstancesAddColumn(col);

        col->setLocation(item);
        m_ui->txtNewColumnName->setFocus();
        updateIssues();
//        Workspace::getInstance()->workingVersion()->checkIssuesOfNewColumn(col, m_table);

    }

    m_ui->txtNewColumnName->setText("");
    m_ui->cmbNewColumnType->setCurrentIndex(-1);
    m_ui->txtColumnDescription->setText("");
    m_ui->chkPrimary->setChecked(false);
    m_ui->chkAutoInc->setChecked(false);
    m_ui->btnAdd->setIcon(IconFactory::getAddIcon());

    lstColumns->resizeColumnToContents(0);
    lstColumns->resizeColumnToContents(1);
    lstColumns->resizeColumnToContents(2);
    lstColumns->resizeColumnToContents(3);

    populateColumnsForIndices();
    updateDefaultValuesTableHeader();
    restoreDefaultValuesTable();

    autoSave();
}

void NewTableForm::onCancelColumnEditing()
{
    m_ui->txtNewColumnName->clear();
    m_ui->cmbNewColumnType->setCurrentIndex(-1);
    m_currentColumn = 0;
    m_ui->chkPrimary->setChecked(false);
    m_ui->chkAutoInc->setChecked(false);
    m_ui->btnAdd->setIcon(IconFactory::getAddIcon());
    toggleColumnFieldDisableness(false);
    m_ui->grpColumnDetails->setTitle(tr("New column"));
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
            finalizeColumnMovement();
        }
    }
}

void NewTableForm::toggleColumnFieldDisableness(bool a)
{
    m_ui->txtNewColumnName->setDisabled(a);
    m_ui->cmbNewColumnType->setDisabled(a);
    m_ui->chkPrimary->setDisabled(a);
    m_ui->chkAutoInc->setDisabled(a);
    m_ui->txtColumnDescription->setDisabled(a);
}

void NewTableForm::showColumn(const Column * c)
{
    if(!m_table->hasColumn(c->getName())) return;
    m_ui->txtNewColumnName->setText(c->getName());
    m_ui->cmbNewColumnType->setCurrentIndex(m_project->getWorkingVersion()->getDataTypeIndex(c->getDataType()->getName()));
    m_ui->chkPrimary->setChecked(c->isPk());
    m_ui->chkAutoInc->setChecked(c->hasAutoIncrement());
    m_ui->txtColumnDescription->setText(c->getDescription());

    m_ui->btnAdd->setIcon(IconFactory::getApplyIcon());
    m_ui->btnCancelColumnEditing->show();

    m_ui->grpColumnDetails->setTitle("Column details");

}

/**
 * Called when a column is selected. onColumnSelect onSelectColumn
 */
void NewTableForm::onItemSelected(QTreeWidgetItem* current, int)
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
        if(m_ui->lstSelectedColumnsForIndex->count() == 0)
        {
            QListWidgetItem* qlwi = new QListWidgetItem(m_table->fullColumns()[i], m_ui->lstAvailableColumnsForIndex);
            Column* col = m_table->getColumn(m_table->fullColumns()[i]);
            if(col)
            {
                qlwi->setIcon(col->getDataType()->getIcon());
            }
            else
            {
                col = m_table->getColumnFromParents(m_table->fullColumns()[i]);
                if(col)
                {
                    qlwi->setIcon(col->getDataType()->getIcon());
                }
            }
        }
        else
        {
            for(int j = 0; j<m_ui->lstSelectedColumnsForIndex->count(); j++)
            {
                if(m_ui->lstSelectedColumnsForIndex->item(j)->text() != m_table->getColumns()[i]->getName())
                {
                    QListWidgetItem* qlwi = new QListWidgetItem(m_table->fullColumns()[i], m_ui->lstAvailableColumnsForIndex);
                    // TODO: duplication with the one above
                    Column* col = m_table->getColumn(m_table->fullColumns()[i]);
                    if(col)
                    {
                        qlwi->setIcon(col->getDataType()->getIcon());
                    }
                    else
                    {
                        col = m_table->getColumnFromParents(m_table->fullColumns()[i]);
                        if(col)
                        {
                            qlwi->setIcon(col->getDataType()->getIcon());
                        }
                    }
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

    Table* other = m_table->version()->getTable(m_ui->txtTableName->text());
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
        if(m_ui->txtNewIndexName->text().length() != 0 && m_ui->lstSelectedColumnsForIndex->count() != 0)
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

void NewTableForm::onReset()
{
}

void NewTableForm::doTheSave()
{
    if(m_project->getWorkingVersion()->hasTable(m_table))
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
    m_table->prepareDiagramEntity();
    m_table->setPersistent(m_ui->chkPersistent->isChecked());
    m_table->setTemporary(m_ui->chkTemporary->isChecked());
    m_table->setDescription(m_ui->txtDescription->toPlainText());
    m_table->setStorageEngine(m_currentStorageEngine);
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
    a.append(index->getType());
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

    if(m_ui->lstSelectedColumnsForIndex->count() == 0)
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

        Index* index = new Index(m_ui->txtNewIndexName->text(), m_ui->cmbIndexType->currentText(), m_table);
        int cnt = m_ui->lstSelectedColumnsForIndex->count();
        for(int i = 0; i< cnt; i++)
        {
            // check if this goes to a parent table or stays here
            Column* col = m_table->getColumn(m_ui->lstSelectedColumnsForIndex->item(i)->text());
            if(col)  // stays here
            {
                index->addColumn(col);
            }
            else
            {
                col = m_table->getColumnFromParents(m_ui->lstSelectedColumnsForIndex->item(i)->text());
                if(col)
                {
                    index->addColumn(col);
                }
                else
                {
                    // something wrent wrong ... we shouldn't be here
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

    autoSave();
}

void NewTableForm::resetIndexGui()
{
    m_ui->lstSelectedColumnsForIndex->clear();
    populateColumnsForIndices();
    m_ui->cmbIndexType->setCurrentIndex(0);
    m_ui->txtNewIndexName->setText("");
    m_ui->btnAddIndex->setIcon(IconFactory::getAddIcon());
    m_currentIndex = 0;

    toggleIndexFieldDisableness(false);
}

void NewTableForm::populateIndexGui(Index* idx)
{
    m_ui->txtNewIndexName->setText(idx->getName());

    // fill up the two lists for the index columns
    m_ui->lstAvailableColumnsForIndex->clear();
    m_ui->lstSelectedColumnsForIndex->clear();

    for(int i=0; i< m_table->fullColumns().size(); i++)
    {
        Column* column = m_table->getColumn(m_table->fullColumns()[i]);
        if(!column)
        {
            column = m_table->getColumnFromParents(m_table->fullColumns()[i]);
            if(!column) // this shouldn't be
            {
                return;
            }
        }
        QListWidget *targetList = 0;
        if(idx->hasColumn(column))
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
        if(*it == idx->getType())
        {
            break;
        }
        indx ++;
        it ++;
    }
    m_ui->cmbIndexType->setCurrentIndex(indx);
}

void NewTableForm::toggleIndexFieldDisableness(bool a)
{
    m_ui->txtNewIndexName->setDisabled(a);
    m_ui->lstAvailableColumnsForIndex->setDisabled(a);
    m_ui->lstSelectedColumnsForIndex->setDisabled(a);
    m_ui->cmbIndexType->setDisabled(a);

    m_ui->btnMoveColumnToLeft->setDisabled(a);
    m_ui->btnMoveColumnToRight->setDisabled(a);
    m_ui->btnMoveIndexColumnUp->setDisabled(a);
    m_ui->btnMoveIndexColumnDown->setDisabled(a);
}

void NewTableForm::onSelectIndex(QTreeWidgetItem*, int)
{
    QModelIndex x = m_ui->lstIndices->currentIndex();
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

    if(m_ui->lstIndices->currentItem()->text(0).startsWith("autoidx"))
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
    QStringList foreignColumns = table->fullColumns();
    m_ui->lstForeignTablesColumns->clear();
    for(int i=0; i<foreignColumns.size(); i++)
    {
        QListWidgetItem* qlwi = new QListWidgetItem(foreignColumns[i], m_ui->lstForeignTablesColumns);
        // TODO: these few lines below are duplicates with populateColumnsForIndices
        Column* col = table->getColumn(foreignColumns[i]);
        if(col)
        {
            qlwi->setIcon(col->getDataType()->getIcon());
        }
        else
        {
            col = table->getColumnFromParents(foreignColumns[i]);
            if(col)
            {
                qlwi->setIcon(col->getDataType()->getIcon());
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
                    //qDebug() << col->getName() << " is " << col->getDataType()->getName() << " >> " << foreignColumn->getName() << " is " << foreignColumn->getDataType()->getName();
                    QListWidgetItem* qlwj = new QListWidgetItem(parentColumns[j], m_ui->lstLocalColumn);
                    qlwj->setIcon(col->getDataType()->getIcon());
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
                        qlwj->setIcon(col->getDataType()->getIcon());
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

    item->setIcon(0, cforeignColumn->getDataType()->getIcon());
    item->setIcon(1, clocalColumn->getDataType()->getIcon());

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
        Table* tbl = m_project->getWorkingVersion()->getTable(fktName);
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

        item->setIcon(0, assocs[i]->getForeignColumn()->getDataType()->getIcon());
        item->setIcon(1, assocs[i]->getLocalColumn()->getDataType()->getIcon());

        m_ui->lstForeignKeyAssociations->addTopLevelItem(item);
    }

    m_ui->lstForeignKeyAssociations->resizeColumnToContents(0);
    m_ui->lstForeignKeyAssociations->resizeColumnToContents(1);

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
    QStringList foreignColumns = table->fullColumns();
    m_ui->lstForeignTablesColumns->clear();
    for(int i=0; i<foreignColumns.size(); i++)
    {
        QListWidgetItem* qlwi = new QListWidgetItem(foreignColumns[i], m_ui->lstForeignTablesColumns);

        Column* col = table->getColumn(foreignColumns[i]);
        if(col)
        {
            qlwi->setIcon(col->getDataType()->getIcon());
        }
        else
        {
            col = table->getColumnFromParents(foreignColumns[i]);
            if(col)
            {
                qlwi->setIcon(col->getDataType()->getIcon());
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
        columnHeaderItem->setIcon(m_table->getColumns()[i]->getDataType()->getIcon());
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
                QString cName = m_ui->tableStartupValues->horizontalHeaderItem(i)->text();
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
    QString s = m_ui->cmbCharSetForSql->itemData(m_ui->cmbCharSetForSql->currentIndex()).toString();
    if(s.indexOf('_') == -1) s = "latin1";
    s=s.left(s.indexOf('_'));
    presentSql(m_project, s);
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

    updateSqlDueToChange();
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
    m_table->version()->removeForeignKeyFromDiagrams(m_currentForeignKey);
    delete m_currentForeignKey->getLocation();
    m_currentForeignKey = 0;
    resetFkGui();
    autoSave();
}

void NewTableForm::onPersistentChange(int a)
{
    m_table->setPersistent(a == Qt::Checked);
    m_table->prepareDiagramEntity();
}

void NewTableForm::onTemporaryChange(int a)
{
    m_table->setTemporary(a == Qt::Checked);
    m_table->prepareDiagramEntity();
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
    Workspace::getInstance()->workingVersion()->getGui()->getMainWindow()->addDockWidget(Qt::RightDockWidgetArea, hw);
}

void NewTableForm::onChangeTab(int idx)
{
    if(idx > 0)
    {
        if(m_ui->tabWidget->tabText(idx) == "SQL")
        {
            presentSql(m_project, m_ui->cmbCharacterSets->currentText());
        }
    }
}

void NewTableForm::onChangeDescription()
{
    m_table->setDescription(m_ui->txtDescription->toPlainText());
}

void NewTableForm::onChangeName(QString a)
{
    QVariant v(a);
    if(!m_table) return;
    QString prevName = m_table->getName();
    if(!m_table->getLocation()) return;
    m_table->getLocation()->setData(0, Qt::UserRole, v);
    m_table->setName(a);
    m_table->getLocation()->setText(0, a);
    updateSqlDueToChange();
    // and see if this was due to a new table being focused ... weird check.
    if(prevName != a) updateIssues();
}

void NewTableForm::onInject()
{
    InjectSqlDialog* injectDialog = new InjectSqlDialog(Workspace::getInstance()->currentProjectsEngine(), this);
    injectDialog->selectCodePage(m_ui->cmbCharSetForSql->currentIndex());
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
                QString host = c->getHost();
                QString user = c->getUser();
                QString pass = c->getPassword();
                QString db = c->getDb();
                if(!m_dbEngine->injectSql(host, user, pass, db, finalSql, tSql, injectDialog->getRollbackOnError(), injectDialog->getCreateOnlyIfNotExist()))
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

void NewTableForm::presentSql(Project *, const QString& codepage)
{
    QString fs = "";
    QHash<QString,QString> fo = Configuration::instance().sqlGenerationOptions();
    fo["FKSposition"] = "OnlyInternal";
    finalSql = m_project->getEngine()->getSqlGenerator()->generateCreateTableSql(m_table, fo, m_table->getName(), codepage);
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

    // TODO: This is duplicate with stuff from the "onAddcolumn"
    col->setName(col->getName() + "_copy");
    col->setName(m_table->generateUniqueColumnName(col->getName()));
    // TODO: Rename the column to be valid!
    ContextMenuEnabledTreeWidgetItem* item = createTWIForColumn(col);
    lstColumns->addTopLevelItem(item);

    m_table->addColumn(col);
    // and update the table instances of the table, by adding a new table instance
    m_table->tableInstancesAddColumn(col);

    col->setLocation(item);
    m_ui->txtNewColumnName->setFocus( );
    // till here
}

void NewTableForm::onCodepageChange(QString)
{
    updateSqlDueToChange();
}

void NewTableForm::presentSql(Project*, SqlSourceEntity*,const QString&)
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

        m_currentColumn->setDataType(m_project->getWorkingVersion()->getDataType(m_ui->cmbNewColumnType->currentText()));
        m_currentColumn->getLocation()->setIcon(COL_POS_DT, m_currentColumn->getDataType()->getIcon());
        m_currentColumn->getLocation()->setText(COL_POS_DT, m_currentColumn->getDataType()->getName());

        autoSave();
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
        m_currentIndex->getLocation()->setText(0, m_currentIndex->getName());
        autoSave();
    }
}
