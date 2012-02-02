#include "TriggerForm.h"
#include "ui_TriggerForm.h"
#include "TextEditWithCodeCompletion.h"
#include "FrameForLineNumbers.h"
#include "core_Trigger.h"
#include "IconFactory.h"
#include "Table.h"
#include "Workspace.h"
#include "Version.h"

TriggerForm::TriggerForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TriggerForm), m_trigger(0), m_forcedChange(false)
{
    ui->setupUi(this);

    m_frameForLineNumbers = new FrameForLineNumbers(ui->frame_2);
    m_frameForLineNumbers->setObjectName(QString::fromUtf8("m_frameForLineNumbers"));

    ui->horizontalLayout_4->addWidget(m_frameForLineNumbers);

    m_textEdit = new TextEditWithCodeCompletion(ui->frame_2);
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
    if(t->getTable())
    {
        ui->cmbTables->setCurrentIndex(ui->cmbTables->findText(t->getTable()->getName()));
    }
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
    ui->cmbTables->clear();
    for(int i=0; i<tables.size(); i++)
    {
        ui->cmbTables->addItem(IconFactory::getTableIcon(), tables.at(i)->getName());
    }
}

void TriggerForm::feedInTriggerEvents(const QStringList &events)
{
    ui->cmbEvent->clear();
    for(int i=0; i<events.size(); i++)
    {
        QIcon icon = IconFactory::getEmptyIcon();
        if(events.at(i) == "INSERT") icon = IconFactory::getTriggerInsertIcon();
        if(events.at(i) == "DELETE") icon = IconFactory::getTriggerDeleteIcon();
        if(events.at(i) == "UPDATE") icon = IconFactory::getTriggerUpdateIcon();
        ui->cmbEvent->addItem(icon, events.at(i));
    }
}

void TriggerForm::feedInTriggerTimes(const QStringList &times)
{
    ui->cmbTime->clear();
    for(int i=0; i<times.size(); i++)
    {
        QIcon icon = IconFactory::getEmptyIcon();
        if(times.at(i) == "BEFORE") icon = IconFactory::getTriggerBeforeIcon();
        if(times.at(i) == "AFTER") icon = IconFactory::getTriggerAfterIcon();
        ui->cmbTime->addItem(icon, times.at(i));
    }
}

void TriggerForm::showSql()
{
    m_textEdit->setPlainText(m_trigger->getSql());
    m_textEdit->updateLineNumbers();
}

void TriggerForm::nameChanged(QString a)
{
    if(!m_trigger) return;
    m_trigger->setName(a);
    if(m_trigger->getLocation())
    {
        m_trigger->getLocation()->setText(0, a);
    }
}

void TriggerForm::eventChanged(QString a)
{
    if(m_trigger) m_trigger->setEvent(a);
}

void TriggerForm::whenChanged(QString a)
{
    if(m_trigger) m_trigger->setTime(a);
}

void TriggerForm::tableChanged(QString a)
{
    Table* t = Workspace::getInstance()->workingVersion()->getTable(a);
    if(t && m_trigger)
    {
        m_trigger->setTable(t);
    }
}