#include "TableInstanceForm.h"
#include "ui_TableInstanceForm.h"
#include "TableInstance.h"
#include "UserDataType.h"

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
