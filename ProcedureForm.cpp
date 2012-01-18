#include "ProcedureForm.h"
#include "ui_ProcedureForm.h"
#include "QTextEditWithCodeCompletion.h"
#include "FrameForLineNumbers.h"
#include "core_Procedure.h"

ProcedureForm::ProcedureForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProcedureForm), m_textEdit(0), m_frameForLineNumbers(0), m_proc(0), m_forcedChange(false)
{
    ui->setupUi(this);

    m_frameForLineNumbers = new FrameForLineNumbers(this);
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(m_frameForLineNumbers->sizePolicy().hasHeightForWidth());
    m_frameForLineNumbers->setSizePolicy(sizePolicy);
    m_frameForLineNumbers->setMinimumSize(QSize(48, 0));
    m_frameForLineNumbers->setMaximumSize(QSize(48, 16777215));
    m_frameForLineNumbers->setFrameShape(QFrame::StyledPanel);
    m_frameForLineNumbers->setFrameShadow(QFrame::Raised);
    ui->horizontalLayout->addWidget(m_frameForLineNumbers);

    m_textEdit = new QTextEditWithCodeCompletion(this);
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
    QString c = "PROCEDURE";
    int i = t.indexOf(c, 0, Qt::CaseInsensitive);
    if(i == -1)
    {
        c = "FUNCTION";
        i = t.indexOf(c, 0, Qt::CaseInsensitive);
    }

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
        m_proc->getLocation()->setText(0, n);
        m_proc->setName(n);
        QVariant v;
        v.setValue(n);
        m_proc->getLocation()->setData(0, Qt::UserRole, v);
        return n;
    }

    return "";
}

void ProcedureForm::textChanged()
{
    m_proc->setSql(m_textEdit->toPlainText());
    if(!m_forcedChange) getProcNameFromSql();
}

void ProcedureForm::initSql()
{
    QString sql = "CREATE PROCEDURE " + m_proc->getName();
    sql += "()\nBEGIN\n\nEND";
    m_forcedChange = true;
    m_textEdit->setPlainText(sql);
    m_forcedChange = false;
    m_textEdit->updateLineNumbers();
}

void ProcedureForm::showSql()
{
    m_textEdit->setPlainText(m_proc->getSql());
}
