#include "DocumentationForm.h"
#include "ui_DocumentationForm.h"
#include <QDebug>
#include "QHtmlCSSStyleSet.h"
#include "DocumentationGenerator.h"
#include "Workspace.h"
#include "Solution.h"
#include "Project.h"
#include "Version.h"

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

void DocumentationForm::styleChanged(QString a)
{
    QHtmlCSSStyleSet* css = new QHtmlCSSStyleSet(QString(":/doc/" + a.toLower() + ".css"));
    DocumentationGenerator gen (Workspace::getInstance()->currentSolution(), css);
    setDoc(gen.getDocumentation());
}

void DocumentationForm::onSave()
{

}

void DocumentationForm::onPrint()
{

}
