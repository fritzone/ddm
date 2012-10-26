#include "TableComparisonForm.h"
#include "ui_TableComparisonForm.h"
#include "core_Table.h"

TableComparisonForm::TableComparisonForm(QWidget *parent) : m_leftTable(0), m_rightTable(0),
    QWidget(parent),
    ui(new Ui::TableComparisonForm)
{
    ui->setupUi(this);

    m_spl = new QSplitter(Qt::Horizontal, this);
    m_spl->addWidget(ui->grpLeft);
    m_spl->addWidget(ui->grpRight);
    ui->horizontalLayout->addWidget(m_spl);

    m_trees[LEFT] = ui->treeWidgetLeft;
    m_trees[RIGHT] = ui->treeWidgetRight;
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

    populateColumns();
}

void TableComparisonForm::populateColumns()
{
    QTreeWidgetItem* colItemLeft = new QTreeWidgetItem(QStringList("Columns"));
    m_trees[LEFT]->addTopLevelItem(colItemLeft);
    QTreeWidgetItem* colItemRight = new QTreeWidgetItem(QStringList("Columns"));
    m_trees[RIGHT]->addTopLevelItem(colItemRight);
}
