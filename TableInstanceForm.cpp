#include "TableInstanceForm.h"
#include "ui_TableInstanceForm.h"
#include "TableInstance.h"
#include "UserDataType.h"
#include "StartupValuesHelper.h"
#include "Column.h"
#include "utils.h"
#include "Workspace.h"
#include "Version.h"
#include "VersionGuiElements.h"
#include "MainWindow.h"
#include "gui_HelpWindow.h"

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

        // TODO: Fix this in a way that it will work with the SP
//        if(c->hasAutoIncrement())
//        {
//            int lastR = ui->values->rowCount() - 1; // number of preceeding rows, the last one is empty
//            int lastV = -1;
//            if(lastR != 0)
//            {
//                lastV = ui->values->item(lastR-1,i)->text().toInt();
//            }
//            lastV ++;
//            QTableWidgetItem* twi = new QTableWidgetItem(QString::number(lastV));
//            ui->values->setItem(lastR, i, twi);
//        }
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


void TableInstanceForm::onHelp()
{
    HelpWindow* hw = HelpWindow::instance();
    hw->showHelp(QString("/doc/tinst.html"));
    hw->show();
}

void TableInstanceForm::onValuesDoubleClick()
{
    onAddNewRow();
}

