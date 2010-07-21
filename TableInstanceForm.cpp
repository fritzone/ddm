#include "TableInstanceForm.h"
#include "ui_TableInstanceForm.h"
#include "TableInstance.h"
#include "UserDataType.h"
#include "StartupValuesHelper.h"

TableInstanceForm::TableInstanceForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TableInstanceForm)
{
    ui->setupUi(this);
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
    QList<QString> columns = m_tinst->columns();
    ui->values->setColumnCount(columns.size());
    for(int i=0; i<columns.size(); i++)
    {
        QTableWidgetItem *columnHeaderItem = new QTableWidgetItem(columns[i]);
        columnHeaderItem->setIcon(m_tinst->table()->getDataTypeOfColumn(columns[i])->getIcon());
        columnHeaderItem->setTextAlignment(Qt::AlignVCenter);

        ui->values->setHorizontalHeaderItem(i, columnHeaderItem);

    }
}


void TableInstanceForm::onLoadValuesFromCSV()
{
    loadStartupValuesFromCSVIntoTable(ui->values, this);
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
}

void TableInstanceForm::onValidateData()
{
    QHash < QString, QVector<QString> > v;
    for(int i=0; i<ui->values->columnCount(); i++)
    {
        qDebug() << ui->values->horizontalHeaderItem(i)->text();

        QVector<QString> a ;
        for(int j=0; j<ui->values->rowCount(); j++)
        {
            if(ui->values->item(j,i))
            {
                a.append(ui->values->item(j,i)->text());
            }
            else
            {
                a.append("");
            }
            qDebug() << a;
        }
        v[ui->values->horizontalHeaderItem(i)->text()] = a;
    }

    m_tinst->setValues(v);

}
