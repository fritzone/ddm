#include "ProceduresListForm.h"
#include "ui_ProceduresListForm.h"
#include "core_Procedure.h"
#include "IconFactory.h"
#include "MainWindow.h"

ProceduresListForm::ProceduresListForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProceduresListForm)
{
    ui->setupUi(this);
    ui->btnUp->hide();
    ui->btnDown->hide();
    ui->btnRemove->hide();
}

ProceduresListForm::~ProceduresListForm()
{
    delete ui;
}

void ProceduresListForm::changeEvent(QEvent *e)
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

void ProceduresListForm::populateProcedures(const QVector<Procedure *> views)
{
    ui->treeProcedures->clear();
    for(int i=0; i<views.size(); i++)
    {
        QStringList a(views.at(i)->getName());

        QTreeWidgetItem* item = new QTreeWidgetItem((QTreeWidget*)0, a);
        item->setIcon(0, IconFactory::getProcedureIcon());
        ui->treeProcedures->addTopLevelItem(item);

    }
}

void ProceduresListForm::doubleClickTree(QTreeWidgetItem* item, int)
{
    QString name = item->text(0);
    MainWindow::instance()->showProcedure(name);
}
