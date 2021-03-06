#include "TableInstanceForm.h"
#include "ui_TableInstanceForm.h"
#include "core_TableInstance.h"
#include "core_UserDataType.h"
#include "StartupValuesHelper.h"
#include "core_Column.h"
#include "utils.h"
#include "Workspace.h"
#include "Version.h"
#include "VersionGuiElements.h"
#include "MainWindow.h"
#include "gui_HelpWindow.h"
#include "SpInstance.h"
#include "GuiElements.h"
#include "uids.h"
#include "core_ForeignKey.h"
#include "ddm_strings.h"
#include "SqlNamesValidator.h"
#include "db_DatabaseEngine.h"

#include <QLineEdit>
#include <QComboBox>
#include <QToolTip>
#include <QMessageBox>

TableInstanceForm::TableInstanceForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TableInstanceForm), m_populated(false)
{
    ui->setupUi(this);
    ui->btnImportValues->hide();
    ui->tabWidget->setCurrentIndex(0);
    m_signalMapperForFKTinstCombos = new QSignalMapper(this);

    ui->lineEdit->setValidator(new SqlNamesValidator());
}

TableInstanceForm::~TableInstanceForm()
{
    delete ui;
}

void TableInstanceForm::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void TableInstanceForm::createTableWithValues()
{
    QVector<QString> columns = m_tinst->columns();
    ui->values->setColumnCount(columns.size());
    bool rowcSet = false;
    // header
    for(int i=0; i<columns.size(); i++)
    {
        QTableWidgetItem *columnHeaderItem = new QTableWidgetItem(columns[i]);
        columnHeaderItem->setIcon(IconFactory::getIconForDataType(m_tinst->table()->getDataTypeOfColumn(columns[i])->getType()));
        columnHeaderItem->setTextAlignment(Qt::AlignVCenter);

        QVector <QString> v = m_tinst->values()[columns[i]];

        ui->values->setHorizontalHeaderItem(i, columnHeaderItem);
        if(!rowcSet)
        {
            ui->values->setRowCount(v.size());
            rowcSet = true;
        }

        for(int j=0; j<v.size(); j++)
        {
            QTableWidgetItem* itm = new QTableWidgetItem(v[j]);
            ui->values->setItem(j, i, itm);
        }
        ui->values->resizeColumnsToContents();
    }
    m_populated = true;
}


void TableInstanceForm::onLoadValuesFromCSV()
{
    loadStartupValuesFromCSVIntoTable(ui->values, this);
    onValidateData();
}

void TableInstanceForm::onSaveValuesToCSV()
{
    saveStartupValuesToCSVFromTable(ui->values, this);
}

void TableInstanceForm::onAddNewRow()
{
    addNewRowToTable(ui->values, m_tinst->table());

    // now see that if we have a cell with Auto Increment the next value gets inserted there
    for(int i=0; i<ui->values->columnCount(); i++)
    {
        QString cName = ui->values->horizontalHeaderItem(i)->text();
        Column *c = m_tinst->table()->getColumn(cName);
        if(c == 0)
        {
            c = m_tinst->table()->getColumnFromParents(cName);
            if(c == 0) return;
        }

        bool ainc = false;
        {
            SpInstance* spi = c->getInstanceForSqlRoleUid(Workspace::getInstance()->currentProjectsEngine(), uidColumnAutoIncrement);
            if(spi)
            {
                QString autoInc = spi->get();
                if(autoInc == strTrue)
                {
                    ainc = true;
                }
            }
        }

        if(ainc)
        {
            int lastR = ui->values->rowCount() - 1; // number of preceeding rows, the last one is empty
            int lastV = -1;
            if(lastR != 0)
            {
                for(int j = 0; j<lastR; j++)
                {
                    lastV = ui->values->item(j, i)->text().toInt() > lastV ?
                                ui->values->item(j, i)->text().toInt() :
                                lastV;
                }
            }
            lastV ++;
            QTableWidgetItem* twi = new QTableWidgetItem(QString::number(lastV));
            ui->values->setItem(lastR, i, twi);
        }
    }
}

void TableInstanceForm::onDeleteRow()
{
    ui->values->removeRow(ui->values->currentRow());
    onValidateData();
}

void TableInstanceForm::onValidateData()
{
    QHash < QString, QVector<QString> > v;
    bool errorFound = false;

    // 0. reset
    for(int i=0; i<ui->values->columnCount(); i++)
    {
        for(int j=0; j<ui->values->rowCount(); j++)
        {
            if(ui->values->item(j,i))
            {
                ui->values->item(j,i)->setBackgroundColor(Qt::white);
                ui->values->item(j,i)->setToolTip("");
            }
        }
    }

    // 1. check the type
    for(int i=0; i<ui->values->columnCount(); i++)
    {
        QVector<QString> a ;
        for(int j=0; j<ui->values->rowCount(); j++)
        {
            if(ui->values->item(j,i))
            {
                QString cName = ui->values->horizontalHeaderItem(i)->text();
                Column *c = m_tinst->table()->getColumn(cName);
                if(c == 0)
                {
                    c = m_tinst->table()->getColumnFromParents(cName);
                    if(c == 0) return;
                }
                const UserDataType* dt = c->getDataType();
                if(dt->isValid(ui->values->item(j,i)->text()))
                {
                    a.append(ui->values->item(j,i)->text());
                    ui->values->item(j,i)->setBackgroundColor(Qt::white);
                    ui->values->item(j,i)->setToolTip("");
                }
                else
                {
                    ui->values->item(j,i)->setBackgroundColor(Qt::red);
                    errorFound = true;
                    ui->values->item(j,i)->setToolTip(tr("This column type does not support this value"));
                }
            }
            else
            {
                a.append("");
            }
        }
        v[ui->values->horizontalHeaderItem(i)->text()] = a;
    }

    // 2. check the primary keys
    // 2.1. build a vector of primary key columns
    QVector<QString> pkcols;
    QStringList allColumns = m_tinst->table()->fullColumns();
    for(int i=0; i<allColumns.size(); i++)
    {
        Column *c = m_tinst->table()->getColumn(allColumns.at(i));
        if(c == 0)
        {
            c = m_tinst->table()->getColumnFromParents(allColumns.at(i));
            if(c == 0) return;
        }
        if(c->isPk())
        {
            pkcols.append(c->getName());
        }
    }

    // 2.2. gather all the PKs and check
    QVector<QString> containedPks;
    for(int i=0; i<ui->values->rowCount(); i++)
    {
        QString cpk = "";
        for(int j=0; j<ui->values->columnCount(); j++)
        {
            if(ui->values->item(i,j))
            {
                QString cName = ui->values->horizontalHeaderItem(j)->text();
                if(pkcols.contains(cName))
                {
                    cpk += ui->values->item(i,j)->text();
                }
            }
        }
        if(containedPks.contains(cpk))
        {
            for(int j=0; j<ui->values->columnCount(); j++)
            {
                if(ui->values->item(i,j))
                {
                    QString cName = ui->values->horizontalHeaderItem(j)->text();
                    if(pkcols.contains(cName))
                    {
                        ui->values->item(i,j)->setBackgroundColor(Qt::red);
                        errorFound = true;
                        ui->values->item(i,j)->setToolTip(tr("Duplicate Primary Key"));
                    }
                }
            }
        }
        else
        {
            containedPks.append(cpk);
        }
    }

    // 3. Check that the foreign keys of this table actually refer to existing foreign keys in other table instances

    if(!errorFound)
    {
        m_tinst->setValues(v);
    }
}

void TableInstanceForm::onCellChange(int,int)
{
    if(m_populated)
    {
        onValidateData();
    }
}

void TableInstanceForm::onValuesDoubleClick()
{
    onAddNewRow();
}

void TableInstanceForm::onGotoTable()
{
    MainWindow::instance()->showTableWithGuid(m_tinst->version(), m_tinst->table()->getObjectUid().toString());
}


void TableInstanceForm::onLockUnlock(bool checked)
{
    if(checked)
    {
        ui->btnLock->setIcon(IconFactory::getUnLockedIcon());
        ui->grpContentMain->setEnabled(true);
        m_tinst->unlock();
        m_tinst->updateGui();
        ui->btnLock->setToolTip(QObject::tr("This table instance is <b>unlocked</b>. Click this button to lock it."));

        MainWindow::instance()->finallyDoLockLikeOperation(false, m_tinst->getObjectUid().toString());
    }
    else
    {
        ui->btnLock->setIcon(IconFactory::getLockedIcon());
        ui->grpContentMain->setEnabled(false);
        m_tinst->lock(LockableElement::LOCKED);
        m_tinst->updateGui();
        ui->btnLock->setToolTip(QObject::tr("This table instance is <b>locked</b>. Click this button to unlock it."));

        MainWindow::instance()->finallyDoLockLikeOperation(true, m_tinst->getObjectUid().toString());
    }
}

void TableInstanceForm::disableEditingControls(bool dis)
{
    ui->btnLoadStartupValues->setDisabled(dis);
    ui->btnAddNewDefaultRow->setDisabled(dis);
    ui->btnImportValues->setDisabled(dis);
    ui->btnDeleteRow->setDisabled(dis);
    ui->values->setDisabled(dis);
    ui->lineEdit->setDisabled(dis);
    ui->treeWidget->setDisabled(dis);
}

void TableInstanceForm::setTableInstance(TableInstance *st)
{
    m_tinst = st;
    ui->btnUndelete->hide();
    ui->lineEdit->setText(m_tinst->getName());

    // fill in the foreign keys
    Table* t = st->table();
    const QVector<ForeignKey*> &fks = t->getForeignKeys();
    for(int i=0; i<fks.size(); i++)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(QStringList(fks[i]->getName()));
        item->setIcon(0, IconFactory::getForeignKeyIcon());

        QComboBox* cmbGoesToTable = new QComboBox(0);
        cmbGoesToTable->setAutoFillBackground(true);
        // find all the table instances instantiated from fks[i]->getForeingTable()

        const QVector<TableInstance*> &otherTinsts = st->version()->getTableInstances();
        QString first = "";

        for(int j=0; j<otherTinsts.size(); j++)
        {
            TableInstance* tabInstJ = otherTinsts.at(j);
            if(tabInstJ->table()->getObjectUid() == fks[i]->getForeignTable()->getObjectUid())  // same tab
            {
                cmbGoesToTable->addItem(IconFactory::getTableIcon(), tabInstJ->getName());
                if(first.isEmpty()) first = tabInstJ->getName();
            }
        }
        ui->treeWidget->addTopLevelItem(item);
        ui->treeWidget->setItemWidget(item, 1, cmbGoesToTable);

        m_combos.insert(fks[i]->getName(), cmbGoesToTable);
        connect(cmbGoesToTable, SIGNAL(activated(QString)), m_signalMapperForFKTinstCombos, SLOT(map()));
        m_signalMapperForFKTinstCombos->setMapping(cmbGoesToTable, fks[i]->getName());
        connect(m_signalMapperForFKTinstCombos, SIGNAL(mapped(const QString&)), this, SLOT(onTInstSelectedForFk(const QString&)));

        if(!st->hasFkMappingFor(fks[i]->getName()))
        {
            st->setFkMappingToTinst(fks[i]->getName(), first);
        }
        else
        {
            cmbGoesToTable->setCurrentIndex(cmbGoesToTable->findText(st->getTinstForFk(fks[i]->getName())));
        }
    }

    // TODO: Duplicate from the other forms ... more or less
    if(!m_tinst->wasLocked())
    {
        if(m_tinst->isDeleted())
        {
            ui->frameForUnlockButton->show();
            ui->btnLock->hide();
            ui->btnUndelete->show();
        }
        else
        {
            ui->frameForUnlockButton->hide();
        }
    }
    else
    {
        ui->frameForUnlockButton->show();
        if(m_tinst->lockState() == LockableElement::LOCKED)
        {
            ui->btnLock->setIcon(IconFactory::getLockedIcon());
            ui->btnLock->blockSignals(true);
            ui->btnLock->setChecked(false);
            ui->btnLock->blockSignals(false);
            disableEditingControls(true);
            ui->btnLock->setToolTip(QObject::tr("This table instance is <b>locked</b>. Click this button to unlock it."));
        }
        else
        if(m_tinst->lockState() == LockableElement::FINAL_LOCK)
        {
            ui->btnLock->setIcon(IconFactory::getFinalLockedIcon());
            ui->btnLock->blockSignals(true);
            ui->btnLock->setChecked(false);
            disableEditingControls(true);
            ui->btnLock->setCheckable(false);
            ui->btnLock->setToolTip(QObject::tr("This table instance is in a <b>final lock</b> stage. You cannot modify it."));
        }
        else
        {
            ui->btnLock->setIcon(IconFactory::getUnLockedIcon());
            ui->btnLock->blockSignals(true);
            ui->btnLock->setChecked(true);
            ui->btnLock->blockSignals(false);
            disableEditingControls(false);
            ui->btnLock->setToolTip(QObject::tr("This table instance is <b>unlocked</b>. Click this button to lock it."));
        }

        if(m_tinst->isDeleted())
        {
            ui->btnLock->hide();
            ui->btnUndelete->show();
        }
    }
}

void TableInstanceForm::onTInstSelectedForFk(const QString& d)
{
    if(m_tinst)
    {
        m_tinst->setFkMappingToTinst(d, m_combos[d]->currentText());
    }
}

void TableInstanceForm::onUndelete()
{
    QString tempError;
    if(Version::DO_NOT_REMOVE_FROM_PATCH_TREE_FAILURE != m_tinst->version()->undeleteObject(m_tinst->getObjectUid().toString(), false, tempError))
    {
        MainWindow::instance()->getGuiElements()->removeItemForPatch(m_tinst->version()->getWorkingPatch(), m_tinst->getObjectUid().toString());
        // TODO: Duplicate from above
        if(m_tinst->lockState() == LockableElement::LOCKED)
        {
            ui->btnLock->setIcon(IconFactory::getLockedIcon());
            ui->btnLock->blockSignals(true);
            ui->btnLock->setChecked(false);
            ui->btnLock->blockSignals(false);
            disableEditingControls(true);
            ui->btnLock->setToolTip(QObject::tr("This table instance is <b>locked</b>. Click this button to unlock it."));
        }
        else
        {
            ui->btnLock->setIcon(IconFactory::getUnLockedIcon());
            ui->btnLock->blockSignals(true);
            ui->btnLock->setChecked(true);
            ui->btnLock->blockSignals(false);
            disableEditingControls(false);
            ui->btnLock->setToolTip(QObject::tr("This table instance is <b>unlocked</b>. Click this button to lock it."));
        }
        ui->btnUndelete->hide();
        ui->btnLock->show();
    }
    else
    {
        QMessageBox::critical(this, tr("Error"), tempError);
    }
}

void TableInstanceForm::onChangeName(QString a)
{
    a = a.trimmed();
    if(a.isEmpty())
    {
        return;
    }

    QPalette pal;
    pal.setColor(QPalette::Text, Qt::black);
    ui->lineEdit->setPalette(pal);
    TableInstance* other = m_tinst->version()->getTableInstance(a);
    // and see if there is a table, column or KEYWORD called "a"
    if(m_tinst->version()->getProject()->getEngine()->getKeywords().contains(a, Qt::CaseInsensitive) || (other && other != m_tinst) )
    {
        QPalette pal;
        pal.setColor(QPalette::Text, Qt::red);
        ui->lineEdit->setPalette(pal);

        QString netTooltip = tr("This is not an allowed name for the table.");
        if(m_tinst->version()->getProject()->getEngine()->getKeywords().contains(a, Qt::CaseInsensitive))
        {
            netTooltip += "<p><b>" + a + "</b>" + tr(" is a reserved keyword for this database. DDM does not allow this to avoid future confusion.");
        }
        else
        {
            netTooltip += "<p>" + tr("There is already a table instance called <b> ") + a;
        }

        QToolTip::showText(ui->lineEdit->mapToGlobal(QPoint()), netTooltip, ui->lineEdit);
        ui->lineEdit->setToolTip(netTooltip);

        return;
    }
    else
    {
        ui->lineEdit->setToolTip(tr("TThe name of the table instance"));
    }

    m_tinst->setName(a);
    m_tinst->setDisplayText(a);
    m_tinst->getSqlLocation()->setText(0, a);

    if(m_tinst->version() != Workspace::getInstance()->workingVersion())
    {
        MainWindow::instance()->getGuiElements()->updateItemForPatchWithState(m_tinst->version()->getWorkingPatch(), uidTableInstance, m_tinst->getObjectUid().toString(), a, 2);
    }

}
