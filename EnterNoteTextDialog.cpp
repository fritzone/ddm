#include "EnterNoteTextDialog.h"
#include "ui_EnterNoteTextDialog.h"

EnterNoteTextDialog::EnterNoteTextDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EnterNoteTextDialog)
{
    ui->setupUi(this);
}

EnterNoteTextDialog::~EnterNoteTextDialog()
{
    delete ui;
}

void EnterNoteTextDialog::changeEvent(QEvent *e)
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
