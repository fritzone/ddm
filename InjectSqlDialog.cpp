#include "InjectSqlDialog.h"
#include "ui_InjectSqlDialog.h"

InjectSqlDialog::InjectSqlDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InjectSqlDialog)
{
    ui->setupUi(this);
}

InjectSqlDialog::~InjectSqlDialog()
{
    delete ui;
}

void InjectSqlDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
