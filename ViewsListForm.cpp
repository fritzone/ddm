#include "ViewsListForm.h"
#include "ui_ViewsListForm.h"
#include "core_View.h"
#include "IconFactory.h"
#include "MainWindow.h"

ViewsListForm::ViewsListForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ViewsListForm)
{
    ui->setupUi(this);
    ui->btnUp->hide();
    ui->btnDown->hide();
    ui->btnRemove->hide();
}

ViewsListForm::~ViewsListForm()
{
    delete ui;
}

void ViewsListForm::changeEvent(QEvent *e)
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

void ViewsListForm::populateViews(const QVector<View *> views)
{
    ui->treeViews->clear();
    for(int i=0; i<views.size(); i++)
    {
        QStringList a(views.at(i)->getName());

        QTreeWidgetItem* item = new QTreeWidgetItem((QTreeWidget*)0, a);
        item->setIcon(0, IconFactory::getViewIcon());
        ui->treeViews->addTopLevelItem(item);

    }
}

void ViewsListForm::doubleClickTree(QTreeWidgetItem* item, int)
{
    QString name = item->text(0);
}
