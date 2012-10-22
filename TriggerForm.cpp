#include "TriggerForm.h"
#include "ui_TriggerForm.h"
#include "TextEditWithCodeCompletion.h"
#include "FrameForLineNumbers.h"
#include "core_Trigger.h"
#include "IconFactory.h"
#include "core_Table.h"
#include "Workspace.h"
#include "Version.h"
#include "MainWindow.h"
#include "GuiElements.h"

TriggerForm::TriggerForm(Version *v, bool reverseSource, bool fc, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TriggerForm), m_trigger(0), m_forcedChange(fc), m_reverseSource(reverseSource), m_version(v)
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

    if(m_reverseSource)
    {
        ui->tabWidget->removeTab(1);
    }
}

void TriggerForm::textChanged()
{
    if(m_reverseSource) return;
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
    ui->cmbTables->setCurrentIndex(ui->cmbTables->findText(t->getTable()));
    ui->cmbEvent->setCurrentIndex(ui->cmbEvent->findText(t->getEvent()));
    ui->cmbTime->setCurrentIndex(ui->cmbTime->findText(t->getTime()));
    ui->txtDescription->setText(t->getDescription());


    ui->btnUndelete->hide();

    // TODO: duplicate with the other forms ... at least, the logic!
    if(!m_trigger->wasLocked())
    {
        if(m_trigger->isDeleted())
        {
            ui->frameForUnlockButton->show();
            ui->btnLock->hide();
            ui->btnUndelete->show();
        }
        else
        {
            ui->frameForUnlockButton->hide();
        }
    }
    else
    {
        ui->frameForUnlockButton->show();
        if(m_trigger->lockState() == LockableElement::LOCKED)
        {
            ui->btnLock->setIcon(IconFactory::getLockedIcon());
            ui->btnLock->blockSignals(true);
            ui->btnLock->setChecked(false);
            ui->btnLock->blockSignals(false);
            disableEditingControls(true);
            ui->btnLock->setToolTip(QObject::tr("This trigger is <b>locked</b>. Click this button to unlock it."));
        }
        else
        if(m_trigger->lockState() == LockableElement::FINAL_LOCK)
        {
            ui->btnLock->setIcon(IconFactory::getLockedIcon());
            ui->btnLock->blockSignals(true);
            ui->btnLock->setChecked(false);
            ui->btnLock->blockSignals(false);
            disableEditingControls(true);
            ui->btnLock->setToolTip(QObject::tr("This trigger is <b>locked</b>. Click this button to unlock it."));
        }
        else
        {
            ui->btnLock->setIcon(IconFactory::getUnLockedIcon());
            ui->btnLock->blockSignals(true);
            ui->btnLock->setChecked(true);
            ui->btnLock->blockSignals(false);
            disableEditingControls(false);
            ui->btnLock->setToolTip(QObject::tr("This trigger is <b>unlocked</b>. Click this button to lock it."));
        }

        if(m_trigger->isDeleted())
        {
            ui->btnLock->hide();
            ui->btnUndelete->show();
        }
    }
}

void TriggerForm::disableEditingControls(bool dis)
{
    ui->txtDescription->setDisabled(dis);
    ui->txtTriggerName->setDisabled(dis);
    ui->cmbEvent->setDisabled(dis);
    ui->cmbTables->setDisabled(dis);
    ui->cmbTime->setDisabled(dis);
    m_textEdit->setDisabled(dis);
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

void TriggerForm::feedInTables(const QStringList &tables)
{
    ui->cmbTables->clear();
    for(int i=0; i<tables.size(); i++)
    {
        ui->cmbTables->addItem(IconFactory::getTableIcon(), tables.at(i));
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
    if(m_reverseSource) return;
    if(!m_trigger) return;
    m_trigger->setName(a);
    if(m_trigger->getLocation())
    {
        m_trigger->setDisplayText(a);
    }
}

void TriggerForm::eventChanged(QString a)
{
    if(m_reverseSource) return;
    if(m_trigger) m_trigger->setEvent(a);
}

void TriggerForm::whenChanged(QString a)
{
    if(m_reverseSource) return;
    if(m_trigger) m_trigger->setTime(a);
}

void TriggerForm::tableChanged(QString a)
{
    if(m_reverseSource) return;
    Table* t = Workspace::getInstance()->workingVersion()->getTable(a);
    if(t && m_trigger)
    {
        m_trigger->setTable(t->getName());
    }
}

void TriggerForm::descriptionChanged()
{
    m_trigger->setDescription(ui->txtDescription->toPlainText());
}

void TriggerForm::onLockUnlock(bool checked)
{
    if(checked)
    {
        ui->btnLock->setIcon(IconFactory::getUnLockedIcon());
        disableEditingControls(false);
        m_trigger->unlock();
        m_trigger->updateGui();
        ui->btnLock->setToolTip(QObject::tr("Trigger is <b>unlocked</b>. Click this button to lock it."));

        MainWindow::instance()->finallyDoLockLikeOperation(false, m_trigger->getObjectUid());
    }
    else
    {
        ui->btnLock->setIcon(IconFactory::getLockedIcon());
        disableEditingControls(true);
        m_trigger->lock(LockableElement::LOCKED);
        m_trigger->updateGui();
        ui->btnLock->setToolTip(QObject::tr("Trigger is <b>locked</b>. Click this button to unlock it."));

        MainWindow::instance()->finallyDoLockLikeOperation(true, m_trigger->getObjectUid());
    }

}

void TriggerForm::onUndelete()
{
    if(m_version->undeleteObject(m_trigger->getObjectUid(), false))
    {
        MainWindow::instance()->getGuiElements()->removeItemForPatch(m_version->getWorkingPatch(), m_trigger->getObjectUid());
        // TODO: Duplicate from above
        if(m_trigger->lockState() == LockableElement::LOCKED)
        {
            ui->btnLock->setIcon(IconFactory::getLockedIcon());
            ui->btnLock->blockSignals(true);
            ui->btnLock->setChecked(false);
            ui->btnLock->blockSignals(false);
            disableEditingControls(true);
            ui->btnLock->setToolTip(QObject::tr("This trigger is <b>locked</b>. Click this button to unlock it."));
        }
        else
        {
            ui->btnLock->setIcon(IconFactory::getUnLockedIcon());
            ui->btnLock->blockSignals(true);
            ui->btnLock->setChecked(true);
            ui->btnLock->blockSignals(false);
            disableEditingControls(false);
            ui->btnLock->setToolTip(QObject::tr("This trigger is <b>unlocked</b>. Click this button to lock it."));
        }
        ui->btnUndelete->hide();
        ui->btnLock->show();
    }
}
