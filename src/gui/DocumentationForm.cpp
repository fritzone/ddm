#include "DocumentationForm.h"
#include "ui_DocumentationForm.h"
#include "QHtmlCSSStyleSet.h"
#include "DocumentationGenerator.h"
#include "Workspace.h"
#include "Solution.h"
#include "core_Trigger.h"
#include "core_Function.h"
#include "core_Procedure.h"
#include "Project.h"
#include "Version.h"
#include "core_Table.h"
#include "UidWarehouse.h"
#include "QHtmlDocument.h"
#if QT_VERSION >= 0x050000
#include <QtPrintSupport/QPrinter>
#include <QtWebKit>
#include <QtWebKitWidgets/QWebFrame>
#else
#include <QPrinter>
#include <QWebFrame>
#endif
#include <QFileDialog>

#include <QTextDocument>
#include <QTextDocumentWriter>

QString DocumentationForm::s_lastStyle = "Unstyled";
QStringList DocumentationForm::s_openedStyles;

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

void DocumentationForm::initiateStyleChange(QString ns)
{
    styleChanged(ns);
    ui->comboBox->setCurrentIndex(ui->comboBox->findText(ns));

    for(int i=0; i<s_openedStyles.size(); i++)
    {
        if(ui->comboBox->findText(s_openedStyles.at(i)) == -1)
        {
            ui->comboBox->addItem(s_openedStyles.at(i));
        }
    }
}


void DocumentationForm::setDoc(const QString &html)
{
    QByteArray ba(html.toLocal8Bit().data());
    ui->webView->setContent(ba);
}

void DocumentationForm::showDocumentationforUid(const QString &guid, QHtmlCSSStyleSet* css)
{
    DocumentationGenerator gen(Workspace::getInstance()->currentSolution(), css);

    Table* table =  dynamic_cast<Table*>(UidWarehouse::instance().getElement(guid));
    if(table)
    {
        QHtmlDocument doc("Documentation for table " + table->getName(), QHtmlDocument::HTML_5, css);
        gen.getDocumentationForTable(table, doc);
        setUid(guid);
        setDoc(doc.html());
    }
    else
    {
        Function* f = dynamic_cast<Function*>(UidWarehouse::instance().getElement(guid));
        if(f)
        {
            QHtmlDocument doc("Documentation for function " + f->getName(), QHtmlDocument::HTML_5, css);
            gen.getDocumentationForStoredMethod(f, doc);
            setUid(guid);
            setDoc(doc.html());
        }
        else
        {
            Procedure* p = dynamic_cast<Procedure*>(UidWarehouse::instance().getElement(guid));
            if(p)
            {
                QHtmlDocument doc("Documentation for procedure " + p->getName(), QHtmlDocument::HTML_5, css);
                gen.getDocumentationForStoredMethod(p, doc);
                setUid(guid);
                setDoc(doc.html());
            }
            else
            {
                Trigger* trg = dynamic_cast<Trigger*>(UidWarehouse::instance().getElement(guid));
                if(trg)
                {
                    QHtmlDocument doc("Documentation for trigger " + trg->getName(), QHtmlDocument::HTML_5, css);
                    gen.getDocumentationForTrigger(trg, doc);
                    setUid(guid);
                    setDoc(doc.html());
                }
            }
        }
    }
}

void DocumentationForm::styleChanged(QString a)
{
    QHtmlCSSStyleSet* css = 0;
    bool new_stuff = false;
    if(a == "Choose a file ...")
    {
        QString fileName = QFileDialog::getOpenFileName(this,  "Load stylesheet", "", "CSS files (*.css)");
        if(fileName.length() == 0)
        {
            return;
        }
        css =  new QHtmlCSSStyleSet(fileName);
        s_lastStyle = fileName;
        new_stuff = true;
    }
    else
    {
        QFile f(a);
        if(f.exists())
        {
            css = new QHtmlCSSStyleSet(a);
            s_lastStyle = a;
            new_stuff = true;
        }
        else
        {
            if(a != "Unstyled")
            {
                s_lastStyle = ":/doc/" + a.toLower() + ".css";
                css = new QHtmlCSSStyleSet(QString(s_lastStyle));
            }
        }
    }

    if(m_uidShown.length())
    {
        showDocumentationforUid(m_uidShown, css);
    }
    else
    {
        DocumentationGenerator gen (Workspace::getInstance()->currentSolution(), css);
        setDoc(gen.getDocumentation());
    }

    if(new_stuff && !s_openedStyles.contains(s_lastStyle))
    {
        if(!!s_lastStyle.startsWith(":/doc"))
        {
            s_openedStyles.append(s_lastStyle);
        }

        if(ui->comboBox->findText(s_lastStyle) == -1 && !s_lastStyle.startsWith(":/doc"))
        {
            ui->comboBox->addItem(s_lastStyle);
        }
    }
}

void DocumentationForm::onSave()
{
    QString name = QFileDialog::getSaveFileName(this, tr("Save Documentation"), "", tr("Adobe PDF Files(*.pdf);;Open Document Format(*.odt);;Web Page(*.html *.htm)"));

    if(name.length() == 0)  // nothing selected
    {
        return;
    }

    QString mode("");
    if(name.endsWith(".pdf", Qt::CaseInsensitive)) mode = "pdf";
    else if(name.endsWith(".odt", Qt::CaseInsensitive)) mode = "odf";
    else if(name.endsWith(".htm", Qt::CaseInsensitive)) mode = "htm";
    else if(name.endsWith(".html", Qt::CaseInsensitive)) mode = "htm";
    else
    {
        mode = "htm";
        name += ".html";
    }

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
