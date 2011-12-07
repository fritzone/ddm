#include "BrowseTableForm.h"
#include "ui_BrowseTableForm.h"

BrowseTableForm::BrowseTableForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BrowseTableForm)
{
    ui->setupUi(this);
}

BrowseTableForm::~BrowseTableForm()
{
    delete ui;
}

void BrowseTableForm::changeEvent(QEvent *e)
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

QTableView* BrowseTableForm::getTable()
{
    return ui->table;
}
