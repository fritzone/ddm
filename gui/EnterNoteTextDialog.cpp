#include "EnterNoteTextDialog.h"
#include "ui_EnterNoteTextDialog.h"

#include <QPaintEngine>
#include <QPaintDevice>
#include <QPainter>
#include <QColorDialog>


EnterNoteTextDialog::EnterNoteTextDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EnterNoteTextDialog), m_currentColor(Qt::black)
{
    ui->setupUi(this);
    setColorIcon();
    setCursor(Qt::ArrowCursor);
    ui->textEdit->setFocus();
}

EnterNoteTextDialog::~EnterNoteTextDialog()
{
    delete ui;
}

void EnterNoteTextDialog::setColorIcon()
{
    QPixmap pixmap(18, 18);

    QPainter painter(&pixmap);

    painter.fillRect(QRectF(0,0,18,18), Qt::lightGray);
    painter.fillRect( QRectF(2,14,14,4), QBrush(m_currentColor));
    painter.setPen(Qt::black);
    QFont boldItalic("Arial", 14, QFont::Bold);
    //QFont boldItalic(boldItalic1, painter.paintEngine()->paintDevice());
    boldItalic.setBold(true);
    boldItalic.setPixelSize(14);
    boldItalic.setItalic(true);
    painter.setFont(boldItalic);
    painter.drawText( QRectF(4, 0, 12, 12), Qt::AlignLeft, "T");
    painter.end();
    QIcon icon(pixmap);

    ui->btnChangeColor->setIcon(icon);

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

void EnterNoteTextDialog::setText(const QString &s)
{
    ui->textEdit->setHtml(s);
}

void EnterNoteTextDialog::onChangeColor()
{
    m_currentColor = QColorDialog::getColor(m_currentColor);
    setColorIcon();
    ui->textEdit->setTextColor(m_currentColor);
}

void EnterNoteTextDialog::onChangeBold(bool b)
{
    ui->textEdit->setFontWeight(b?QFont::Bold:QFont::Normal);
}

void EnterNoteTextDialog::onChangeItalic(bool b)
{
    ui->textEdit->setFontItalic(b);
}

bool EnterNoteTextDialog::isFramed() const
{
    return ui->chkFramed->isChecked();
}

void EnterNoteTextDialog::setFramed(bool b)
{
    ui->chkFramed->setCheckState(b?Qt::Checked:Qt::Unchecked);
}

void EnterNoteTextDialog::onChangeUnderline(bool b)
{
    ui->textEdit->setFontUnderline(b);
}

void EnterNoteTextDialog::onChangeSize(QString si)
{
    ui->textEdit->setFontPointSize(si.toInt());
}

void EnterNoteTextDialog::onCurrentCharFormatChanged(QTextCharFormat tcf)
{
    ui->btnSetUnderline->setChecked(tcf.fontUnderline());
    ui->btnSetBold->setChecked(tcf.fontWeight() == QFont::Bold);
    ui->btnSetItalic->setChecked(tcf.fontItalic());
    ui->cmbSize->setEditText(QString::number((int)tcf.fontPointSize()));
    ui->fontComboBox->setCurrentFont(tcf.font());
}

void EnterNoteTextDialog::onChangeFont(QFont _fnt)
{
    QFont fnt = _fnt;
    fnt.setPointSize(ui->cmbSize->currentText().toInt());
    fnt.setItalic(ui->btnSetItalic->isChecked());
    fnt.setBold(ui->btnSetBold->isChecked());
    fnt.setUnderline(ui->btnSetUnderline->isChecked());
    ui->textEdit->setCurrentFont(fnt);
}
