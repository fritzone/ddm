#include "ProcedureForm.h"
#include "ui_ProcedureForm.h"

ProcedureForm::ProcedureForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProcedureForm)
{
    ui->setupUi(this);
}

ProcedureForm::~ProcedureForm()
{
    delete ui;
}

void ProcedureForm::changeEvent(QEvent *e)
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
