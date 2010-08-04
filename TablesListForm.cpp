#include "TablesListForm.h"
#include "IconFactory.h"
#include "Table.h"

#include "ui_TablesListForm.h"

TablesListForm::TablesListForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TablesListForm)
{
    ui->setupUi(this);
}

TablesListForm::~TablesListForm()
{
    delete ui;
}

void TablesListForm::changeEvent(QEvent *e)
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

void TablesListForm::populateTables(QVector<Table*>const & tables)
{
    for(int i=0; i<tables.size(); i++)
    {
        QStringList a(tables[i]->getName());

        QTreeWidgetItem* item = new QTreeWidgetItem((QTreeWidget*)0, a);
        item->setIcon(0, IconFactory::getTablesIcon());
        ui->treeTables->addTopLevelItem(item);
    }
}
