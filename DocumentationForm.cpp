#include "DocumentationForm.h"
#include "ui_DocumentationForm.h"
#include "QHtmlCSSStyleSet.h"
#include "DocumentationGenerator.h"
#include "Workspace.h"
#include "Solution.h"
#include "Project.h"
#include "Version.h"
#include "core_Table.h"
#include "UidWarehouse.h"
#include "QHtmlDocument.h"

#include <QPrinter>
#include <QDebug>
#include <QFileDialog>
#include <QWebFrame>
#include <QWebPage>
#include <QTextDocument>
#include <QTextDocumentWriter>

DocumentationForm::DocumentationForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DocumentationForm), m_uidShown("")
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
    QHtmlCSSStyleSet* css = 0;
    if(a == "Choose a file ...")
    {
        QString fileName = QFileDialog::getOpenFileName(this,  "Load stylesheet", "", "CSS files (*.css)");
        if(fileName.length() == 0)
        {
            return;
        }
        css =  new QHtmlCSSStyleSet(fileName);
    }
    else
    {
        css = new QHtmlCSSStyleSet(QString(":/doc/" + a.toLower() + ".css"));

    }
    if(m_uidShown.length())
    {
        Table* table =  dynamic_cast<Table*>(UidWarehouse::instance().getElement(m_uidShown));
        if(table != 0)  // shouldn't be ...
        {
            DocumentationGenerator gen(Workspace::getInstance()->currentSolution(), css);
            QHtmlDocument doc("Documentation for table " + table->getName(), QHtmlDocument::HTML_5, css);
            gen.getDocumentationForTable(table, doc);
            setDoc(doc.html());
        }
    }
    else
    {
        DocumentationGenerator gen (Workspace::getInstance()->currentSolution(), css);
        setDoc(gen.getDocumentation());
    }
}

void DocumentationForm::onSave()
{
    QString name = QFileDialog::getSaveFileName(this, tr("Save Documentation"), "", tr("Adobe PDF Files(*.pdf);;Open Document Format(*.odf);;Web Page(*.html *.htm)"));

    if(name.length() == 0)  // nothing selected
    {
        return;
    }

    QString mode("");
    if(name.endsWith(".pdf", Qt::CaseInsensitive)) mode = "pdf";
    if(name.endsWith(".odf", Qt::CaseInsensitive)) mode = "odf";
    if(name.endsWith(".htm", Qt::CaseInsensitive)) mode = "htm";
    if(name.endsWith(".html", Qt::CaseInsensitive)) mode = "htm";

    if(mode.length() == 0)
    {
        return;
    }

    if(mode == "pdf")
    {
        QPrinter printer;
        printer.setPrinterName("Print to File (PDF)");
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setPrintRange(QPrinter::AllPages);
        printer.setOrientation(QPrinter::Portrait);
        printer.setPaperSize(QPrinter::A4);
        printer.setResolution(QPrinter::HighResolution);
        printer.setFullPage(false);
        printer.setNumCopies(1);
        printer.setOutputFileName(name);
        ui->webView->print(&printer);
    }
    else
    if(mode == "htm")
    {
        QString t = ui->webView->page()->mainFrame()->toHtml();
        QFile file(name);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            return;
        }

        QTextStream out(&file);
        out << t;
    }
    else
    if(mode == "odf")
    {
        QTextDocument td;
        td.setHtml(ui->webView->page()->mainFrame()->toHtml());
        QTextDocumentWriter wr;
        wr.setFormat("odf");
        wr.setFileName(name);
        wr.write(&td);
    }

}

void DocumentationForm::onPrint()
{

}
