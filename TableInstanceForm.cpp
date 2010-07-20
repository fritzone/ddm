#include "TableInstanceForm.h"
#include "ui_TableInstanceForm.h"

TableInstanceForm::TableInstanceForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TableInstanceForm)
{
    ui->setupUi(this);
}

TableInstanceForm::~TableInstanceForm()
{
    delete ui;
}

void TableInstanceForm::changeEvent(QEvent *e)
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
