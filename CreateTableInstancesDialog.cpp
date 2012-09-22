#include "ui_CreateTableInstancesDialog.h"
#include "CreateTableInstancesDialog.h"
#include "IconFactory.h"

#include <QListWidgetItem>

CreateTableInstancesDialog::CreateTableInstancesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateTableInstancesDialog)
{
    ui->setupUi(this);
}

CreateTableInstancesDialog::~CreateTableInstancesDialog()
{
    delete ui;
}

void CreateTableInstancesDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void CreateTableInstancesDialog::addTable(const QString& tabName)
{
    QListWidgetItem* item = new QListWidgetItem(tabName, ui->listWidget);
    item->setIcon(IconFactory::getTablesIcon());
    item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    item->setCheckState(Qt::Unchecked);
}

QStringList CreateTableInstancesDialog::getSelectedTables()
{
    QStringList res;

    for(int i=0; i<ui->listWidget->count(); i++)
    {
        if(ui->listWidget->item(i)->checkState() == Qt::Checked)
        {
            res.append(ui->listWidget->item(i)->text());
        }
    }
    return res;
}

void CreateTableInstancesDialog::onSelectAllChanged(int a)
{
    if(a == Qt::Checked || a == Qt::Unchecked )
    {
        for(int i=0; i<ui->listWidget->count(); i++)
        {
            ui->listWidget->item(i)->setCheckState((Qt::CheckState)a);
        }
    }}
