#include "SimpleTextInputDialog.h"
#include "ui_SimpleTextInputDialog.h"

SimpleTextInputDialog::SimpleTextInputDialog(QWidget *parent, const QString& caption, const QString& text) : QDialog(parent),
    ui(new Ui::SimpleTextInputDialog)
{
    ui->setupUi(this);
    setWindowTitle(caption);
    setText(text);
    setModal(true);
}



SimpleTextInputDialog::~SimpleTextInputDialog()
{
    delete ui;
}

void SimpleTextInputDialog::changeEvent(QEvent *e)
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

void SimpleTextInputDialog::setText(const QString& txt)
{
    ui->lineEdit->setText(txt);
}

QString SimpleTextInputDialog::getText()
{
    return ui->lineEdit->text();
}
