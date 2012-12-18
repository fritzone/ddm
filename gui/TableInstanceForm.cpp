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

#include <QLineEdit>

TableInstanceForm::TableInstanceForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TableInstanceForm), m_populated(false)
{
    ui->setupUi(this);
    ui->btnImportValues->hide();
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
            SpInstance* spi = c->getInstanceForSqlRoleUid(Workspace::getInstance()->currentProjectsEngine(), uidMysqlColumnAutoIncrement);
            if(spi)
            {
                QString autoInc = spi->get();
                if(autoInc == "TRUE")
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
    MainWindow::instance()->showTableWithGuid(m_tinst->version(), m_tinst->table()->getObjectUid());
}


void TableInstanceForm::onLockUnlock(bool checked)
{
    if(checked)
    {
        ui->btnLock->setIcon(IconFactory::getUnLockedIcon());
        ui->grpContent->setEnabled(true);
        m_tinst->unlock();
        m_tinst->updateGui();
        ui->btnLock->setToolTip(QObject::tr("This table instance is <b>unlocked</b>. Click this button to lock it."));

        MainWindow::instance()->finallyDoLockLikeOperation(false, m_tinst->getObjectUid());
    }
    else
    {
        ui->btnLock->setIcon(IconFactory::getLockedIcon());
        ui->grpContent->setEnabled(false);
        m_tinst->lock(LockableElement::LOCKED);
        m_tinst->updateGui();
        ui->btnLock->setToolTip(QObject::tr("This table instance is <b>locked</b>. Click this button to unlock it."));

        MainWindow::instance()->finallyDoLockLikeOperation(true, m_tinst->getObjectUid());
    }
}

void TableInstanceForm::disableEditingControls(bool dis)
{
    ui->btnLoadStartupValues->setDisabled(dis);
    ui->btnAddNewDefaultRow->setDisabled(dis);
    ui->btnImportValues->setDisabled(dis);
    ui->btnDeleteRow->setDisabled(dis);
}

void TableInstanceForm::setTableInstance(TableInstance *st)
{
    m_tinst = st;
    ui->btnUndelete->hide();
    ui->lineEdit->setText(m_tinst->getName());

    // TODO: Duplicate from the other forms
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


void TableInstanceForm::onUndelete()
{
    if(m_tinst->version()->undeleteObject(m_tinst->getObjectUid(), false))
    {
        MainWindow::instance()->getGuiElements()->removeItemForPatch(m_tinst->version()->getWorkingPatch(), m_tinst->getObjectUid());
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
}

void TableInstanceForm::onChangeName(QString t)
{
    m_tinst->setName(t);
    m_tinst->setDisplayText(t);
    m_tinst->getSqlLocation()->setText(0, t);

    if(m_tinst->version() != Workspace::getInstance()->workingVersion())
    {
        MainWindow::instance()->getGuiElements()->updateItemForPatchWithState(m_tinst->version()->getWorkingPatch(), uidTableInstance, m_tinst->getObjectUid(), t, 2);
    }

}
