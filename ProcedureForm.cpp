#include "ProcedureForm.h"
#include "ui_ProcedureForm.h"
#include "TextEditWithCodeCompletion.h"
#include "FrameForLineNumbers.h"
#include "core_Connection.h"
#include "MainWindow.h"
#include "GuiElements.h"

ProcedureForm::ProcedureForm(Version* v, ProcedureFormMode m, bool forced, Connection *c, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProcedureForm), m_textEdit(0), m_frameForLineNumbers(0), m_proc(0), m_forcedChange(forced), m_mode(m), m_version(v)
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
    int t = 0;
    QString n = m_proc->getNameFromSql(0, t);
    if(n != "UNNAMED")
    {
        if(!m_proc->isDeleted())
        {
            m_proc->setDisplayText(n);
            m_proc->setName(n);
        }
    }
    return m_proc->getName();
}

void ProcedureForm::textChanged()
{
    if(m_proc->isDeleted()) return;

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

void ProcedureForm::onLockUnlock(bool checked)
{
    if(checked)
    {
        ui->btnLock->setIcon(IconFactory::getUnLockedIcon());
        ui->grpContent->setEnabled(true);
        m_proc->unlock();
        m_proc->updateGui();
        ui->btnLock->setToolTip((m_mode == MODE_PROCEDURE?"Procedure ":"Function ") + QObject::tr("is <b>unlocked</b>. Click this button to lock it."));

        MainWindow::instance()->finallyDoLockLikeOperation(false, m_proc->getObjectUid());
    }
    else
    {
        ui->btnLock->setIcon(IconFactory::getLockedIcon());
        ui->grpContent->setEnabled(false);
        m_proc->lock(LockableElement::LOCKED);
        m_proc->updateGui();
        ui->btnLock->setToolTip((m_mode == MODE_PROCEDURE?"Procedure ":"Function ") + QObject::tr("is <b>locked</b>. Click this button to lock it."));

        MainWindow::instance()->finallyDoLockLikeOperation(true, m_proc->getObjectUid());
    }

}

void ProcedureForm::setProcedure(StoredMethod *p)
{
    m_proc = p;
    ui->btnUndelete->hide();

    // TODO: duplicate with the other forms ... at least, the logic!
    if(!m_proc->wasLocked())
    {
        if(m_proc->isDeleted())
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
        if(m_proc->lockState() == LockableElement::LOCKED)
        {
            ui->btnLock->setIcon(IconFactory::getLockedIcon());
            ui->btnLock->blockSignals(true);
            ui->btnLock->setChecked(false);
            ui->btnLock->blockSignals(false);
            ui->grpContent->setEnabled(false);
            ui->btnLock->setToolTip((m_mode == MODE_PROCEDURE?"This Procedure ":"This Function ") + QObject::tr("is <b>locked</b>. Click this button to unlock it."));
        }
        else
        {
            ui->btnLock->setIcon(IconFactory::getUnLockedIcon());
            ui->btnLock->blockSignals(true);
            ui->btnLock->setChecked(true);
            ui->btnLock->blockSignals(false);
            ui->grpContent->setEnabled(true);
            ui->btnLock->setToolTip((m_mode == MODE_PROCEDURE?"This Procedure ":"This Function ") + QObject::tr("is <b>unlocked</b>. Click this button to unlock it."));
        }

        if(m_proc->isDeleted())
        {
            ui->btnLock->hide();
            ui->btnUndelete->show();
        }
    }
}


void ProcedureForm::onUndelete()
{
    if(m_version->undeleteObject(m_proc->getObjectUid(), false))
    {
        MainWindow::instance()->getGuiElements()->removeItemForPatch(m_version->getWorkingPatch(), m_proc->getObjectUid());
        // TODO: Duplicate from above
        if(m_proc->lockState() == LockableElement::LOCKED)
        {
            ui->btnLock->setIcon(IconFactory::getLockedIcon());
            ui->btnLock->blockSignals(true);
            ui->btnLock->setChecked(false);
            ui->btnLock->blockSignals(false);
            ui->grpContent->setEnabled(false);
            ui->btnLock->setToolTip((m_mode == MODE_PROCEDURE?"This Procedure ":"This Function ") + QObject::tr("is <b>locked</b>. Click this button to unlock it."));
        }
        else
        {
            ui->btnLock->setIcon(IconFactory::getUnLockedIcon());
            ui->btnLock->blockSignals(true);
            ui->btnLock->setChecked(true);
            ui->btnLock->blockSignals(false);
            ui->grpContent->setEnabled(true);
            ui->btnLock->setToolTip((m_mode == MODE_PROCEDURE?"This Procedure ":"This Function ") + QObject::tr("is <b>unlocked</b>. Click this button to unlock it."));
        }
        ui->btnUndelete->hide();
        ui->btnLock->show();
    }
}
