#include "TriggerForm.h"
#include "ui_TriggerForm.h"
#include "TextEditWithCodeCompletion.h"
#include "FrameForLineNumbers.h"
#include "core_Trigger.h"
#include "IconFactory.h"
#include "Table.h"

TriggerForm::TriggerForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TriggerForm), m_forcedChange(false)
{
    ui->setupUi(this);

    m_frameForLineNumbers = new FrameForLineNumbers(ui->frame_21);
    m_frameForLineNumbers->setObjectName(QString::fromUtf8("m_frameForLineNumbers"));

    ui->horizontalLayout_4->addWidget(m_frameForLineNumbers);

    m_textEdit = new TextEditWithCodeCompletion(ui->frame_21);
    m_textEdit->setObjectName(QString::fromUtf8("m_textEdit"));

    ui->horizontalLayout_4->addWidget(m_textEdit);
    m_textEdit->setLineNumberFrame(m_frameForLineNumbers);
    m_textEdit->setFocus();
    connect(m_textEdit, SIGNAL(textChanged()), this, SLOT(textChanged()));

}

void TriggerForm::textChanged()
{
    m_trigger->setSql(m_textEdit->toPlainText());
}

TriggerForm::~TriggerForm()
{
    delete ui;
}

void TriggerForm::changeEvent(QEvent *e)
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

void TriggerForm::setTrigger(Trigger *t)
{
    m_trigger = t;
    ui->txtTriggerName->setText(t->getName());
}

void TriggerForm::initSql()
{
    QString sql = "BEGIN\n\nEND";
    m_forcedChange = true;
    m_textEdit->setPlainText(sql);
    m_forcedChange = false;
    m_textEdit->updateLineNumbers();
}

void TriggerForm::feedInTables(const QVector<Table *> &tables)
{
    for(int i=0; i<tables.size(); i++)
    {
        ui->cmbTables->addItem(IconFactory::getTableIcon(), tables.at(i)->getName());
    }
}
