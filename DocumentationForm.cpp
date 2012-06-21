#include "DocumentationForm.h"
#include "ui_DocumentationForm.h"
#include <QDebug>

DocumentationForm::DocumentationForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DocumentationForm)
{
    ui->setupUi(this);
}

DocumentationForm::~DocumentationForm()
{
    delete ui;
}

void DocumentationForm::setDoc(const QString &html)
{
    qDebug() << html;
    QByteArray ba(html.toLocal8Bit().data());
    ui->webView->setContent(ba);
}
