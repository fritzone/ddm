#include "QueryItemListDialog.h"
#include "ui_QueryItemListDialog.h"

#include "Workspace.h"
#include "Version.h"
#include "Table.h"
#include "IconFactory.h"
#include "TableInstance.h"

#include <QListWidget>

QueryItemListDialog::QueryItemListDialog(QueryComponents::ListType t, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QueryItemListDialog), m_selected()
{
    ui->setupUi(this);

    switch(t)
    {
    case QueryComponents::LIST_TABLES:
        if(Workspace::getInstance()->currentProjectIsOop())
        {
            for(int i=0; i<Workspace::getInstance()->workingVersion()->getTableInstances().size(); i++)
            {
                TableInstance* tabI = Workspace::getInstance()->workingVersion()->getTableInstances().at(i);
                QListWidgetItem* lwi = new QListWidgetItem(tabI->getName());
                lwi->setFont(QFont("Arial", 14, 2));
                lwi->setIcon(IconFactory::getTabinstIcon());
                ui->lstValues->addItem(lwi);
            }
        }
        else
        {
            for(int i=0; i<Workspace::getInstance()->workingVersion()->getTables().size(); i++)
            {
                Table* tabI = Workspace::getInstance()->workingVersion()->getTables().at(i);
                QListWidgetItem* lwi = new QListWidgetItem(tabI->getName());
                lwi->setFont(QFont("Arial", 14, 2));
                lwi->setIcon(IconFactory::getTablesIcon());
                ui->lstValues->addItem(lwi);

            }

        }
        break;
    }
}

QueryItemListDialog::~QueryItemListDialog()
{
    delete ui;
}


void QueryItemListDialog::onDblClickItem(QListWidgetItem* itm)
{
    m_selected = itm->text();
    close();
}
