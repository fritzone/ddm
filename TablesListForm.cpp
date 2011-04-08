#include "TablesListForm.h"
#include "IconFactory.h"
#include "Table.h"
#include "MainWindow.h"
#include "Project.h"
#include "Version.h"

#include "ui_TablesListForm.h"

TablesListForm::TablesListForm(QWidget *parent) : QWidget(parent), ui(new Ui::TablesListForm), mw(reinterpret_cast<MainWindow*>(parent))
{
    ui->setupUi(this);
    ui->btnUp->hide();
    ui->btnDown->hide();
    ui->btnRemove->hide();
}

TablesListForm::~TablesListForm()
{
    delete ui;
    ui = 0;
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
    ui->treeTables->clear();
    for(int i=0; i<tables.size(); i++)
    {
        QStringList a(tables[i]->getName());

        QTreeWidgetItem* item = new QTreeWidgetItem((QTreeWidget*)0, a);
        item->setIcon(0, IconFactory::getTablesIcon());
        ui->treeTables->addTopLevelItem(item);
    }
}


void TablesListForm::doubleClickTree(QTreeWidgetItem* item, int)
{
    QString tabName = item->text(0);
    mw->showTable(tabName, true);
}

void TablesListForm::setOop(bool a)
{
    if(a)   // ugyl like hell ...
    {
        ui->lblHeader->setText("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\np, li { white-space: pre-wrap; }\n</style></head><body style=\" font-family:'Sans'; font-size:10pt; font-weight:400; font-style:normal;\">\n<table border=\"0\" style=\"-qt-table-type: root; margin-top:4px; margin-bottom:4px; margin-left:4px; margin-right:4px;\">\n<tr>\n<td style=\"border: none;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; font-weight:600;\">Table templates</span></p></td></tr></table></body></html>");
    }
    else
    {
        ui->lblHeader->setText("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\np, li { white-space: pre-wrap; }\n</style></head><body style=\" font-family:'Sans'; font-size:10pt; font-weight:400; font-style:normal;\">\n<table border=\"0\" style=\"-qt-table-type: root; margin-top:4px; margin-bottom:4px; margin-left:4px; margin-right:4px;\">\n<tr>\n<td style=\"border: none;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; font-weight:600;\">Tables</span></p></td></tr></table></body></html>");
    }
}
