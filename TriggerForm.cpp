#include "TriggerForm.h"
#include "ui_TriggerForm.h"

TriggerForm::TriggerForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TriggerForm)
{
    ui->setupUi(this);
}

TriggerForm::~TriggerForm()
{
    delete ui;
}

void TriggerForm::changeEvent(QEvent *e)
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
