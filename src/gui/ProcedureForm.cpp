#include "ProcedureForm.h"
#include "ui_ProcedureForm.h"
#include "TextEditWithCodeCompletion.h"
#include "FrameForLineNumbers.h"
#include "Connection.h"
#include "MainWindow.h"
#include "GuiElements.h"
#include "strings.h"
#include "core_ParameterAndDescription.h"

ProcedureForm::ProcedureForm(Version* v, ProcedureFormMode m, bool guided, bool forced, Connection *c, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProcedureForm), m_textEdit(0), m_frameForLineNumbers(0), m_proc(0), m_forcedChange(forced), m_mode(m), m_version(v)
{
    m_init = true;
    ui->setupUi(this);

    m_frameForLineNumbers = new FrameForLineNumbers(this);
    ui->horizontalLayout->addWidget(m_frameForLineNumbers);

    m_textEdit = new TextEditWithCodeCompletion(this, c);
    m_textEdit->setLineNumberFrame(m_frameForLineNumbers);
    ui->horizontalLayout->addWidget(m_textEdit);
    m_textEdit->setFocus();

    ui->btnInject->hide();
    connect(m_textEdit, SIGNAL(textChanged()), this, SLOT(textChanged()));

    if(m == MODE_PROCEDURE)
    {
        ui->cmbReturnType->hide();
    }
    else
    {
        m_availableDataTypes = v->getDataTypes();
        for(int i=0; i<m_availableDataTypes.size(); i++)
        {
            ui->cmbReturnType->addItem(IconFactory::getIconForDataType(m_availableDataTypes[i]->getType()),
                                            m_availableDataTypes[i]->getName());
            ui->cmbParameterType->addItem(IconFactory::getIconForDataType(m_availableDataTypes[i]->getType()),
                                          m_availableDataTypes[i]->getName());
        }
    }

    m_guided = guided;
    toggleGuidedCreationControls(m_guided);
    ui->txtParamDesc->hide();
    ui->tabWidget->setCurrentIndex(0);

    if(m_mode == MODE_PROCEDURE)
    {
        ui->tabWidget->setTabIcon(0, IconFactory::getProcedureIcon());
    }
    else
    {
        ui->tabWidget->setTabIcon(0, IconFactory::getFunctionTreeIcon());
    }

    if(m_guided)
    {
        QTreeWidgetItem *header = ui->lstParameters->headerItem();
        header->setText(0, tr("Name"));
        header->setText(1, tr("Dir"));
        header->setText(2, tr("Type"));
        header->setText(3, tr("Desc"));

        ui->btnLoad->hide();
    }

}

ProcedureForm::~ProcedureForm()
{
    delete ui;
}

void ProcedureForm::disableEditingControls(bool dis)
{
    m_textEdit->setReadOnly(dis);
    ui->btnLoad->setDisabled(dis);
}

void ProcedureForm::toggleGuidedCreationControls(bool guided)
{
    ui->lblProcName->setVisible(guided);
    ui->txtProcName->setVisible(guided);
    ui->txtBrief->setVisible(guided);
    ui->lblBrief->setVisible(guided);
    ui->grpParameters->setVisible(guided);
    ui->lblReturnType->setVisible(guided);
    ui->cmbReturnType->setVisible(guided);

    if(!guided)
    {
        ui->tabWidget->removeTab(1);
    }
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
        if(!m_guided)
        {
            getProcNameFromSql();
        }
    }
}

void ProcedureForm::initSql()
{
    QString sql = strCreate + " " + (m_mode == MODE_PROCEDURE?"PROCEDURE ":"FUNCTION ") + m_proc->getName();
    ui->txtProcName->setText(m_proc->getName());
    sql += "()";

    UserDataType *dt = m_version->getDataType(ui->cmbReturnType->currentText()); // 0 if none found, used down
    QString returns = (dt ? dt->sqlAsString() : "INT");

    if(m_mode == MODE_FUNCTION) sql += " RETURNS " + (m_guided? returns : "");
    sql += "\nBEGIN\n\nEND";
    m_forcedChange = true;
    m_textEdit->setPlainText(sql);
    m_forcedChange = false;
    m_textEdit->updateLineNumbers();

    if(!m_guided)
    {
        return;
    }
    m_textEdit->disableRow(1); // disabling the first row
    m_textEdit->disableRow(2); // and the one with BEGIN
    m_textEdit->updateLineNumbers();
    m_init = false;
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
        //ui->grpContent->setEnabled(true);
        disableEditingControls(false);
        m_proc->unlock();
        m_proc->updateGui();
        ui->btnLock->setToolTip((m_mode == MODE_PROCEDURE?"Procedure ":"Function ") + QObject::tr("is <b>unlocked</b>. Click this button to lock it."));

        MainWindow::instance()->finallyDoLockLikeOperation(false, m_proc->getObjectUid());
    }
    else
    {
        ui->btnLock->setIcon(IconFactory::getLockedIcon());
        //ui->grpContent->setEnabled(false);
        disableEditingControls(true);
        m_proc->lock(LockableElement::LOCKED);
        m_proc->updateGui();
        ui->btnLock->setToolTip((m_mode == MODE_PROCEDURE?"Procedure ":"Function ") + QObject::tr("is <b>locked</b>. Click this button to lock it."));

        MainWindow::instance()->finallyDoLockLikeOperation(true, m_proc->getObjectUid());
    }
}

void ProcedureForm::setProcedure(StoredMethod *p)
{
    m_proc = p;
    toggleGuidedCreationControls(p->isGuided());

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
            //ui->grpContent->setEnabled(false);
            disableEditingControls(true);
            ui->btnLock->setToolTip((m_mode == MODE_PROCEDURE?"This Procedure ":"This Function ") + QObject::tr("is <b>locked</b>. Click this button to unlock it."));
        }
        else
        {
            ui->btnLock->setIcon(IconFactory::getUnLockedIcon());
            ui->btnLock->blockSignals(true);
            ui->btnLock->setChecked(true);
            ui->btnLock->blockSignals(false);
            //ui->grpContent->setEnabled(true);
            disableEditingControls(false);
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
            //ui->grpContent->setEnabled(false);
            disableEditingControls(true);
            ui->btnLock->setToolTip((m_mode == MODE_PROCEDURE?"This Procedure ":"This Function ") + QObject::tr("is <b>locked</b>. Click this button to unlock it."));
        }
        else
        {
            ui->btnLock->setIcon(IconFactory::getUnLockedIcon());
            ui->btnLock->blockSignals(true);
            ui->btnLock->setChecked(true);
            ui->btnLock->blockSignals(false);
            //ui->grpContent->setEnabled(true);
            disableEditingControls(false);
            ui->btnLock->setToolTip((m_mode == MODE_PROCEDURE?"This Procedure ":"This Function ") + QObject::tr("is <b>unlocked</b>. Click this button to unlock it."));
        }
        ui->btnUndelete->hide();
        ui->btnLock->show();
    }
}


void ProcedureForm::onSave()
{
    QString name = QFileDialog::getSaveFileName(this, tr("Save sql"), "", tr("SQL files (*.sql)"));
    QFile file(name);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return;
    }
    QTextStream out(&file);
    out << m_textEdit->toPlainText() << "\n";
}

void ProcedureForm::onLoadFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,  "Load values", "", "SQL files (*.sql)");
    if(fileName.length() == 0)
    {
        return;
    }

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    QString sql = "";
    QTextStream in(&file);

    while (!in.atEnd())
    {
        QString line = in.readLine();
        sql += line;
        sql += strNewline;
    }

    m_forcedChange = true;
    m_textEdit->setPlainText(sql);
    m_forcedChange = false;
    m_textEdit->updateLineNumbers();

}

void ProcedureForm::onInject()
{
}

void ProcedureForm::onNew()
{
}

void ProcedureForm::onReturnTypeComboChange(QString a)
{
    UserDataType *dt = m_version->getDataType(a); // 0 if none found, used down
    QString returns = (dt ? dt->sqlAsString() : a);

    if(m_init) return;

    QString plainTextEditContents = m_textEdit->toPlainText();
    QStringList lines = plainTextEditContents.split("\n");

    if(lines.empty())
    {
        return;
    }

    m_proc->setReturn(returns);
    QString line0 = lines.at(0);
    int idxOfReturns = line0.indexOf("RETURNS", Qt::CaseInsensitive);
    line0 = line0.left(idxOfReturns + 8) + " " + returns;
    lines[0] = line0;
    m_textEdit->setPlainText(lines.join("\n"));
}

void ProcedureForm::onProcNameChange(QString a)
{
    if(m_init)
    {
        return;
    }

    if(a == "UNNAMED")
    {
        return;
    }

    if(m_proc->isDeleted())
    {
        return;
    }

    QString plainTextEditContents = m_textEdit->toPlainText();
    QStringList lines = plainTextEditContents.split("\n");

    if(lines.empty())
    {
        return;
    }

    QString t = m_mode == MODE_PROCEDURE?"PROCEDURE":"FUNCTION";
    QString line0 = lines.at(0);
    int idxOfType = line0.indexOf(t, Qt::CaseInsensitive);
    int i = idxOfType + t.length();
    while(line0.at(i).isSpace()) i++;   // skip space
    while(line0.at(i).isLetterOrNumber() || line0.at(i) == '_') i++; // skip the name
    line0 = line0.left(idxOfType + t.length() + 1) + a + line0.mid(i);
    lines[0] = line0;
    m_textEdit->setPlainText(lines.join("\n"));
    m_proc->setDisplayText(a);
    m_proc->setName(a);
}

void ProcedureForm::onBriefDescrChange(QString a)
{
    m_proc->setBriefDescr(a);
}

void ProcedureForm::onDescrChange()
{
    m_proc->setDescription(ui->txtDescription->toPlainText());
}

void ProcedureForm::updateSqlDueToParamChange()
{
    QString plainTextEditContents = m_textEdit->toPlainText();
    QStringList lines = plainTextEditContents.split("\n");

    if(!lines.empty())
    {
        QString line0 = lines.at(0);
        int idxOfOpenParen = line0.indexOf("(");
        int idxOfCloseParen = idxOfOpenParen;
        int level = 0;
        while(idxOfCloseParen < line0.length())
        {
            idxOfCloseParen ++;
            if(line0[idxOfCloseParen] == '(') level ++;
            if(line0[idxOfCloseParen] == ')')
            {
                level --;
                if(level == -1)
                {
                    break;
                }
            }
        }

        QString params = "";
        const QVector<ParameterAndDescription>& pars = m_proc->getParametersWithDescription();
        for(int i=0; i<pars.size(); i++)
        {
            UserDataType *dt = m_version->getDataType(pars[i].m_type); // 0 if none found, used down
            QString parT = (dt ? dt->sqlAsString() : pars[i].m_type);
            params += pars[i].m_direction.trimmed() + (pars[i].m_direction.trimmed().isEmpty()?"":" ") + pars[i].m_parameter + " " + parT;
            if(i < pars.size() - 1)
            {
                params += ", ";
            }
        }
        QString newLine0 = line0.left(idxOfOpenParen) + "(" + params + line0.mid(idxOfCloseParen);
        lines[0] = newLine0;
        m_textEdit->setPlainText(lines.join("\n"));
    }
}

void ProcedureForm::onAddParameter()
{
    QStringList paramEntry;

    ParameterAndDescription pd(ui->txtParamName->text(),
                               ui->cmbParameterType->currentText(),
                               ui->txtParamDesc->toPlainText(),
                               ui->cmbParameterDirection->currentText(), GUIDED);

    if(m_proc->hasParameter(pd.m_parameter))
    {
        QMessageBox::critical(this, tr("Error"), tr("You cannot have another parameter named:<b> ") + pd.m_parameter, QMessageBox::Ok);
        return;
    }

    m_proc->addParameter(pd);
    paramEntry << pd.m_parameter << pd.m_direction << pd.m_type << pd.m_description;
    QTreeWidgetItem* itm = new QTreeWidgetItem(paramEntry);
    ui->lstParameters->addTopLevelItem(itm);

    // and now patch the textfield
    updateSqlDueToParamChange();
}

void ProcedureForm::onDeleteParameter()
{
    if(ui->lstParameters->currentItem() == 0)
    {
        return;
    }

    QString paramName = ui->lstParameters->currentItem()->text(1);
    m_proc->removeParameter(paramName);
    delete ui->lstParameters->currentItem();

    // and now patch the textfield
    updateSqlDueToParamChange();
}

void ProcedureForm::onMoveUpParameter()
{
    QTreeWidgetItem* item = ui->lstParameters->currentItem();
    int              row  = ui->lstParameters->currentIndex().row();

    if (item && row > 0)
    {
        ui->lstParameters->takeTopLevelItem(row);
        ui->lstParameters->insertTopLevelItem(row - 1, item);
        ui->lstParameters->setCurrentItem(item);

        m_proc->moveUpParameter(row);
        updateSqlDueToParamChange();
    }
}

void ProcedureForm::onMoveDownParameter()
{
    QTreeWidgetItem* item = ui->lstParameters->currentItem();
    int              row  = ui->lstParameters->currentIndex().row();

    if (item && row < ui->lstParameters->topLevelItemCount() - 1)
    {
        ui->lstParameters->takeTopLevelItem(row);
        ui->lstParameters->insertTopLevelItem(row + 1, item);
        ui->lstParameters->setCurrentItem(item);

        m_proc->moveDownParameter(row);
        updateSqlDueToParamChange();
    }
}
