#include "NewDataTypeForm.h"
#include "ui_NewDataTypeForm.h"
#include "MainWindow.h"
#include "db_DatabaseEngine.h"
#include "core_UserDataType.h"
#include "db_AbstractDTSupplier.h"
#include "db_Codepage.h"
#include "core_UserDataType.h"
#include "strings.h"
#include "Configuration.h"
#include "Workspace.h"
#include "Version.h"
#include "VersionGuiElements.h"
#include "gui_HelpWindow.h"
#include "MainWindow.h"
#include "GuiElements.h"

#include <QMessageBox>
#include <QComboBox>
#include <QListWidget>
#include <QListWidgetItem>

NewDataTypeForm::NewDataTypeForm(Version *v, DT_TYPE t, DatabaseEngine* dbe, QWidget *parent) :
    QWidget(parent), m_ui(new Ui::NewDataTypeForm), m_dbEngine(dbe), m_udt(0), m_version(v)
{
    m_ui->setupUi(this);

    m_ui->lblUnsigned->hide();
    m_ui->chkUnsigned->hide();

    m_ui->lblWidth->hide();
    m_ui->txtWidth->hide();

    m_ui->lblValues->hide();
    m_ui->lstEnumValues->hide();
    m_ui->txtEnumCurrentValue->hide();
    m_ui->btnAddEnumValue->hide();
    m_ui->btnRemoveEnumValue->hide();

    resetContent();

    switch(t)
    {
    case DT_STRING :
        m_ui->cmbDTType->setCurrentIndex(0);
        break;
    case DT_NUMERIC :
        m_ui->cmbDTType->setCurrentIndex(1);
        break;
    case DT_BOOLEAN :
        m_ui->cmbDTType->setCurrentIndex(2);
        break;
    case DT_DATETIME :
        m_ui->cmbDTType->setCurrentIndex(3);
        break;
    case DT_BLOB :
        m_ui->cmbDTType->setCurrentIndex(4);
        break;
    case DT_MISC :
        m_ui->cmbDTType->setCurrentIndex(5);
        break;
    case DT_SPATIAL :
        m_ui->cmbDTType->setCurrentIndex(6);
        break;
    case DT_INVALID :
    case DT_GENERIC:
    default:
        m_ui->cmbDTType->setCurrentIndex(-1);
        break;
    }

    m_ui->frameForUnlockButton->hide();
}

NewDataTypeForm::~NewDataTypeForm()
{
    delete m_ui;
}

void NewDataTypeForm::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void NewDataTypeForm::focusOnName()
{
    m_ui->txtDTName->setFocus();

}

void NewDataTypeForm::basicDTselected(QString newSelection)
{
    m_ui->cmbDTSQLType->clear();
    m_ui->lblUnsigned->hide();
    m_ui->chkUnsigned->hide();

    hideSpecialComponents();

    DT_TYPE type = UserDataType::getDT_TYPE(newSelection);
    QList<UserDataType> types = m_dbEngine->getDTSupplier()->getDTList(type);

    for(int i=0; i< types.size(); i++)
    {
        m_ui->cmbDTSQLType->addItem(IconFactory::getIconForDataType(type), types[i].getName());
    }
    if(newSelection == strNumeric)
    {
        m_ui->lblUnsigned->show();
        m_ui->chkUnsigned->show();
    }
}

void NewDataTypeForm::hideSpecialComponents()
{
    m_ui->lblWidth->hide();
    m_ui->txtWidth->hide();

    m_ui->lblValues->hide();
    m_ui->lstEnumValues->hide();
    m_ui->txtEnumCurrentValue->hide();
    m_ui->btnAddEnumValue->hide();
    m_ui->btnRemoveEnumValue->hide();

    m_ui->txtDefaultValue->show();
    m_ui->cmbEnumItems->hide();
}

void NewDataTypeForm::resetContent()
{
    m_ui->txtDTName->setText("");
    m_ui->cmbDTSQLType->setCurrentIndex(-1);
    m_ui->cmbDTType->setCurrentIndex(-1);
    m_ui->txtWidth->setText("");
    m_ui->chkUnsigned->setChecked(false);

    m_ui->lblUnsigned->hide();
    m_ui->chkUnsigned->hide();

    hideSpecialComponents();

    m_ui->txtEnumCurrentValue->clear();
    m_ui->lstEnumValues->clear();

    m_ui->tabWidget->setCurrentIndex(0);
    m_ui->txtDescription->clear();
    m_ui->chkCanBeNull->setChecked(true);
    m_ui->chkNullIsDefault->setChecked(false);

    m_ui->txtDTName->setFocus();

}

void NewDataTypeForm::onSave()
{
    if(!m_ui->chkCanBeNull->isChecked() && m_ui->chkNullIsDefault->isChecked())
    {
        QMessageBox::critical(this, "Error", "This data type cannot be null, however it's default value is set to null", QMessageBox::Ok);
        m_ui->tabWidget->setCurrentIndex(1);
        return;
    }

    QStringList mv;
    for(int i=0; i<m_ui->lstEnumValues->count(); i++)
    {
        mv.append(m_ui->lstEnumValues->item(i)->text());
    }

    QString defaultValue = m_ui->txtDefaultValue->text();
    if( UserDataType::getDT_TYPE(m_ui->cmbDTType->currentText()) == 5)  // MISC type (Enum or Set)
    {
        defaultValue = m_ui->cmbEnumItems->currentText();
    }

    if(m_ui->chkNullIsDefault->isChecked())
    {
        defaultValue = "null";
    }

    if(MainWindow::instance()->onSaveNewDataType(m_ui->txtDTName->text(),
                               m_ui->cmbDTType->currentText(),
                               m_ui->cmbDTSQLType->currentText(),
                               m_ui->txtWidth->text(),
                               defaultValue,
                               mv, m_ui->txtDescription->toPlainText(),
                               m_ui->chkUnsigned->isChecked(),
                               m_ui->chkCanBeNull->isChecked(),
                               m_udt, m_version))
    {
        resetContent();
    }
}

void NewDataTypeForm::onReset()
{
    resetContent();
}


void NewDataTypeForm::onSqlTypeSelected(QString selectedItem)
{
    int size = m_dbEngine->getDTSupplier()->maximumSize(selectedItem);

    m_ui->txtDefaultValue->show();
    m_ui->cmbEnumItems->hide();
    // enum/set?
    if(size == -1)
    {
        m_ui->lblValues->show();
        m_ui->lstEnumValues->show();
        m_ui->txtEnumCurrentValue->show();
        m_ui->btnAddEnumValue->show();
        m_ui->btnRemoveEnumValue->show();
        m_ui->cmbEnumItems->show();
        m_ui->txtDefaultValue->hide();
    }
    else
    if(size == 0)// size not supported
    {
        m_ui->txtEnumCurrentValue->clear();
        m_ui->lstEnumValues->clear();
        hideSpecialComponents();
        m_ui->txtWidth->setText("");
    }
    else
    if(size > 0)
    {
        hideSpecialComponents();
        m_ui->txtEnumCurrentValue->clear();
        m_ui->lstEnumValues->clear();

        m_ui->lblWidth->show();
        m_ui->txtWidth->show();

        m_ui->txtWidth->clear();

        if(size != 1 && Configuration::instance().defaultLengths())
        {
            m_ui->txtWidth->setText(QString::number(size));
        }
    }
}

void NewDataTypeForm::onAddMiscValue()
{
    QListWidgetItem* newMiscValue = new QListWidgetItem(m_ui->txtEnumCurrentValue->text());
    m_ui->lstEnumValues->addItem(newMiscValue);
    m_ui->cmbEnumItems->addItem(m_ui->txtEnumCurrentValue->text());
    m_ui->cmbEnumItems->setCurrentIndex(-1);
    m_ui->txtEnumCurrentValue->clear();

}

void NewDataTypeForm::onRemoveSelectedMiscValue()
{
    if(m_ui->lstEnumValues->selectedItems().size() > 0)
    {
        int x = m_ui->lstEnumValues->currentRow();
        QString s = m_ui->lstEnumValues->item(x)->text();
        m_ui->lstEnumValues->takeItem (x);
        m_ui->cmbEnumItems->removeItem(x);
    }
}

void NewDataTypeForm::setDataType(UserDataType* udt)
{
    if(udt == 0 || udt->typeAsString() == strNA)
    {
        QMessageBox::critical (this, tr("Error"), tr("This is an invalid DataType"), QMessageBox::Ok);
        return;
    }

    m_udt = udt;

    m_ui->txtDTName->setText(m_udt->getName());

    // populate the combo boxes
    if(m_ui->cmbDTType->findText(udt->typeAsString()) != -1)
    {
        m_ui->cmbDTType->setCurrentIndex(m_ui->cmbDTType->findText(udt->typeAsString()));
    }
    else
    {
        QMessageBox::critical (this, tr("Error"), tr("This is an invalid DataType"), QMessageBox::Ok);
        return;
    }

    int isqlty = m_ui->cmbDTSQLType->findText(udt->getSqlType()) ;
    if(isqlty != -1)
    {
        m_ui->cmbDTSQLType->setCurrentIndex(isqlty);
    }
    else
    {
        QMessageBox::critical (this, tr("Error"), tr("This is an invalid DataType for this DB engine"), QMessageBox::Ok);
        return;
    }

    // width, unsigned, default
    m_ui->txtWidth->setText(udt->getSize());
    m_ui->chkUnsigned->setChecked(udt->isUnsigned());
    m_ui->txtDefaultValue->setText(udt->getDefaultValue());

    // populate the "misc" entries, if any
    for(int i=0; i<udt->getMiscValues().count(); i++)
    {
        QListWidgetItem* newMiscValue = new QListWidgetItem(udt->getMiscValues()[i]);
        m_ui->lstEnumValues->addItem(newMiscValue);
        m_ui->cmbEnumItems->addItem(udt->getMiscValues()[i]);
    }

    // now find the default item (if any) and focus on it in the cmbEnumItems
    m_ui->cmbEnumItems->setCurrentIndex(-1);
    for(int i=0; i<udt->getMiscValues().count(); i++)
    {
        if(udt->getDefaultValue() == udt->getMiscValues()[i])
        {
            m_ui->cmbEnumItems->setCurrentIndex(i);
            break;
        }
    }

    m_ui->chkNullIsDefault->setChecked(false);
    if(udt->getDefaultValue() == "null")
    {
        m_ui->cmbEnumItems->setEnabled(false);
        m_ui->chkNullIsDefault->setChecked(true);
        m_ui->txtDefaultValue->setEnabled(false);
    }

    if(!udt->isNullable())
    {
        m_ui->chkNullIsDefault->setEnabled(false);
        m_ui->chkCanBeNull->setChecked(false);
    }
    else
    {
        m_ui->chkCanBeNull->setChecked(true);
    }

    m_ui->txtDescription->setText(udt->getDescription());

    m_ui->btnUndelete->hide();
    // TODO: Duplicate from the other forms
    if(!m_udt->wasLocked())
    {
        if(m_udt->isDeleted())
        {
            m_ui->frameForUnlockButton->show();
            m_ui->btnLock->hide();
            m_ui->btnUndelete->show();
        }
        else
        {
            m_ui->frameForUnlockButton->hide();
        }
    }
    else
    {
        m_ui->frameForUnlockButton->show();
        if(m_udt->isLocked())
        {
            m_ui->btnLock->setIcon(IconFactory::getLockedIcon());
            m_ui->btnLock->blockSignals(true);
            m_ui->btnLock->setChecked(false);
            m_ui->btnLock->blockSignals(false);
            m_ui->grpContent->setEnabled(false);
            m_ui->btnLock->setToolTip(QObject::tr("This data type is <b>locked</b>. Click this button to unlock it."));
        }
        else
        {
            m_ui->btnLock->setIcon(IconFactory::getUnLockedIcon());
            m_ui->btnLock->blockSignals(true);
            m_ui->btnLock->setChecked(true);
            m_ui->btnLock->blockSignals(false);
            m_ui->grpContent->setEnabled(true);
            m_ui->btnLock->setToolTip(QObject::tr("This data type is <b>unlocked</b>. Click this button to lock it."));
        }

        if(m_udt->isDeleted())
        {
            m_ui->btnLock->hide();
            m_ui->btnUndelete->show();
        }
    }
}

void NewDataTypeForm::onLockUnlock(bool checked)
{
    if(checked)
    {
        m_ui->btnLock->setIcon(IconFactory::getUnLockedIcon());
        m_ui->grpContent->setEnabled(true);
        m_udt->unlock();
        m_udt->updateGui();
        m_ui->btnLock->setToolTip(QObject::tr("This data type is <b>unlocked</b>. Click this button to lock it."));

        MainWindow::instance()->finallyDoLockLikeOperation(false, m_udt->getObjectUid());
    }
    else
    {
        m_ui->btnLock->setIcon(IconFactory::getLockedIcon());
        m_ui->grpContent->setEnabled(false);
        m_udt->lock();
        m_udt->updateGui();
        m_ui->btnLock->setToolTip(QObject::tr("This data type is <b>locked</b>. Click this button to unlock it."));

        MainWindow::instance()->finallyDoLockLikeOperation(true, m_udt->getObjectUid());
    }
}

void NewDataTypeForm::onNullClicked()
{
    if(m_ui->chkNullIsDefault->isChecked())
    {
        m_ui->cmbEnumItems->setEnabled(false);
        m_ui->txtDefaultValue->setEnabled(false);
    }
    else
    {
        m_ui->cmbEnumItems->setEnabled(true);
        m_ui->txtDefaultValue->setEnabled(true);
    }
}


void NewDataTypeForm::onCanBeNullClicked()
{
    if(m_ui->chkCanBeNull->isChecked())
    {
        m_ui->chkNullIsDefault->setEnabled(true);
    }
    else
    {
        m_ui->chkNullIsDefault->setEnabled(false);
    }
}

void NewDataTypeForm::keyPressEvent(QKeyEvent *evt)
{
    if(evt->key() == Qt::Key_Return)
    {
        onSave();
    }
    else
    {
        QWidget::keyPressEvent(evt);
    }
}

void NewDataTypeForm::onHelp()
{
    HelpWindow* hw = HelpWindow::instance();
    hw->showHelp(QString("/doc/dtyp.html"));
    hw->show();
}


void NewDataTypeForm::onUndelete()
{
    if(m_udt->version()->undeleteObject(m_udt->getObjectUid()))
    {
        MainWindow::instance()->getGuiElements()->removeItemForPatch(m_udt->version()->getWorkingPatch(), m_udt->getObjectUid());
        // TODO: Duplicate from above
        if(m_udt->isLocked())
        {
            m_ui->btnLock->setIcon(IconFactory::getLockedIcon());
            m_ui->btnLock->blockSignals(true);
            m_ui->btnLock->setChecked(false);
            m_ui->btnLock->blockSignals(false);
            m_ui->grpContent->setEnabled(false);
            m_ui->btnLock->setToolTip(QObject::tr("This Data Type instance is <b>locked</b>. Click this button to unlock it."));
        }
        else
        {
            m_ui->btnLock->setIcon(IconFactory::getUnLockedIcon());
            m_ui->btnLock->blockSignals(true);
            m_ui->btnLock->setChecked(true);
            m_ui->btnLock->blockSignals(false);
            m_ui->grpContent->setEnabled(true);
            m_ui->btnLock->setToolTip(QObject::tr("This table instance is <b>unlocked</b>. Click this button to lock it."));
        }
        m_ui->btnUndelete->hide();
        m_ui->btnLock->show();
    }
}
