#include "TriggerForm.h"
#include "ui_TriggerForm.h"
#include "TextEditWithCodeCompletion.h"
#include "FrameForLineNumbers.h"
#include "core_Trigger.h"
#include "IconFactory.h"
#include "Workspace.h"
#include "Version.h"
#include "MainWindow.h"
#include "GuiElements.h"
#include "core_Table.h"
#include "core_TableInstance.h"
#include "db_DatabaseEngine.h"
#include "Configuration.h"
#include "Connection.h"

#include <QToolTip>
#include <QMessageBox>

TriggerForm::TriggerForm(Version *v, Connection* c, bool reverseSource, bool fc, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TriggerForm), m_trigger(0), m_forcedChange(fc),
    m_reverseSource(reverseSource), m_version(v),
    m_conn(c)
{
    ui->setupUi(this);

    m_frameForLineNumbers = new FrameForLineNumbers(ui->frame_2);
    m_frameForLineNumbers->setObjectName(QString::fromUtf8("m_frameForLineNumbers"));

    ui->horizontalLayout_4->addWidget(m_frameForLineNumbers);

    m_textEdit = new TextEditWithCodeCompletion(ui->frame_2, c);
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
    QStringList lines = m_textEdit->toPlainText().split("\n");
    lines.removeFirst();
    m_trigger->setSql(lines.join("\n"));
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

    int tabIdx = ui->cmbTables->findText(t->getTable());
    if(tabIdx == -1)
    {
        tabIdx = 0; // (no table, select the first one)
    }
    ui->cmbTables->setCurrentIndex(tabIdx);
    ui->cmbEvent->setCurrentIndex(ui->cmbEvent->findText(t->getEvent().toUpper()));
    ui->cmbTime->setCurrentIndex(ui->cmbTime->findText(t->getTime().toUpper()));
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
    QString sql = "";
    if(m_trigger)
    {
        m_trigger->setTable(ui->cmbTables->currentText());
        m_trigger->setTime(ui->cmbTime->currentText());
        m_trigger->setEvent(ui->cmbEvent->currentText());

        sql = m_trigger->generateSqlSource(m_version->getProject()->getEngine()->getSqlGenerator(),
                                           Configuration::instance().sqlGenerationOptions(), 0).join("\n");
        sql = sql.trimmed();
    }
    sql += "\n";
    m_forcedChange = true;
    m_textEdit->setPlainText(sql);
    m_textEdit->disableRow(1);
    m_forcedChange = false;
    m_textEdit->updateLineNumbers();

    m_textEdit->keyPressEvent(new QKeyEvent(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier));
}

void TriggerForm::feedInTables(const QVector<QString> &tables, bool oop)
{
    m_forcedChange = true;

    ui->cmbTables->clear();
    for(int i=0; i<tables.size(); i++)
    {
        ui->cmbTables->addItem(oop?IconFactory::getTabinstIcon():IconFactory::getTableIcon(), tables.at(i));
    }

    m_forcedChange = false;

}

void TriggerForm::feedInTables(const QStringList &tables)
{
    m_forcedChange = true;

    ui->cmbTables->clear();
    for(int i=0; i<tables.size(); i++)
    {
        ui->cmbTables->addItem(IconFactory::getTableIcon(), tables.at(i));
    }

    m_forcedChange = false;

}

void TriggerForm::feedInTriggerEvents(const QStringList &events, QString def)
{
    m_forcedChange = true;
    ui->cmbEvent->clear();
    for(int i=0; i<events.size(); i++)
    {
        QIcon icon = IconFactory::getEmptyIcon();
        if(events.at(i) == "INSERT") icon = IconFactory::getTriggerInsertIcon();
        if(events.at(i) == "DELETE") icon = IconFactory::getTriggerDeleteIcon();
        if(events.at(i) == "UPDATE") icon = IconFactory::getTriggerUpdateIcon();
        if(def.isEmpty())
        {
            ui->cmbEvent->addItem(icon, events.at(i));
        }
        else
        {
            if(events.at(i) == def )
            {
                ui->cmbEvent->addItem(icon, events.at(i));
            }
        }

    }
    m_forcedChange = false;
}

void TriggerForm::feedInTriggerTimes(const QStringList &times, QString def)
{
    m_forcedChange = true;

    ui->cmbTime->clear();
    for(int i=0; i<times.size(); i++)
    {
        QIcon icon = IconFactory::getEmptyIcon();
        if(times.at(i) == "BEFORE") icon = IconFactory::getTriggerBeforeIcon();
        if(times.at(i) == "AFTER") icon = IconFactory::getTriggerAfterIcon();
        if(def.isEmpty())
        {
            ui->cmbTime->addItem(icon, times.at(i));
        }
        else
        {
            if(times.at(i) == def )
            {
                ui->cmbTime->addItem(icon, times.at(i));
            }
        }
    }
    m_forcedChange = false;
}

void TriggerForm::showSql()
{
    AbstractSqlGenerator* asg = m_version?m_version->getProject()->getEngine()->getSqlGenerator():m_conn->getEngine()->getSqlGenerator();
    QString sql = m_trigger->generateSqlSource(asg,
                                       Configuration::instance().sqlGenerationOptions(), 0).join("\n");
    sql = sql.trimmed();
    if(sql.indexOf("\n") == -1)
    {
        sql += "\n";
    }

    m_textEdit->setPlainText(sql);
    m_textEdit->disableRow(1);
    m_textEdit->updateLineNumbers();

    m_textEdit->keyPressEvent(new QKeyEvent(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier));
}

void TriggerForm::nameChanged(QString a)
{
    if(m_reverseSource) return;
    if(!m_trigger) return;
    if(m_forcedChange) return;

    a = a.trimmed();
    if(a.isEmpty())
    {
        return;
    }

    QPalette pal;
    pal.setColor(QPalette::Text, Qt::black);
    ui->txtTriggerName->setPalette(pal);

    // and see if there is a table, column or KEYWORD called "a"
    Trigger* other = m_version->getTrigger(a);
    if(m_version->getProject()->getEngine()->getKeywords().contains(a, Qt::CaseInsensitive) || (other && other != m_trigger) )
    {
        QPalette pal;
        pal.setColor(QPalette::Text, Qt::red);
        ui->txtTriggerName->setPalette(pal);

        QString netTooltip = tr("This is not an allowed name for the trigger.");
        if(m_version->getProject()->getEngine()->getKeywords().contains(a, Qt::CaseInsensitive))
        {
            netTooltip += "<p><b>" + a + "</b>" + tr(" is a reserved keyword for this database. DDM does not allow this to avoid future confusion.");
        }
        else
        {
            netTooltip += "<p>" + tr("There is already a trigger called <b> ") + a;
        }

        QToolTip::showText(ui->txtTriggerName->mapToGlobal(QPoint()), netTooltip, ui->txtTriggerName);
        ui->txtTriggerName->setToolTip(netTooltip);
        return;
    }
    else
    {
        ui->txtTriggerName->setToolTip(tr("The name of the trigger"));
    }


    m_trigger->setName(a);
    if(m_trigger->getLocation())
    {
        m_trigger->setDisplayText(a);
    }
    showSql();
}

void TriggerForm::eventChanged(QString a)
{
    if(m_reverseSource) return;
    if(m_forcedChange) return;
    if(m_trigger) m_trigger->setEvent(a);
    showSql();
}

void TriggerForm::whenChanged(QString a)
{
    if(m_reverseSource) return;
    if(m_forcedChange) return;

    if(m_trigger) m_trigger->setTime(a);
    showSql();
}

void TriggerForm::tableChanged(QString a)
{
    if(m_reverseSource) return;
    if(m_forcedChange) return;
    if(!m_trigger) return;

    Table* t = m_trigger->version()->getTable(a);
    if(t)
    {
        m_trigger->setTable(t->getName());
    }
    else
    {
        TableInstance* tinst = m_trigger->version()->getTableInstance(a);
        if(tinst)
        {
            m_trigger->setTable(tinst->getName());
        }
    }
    showSql();
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

        MainWindow::instance()->finallyDoLockLikeOperation(false, m_trigger->getObjectUid().toString());
    }
    else
    {
        ui->btnLock->setIcon(IconFactory::getLockedIcon());
        disableEditingControls(true);
        m_trigger->lock(LockableElement::LOCKED);
        m_trigger->updateGui();
        ui->btnLock->setToolTip(QObject::tr("Trigger is <b>locked</b>. Click this button to unlock it."));

        MainWindow::instance()->finallyDoLockLikeOperation(true, m_trigger->getObjectUid().toString());
    }
}

void TriggerForm::onUndelete()
{
    QString tempError;
    if(Version::DO_NOT_REMOVE_FROM_PATCH_TREE_FAILURE != m_version->undeleteObject(m_trigger->getObjectUid().toString(), false, tempError))
    {
        MainWindow::instance()->getGuiElements()->removeItemForPatch(m_version->getWorkingPatch(), m_trigger->getObjectUid().toString());
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
    else
    {
        QMessageBox::critical(this, tr("Error"), tempError);
    }
}
