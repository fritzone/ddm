#include "EnterNoteTextDialog.h"
#include "ui_EnterNoteTextDialog.h"

#include <QPainter>

EnterNoteTextDialog::EnterNoteTextDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EnterNoteTextDialog)
{
    ui->setupUi(this);

    //QPalette pal = ui->btnChangeColor->palette( );

    QImage newImage(18, 18, QImage::Format_ARGB32);

    QPainter painter(&newImage);

    painter.fillRect(QRectF(0,0,18,18), Qt::white);
    painter.fillRect( QRectF(2,14,14,4), QBrush(Qt::black));
    painter.drawText(0,0,"FG");
    painter.end();
    QIcon icon(QPixmap::fromImage(newImage));

    ui->btnChangeColor->setIcon(icon);

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

QString EnterNoteTextDialog::getText()
{
    return ui->textEdit->toHtml();
}
