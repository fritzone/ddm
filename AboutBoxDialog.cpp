#include "AboutBoxDialog.h"
#include "ui_AboutBoxDialog.h"

AboutBoxDialog::AboutBoxDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutBoxDialog)
{
    ui->setupUi(this);
}

AboutBoxDialog::~AboutBoxDialog()
{
    delete ui;
}

void AboutBoxDialog::changeEvent(QEvent *e)
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
