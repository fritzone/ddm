#include "TableComparisonForm.h"
#include "ui_TableComparisonForm.h"
#include "core_Table.h"

TableComparisonForm::TableComparisonForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TableComparisonForm)
{
    ui->setupUi(this);

    m_spl = new QSplitter(Qt::Horizontal, this);
    m_spl->addWidget(ui->grpLeft);
    m_spl->addWidget(ui->grpRight);
    ui->horizontalLayout->addWidget(m_spl);
}

TableComparisonForm::~TableComparisonForm()
{
    delete ui;
}

void TableComparisonForm::populateTree()
{
    ui->txtTab1->setText(m_leftTable->getName());
    if(m_rightTable) ui->txtTab2->setText(m_rightTable->getName());
    else ui->txtTab2->setText("Deleted");
}
