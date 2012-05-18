#include "ProcedureForm.h"
#include "ui_ProcedureForm.h"
#include "TextEditWithCodeCompletion.h"
#include "FrameForLineNumbers.h"
#include "core_Connection.h"

ProcedureForm::ProcedureForm(ProcedureFormMode m, bool forced, Connection *c, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProcedureForm), m_textEdit(0), m_frameForLineNumbers(0), m_proc(0), m_forcedChange(forced), m_mode(m)
{
    ui->setupUi(this);

    m_frameForLineNumbers = new FrameForLineNumbers(this);
    ui->horizontalLayout->addWidget(m_frameForLineNumbers);

    m_textEdit = new TextEditWithCodeCompletion(this, c);
    m_textEdit->setLineNumberFrame(m_frameForLineNumbers);
    ui->horizontalLayout->addWidget(m_textEdit);
    m_textEdit->setFocus();

    connect(m_textEdit, SIGNAL(textChanged()), this, SLOT(textChanged()));

}

ProcedureForm::~ProcedureForm()
{
    delete ui;
}

void ProcedureForm::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

QString ProcedureForm::getProcNameFromSql()
{
    QString t = m_textEdit->toPlainText();
    QString c = m_mode == MODE_PROCEDURE?"PROCEDURE":"FUNCTION";
    int i = t.indexOf(c, 0, Qt::CaseInsensitive);

    if(i != -1)
    {
        i += c.length();
        while(i<t.length() && t.at(i).isSpace())
        {
            i++;
        }
        QString n = "";
        while(i<t.length() && t.at(i) != '(')
        {
            n += ((t.at(i).isSpace())?(QString("")):(t.at(i)));
            i++;
        }
        m_proc->setDisplayText(n);
        m_proc->setName(n);
        return n;
    }

    return "";
}

void ProcedureForm::textChanged()
{
    m_proc->setSql(m_textEdit->toPlainText());
    if(!m_forcedChange)
    {
        getProcNameFromSql();
    }
}

void ProcedureForm::initSql()
{
    QString sql = QString("CREATE ") + (m_mode == MODE_PROCEDURE?"PROCEDURE ":"FUNCTION ") + m_proc->getName();
    sql += "()";
    if(m_mode == MODE_FUNCTION) sql += " RETURNS ";
    sql += "\nBEGIN\n\nEND";
    m_forcedChange = true;
    m_textEdit->setPlainText(sql);
    m_forcedChange = false;
    m_textEdit->updateLineNumbers();
}

void ProcedureForm::showSql()
{
    m_textEdit->setPlainText(m_proc->getSql());
    m_textEdit->updateLineNumbers();
}
