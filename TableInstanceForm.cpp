#include "TableInstanceForm.h"
#include "ui_TableInstanceForm.h"
#include "TableInstance.h"
#include "UserDataType.h"
#include "StartupValuesHelper.h"
#include "Column.h"

template <class T> const T& max ( const T& a, const T& b ) {
  return (b<a)?a:b;
}

TableInstanceForm::TableInstanceForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TableInstanceForm), m_populated(false)
{
    ui->setupUi(this);
    ui->btnImportValues->hide();

    ui->grpHelp->hide();

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
        columnHeaderItem->setIcon(m_tinst->table()->getDataTypeOfColumn(columns[i])->getIcon());
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
            qDebug() << v[j];
            ui->values->setItem(j, i, itm);
        }
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
                    ui->values->item(j,i)->setToolTip("This column type does not support this value");
                }
            }
            else
            {
                a.append("");
            }
        }
        v[ui->values->horizontalHeaderItem(i)->text()] = a;
    }

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
    ui->grpHelp->show();
    ui->btnHelp->hide();
    ui->webView->setUrl(QApplication::applicationDirPath() + QString("/doc/tinst.html"));
}

void TableInstanceForm::onValuesDoubleClick()
{
    onAddNewRow();
}

