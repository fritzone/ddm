#include "TableInstancesListForm.h"
#include "ui_TableInstancesListForm.h"

#include "mainwindow.h"
#include "TableInstance.h"
#include "IconFactory.h"

#include <QTreeWidgetItem>

TableInstancesListForm::TableInstancesListForm(QWidget *parent) : QWidget(parent),  mw(reinterpret_cast<MainWindow*>(parent)), ui(new Ui::TableInstancesListForm)
{
    ui->setupUi(this);

    ui->btnUp->hide();
    ui->btnDown->hide();
    ui->btnRemove->hide();
}

TableInstancesListForm::~TableInstancesListForm()
{
    delete ui;
}

void TableInstancesListForm::changeEvent(QEvent *e)
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

void TableInstancesListForm::populateTableInstances(QVector<TableInstance*>const & tables)
{
    for(int i=0; i<tables.size(); i++)
    {
        QStringList a(tables[i]->getName());

        QTreeWidgetItem* item = new QTreeWidgetItem((QTreeWidget*)0, a);
        item->setIcon(0, IconFactory::getTabinstIcon());
        ui->treeTables->addTopLevelItem(item);
    }
}


void TableInstancesListForm::doubleClickTree(QTreeWidgetItem* item, int)
{
    QString tabName = item->text(0);
    mw->showTableInstance(tabName, true);
}
