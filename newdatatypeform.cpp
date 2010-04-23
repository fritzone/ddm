#include "newdatatypeform.h"
#include "ui_newdatatypeform.h"
#include "mainwindow.h"
#include "DatabaseEngine.h"
#include "DataType.h"
#include "AbstractDTSupplier.h"
#include "AbstractCodepageSupplier.h"
#include "Codepage.h"
#include "UserDataType.h"
#include "strings.h"
#include "Configuration.h"

#include <QMessageBox>
#include <QListWidget>
#include <QListWidgetItem>

NewDataTypeForm::NewDataTypeForm(DatabaseEngine* dbe, QWidget *parent) :
        QWidget(parent), m_ui(new Ui::NewDataTypeForm), m_mw(0),
        m_dbEngine(dbe), m_udt(0)
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

    populateCodepageCombo();
    resetContent();
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
void NewDataTypeForm::setMainWindow(MainWindow* mw)
{
    m_mw = mw;
}

void NewDataTypeForm::buttonClicked(QAbstractButton* btn)
{
    if(btn == m_ui->buttons->buttons().at(0)) // Seems very strange, but works like this... Save is the First, after Reset
    {
        onSave();
    }
    else
    {
        onReset();
    }
}

void NewDataTypeForm::basicDTselected(QString newSelection)
{
    m_ui->cmbDTSQLType->clear();
    m_ui->lblUnsigned->hide();
    m_ui->chkUnsigned->hide();

    hideSpecialComponents();

    DataType::DT_TYPE type = DataType::getDT_TYPE(newSelection);
    QList<DataType> types = AbstractDTSupplier::getDTList(m_dbEngine->getDTSupplier(), type);

    for(int i=0; i< types.size(); i++)
    {
        m_ui->cmbDTSQLType->addItem(QIcon(DataType::getIconString(type)),
                                    types[i].getName());
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

    m_ui->toolBox->setCurrentIndex(0);

    m_ui->txtDTName->setFocus();
}

void NewDataTypeForm::onSave()
{
    if(m_mw != 0)
    {
        QString cp = m_ui->cmbCharacterSets->currentIndex() > 0 ? m_dbEngine->getCodepageSupplier()->getCodepages()[m_ui->cmbCharacterSets->currentIndex()]->getName() : "";
        QStringList mv;
        for(int i=0; i<m_ui->lstEnumValues->count(); i++)
        {
            mv.append(m_ui->lstEnumValues->item(i)->text());
        }

        if(m_mw->onSaveNewDataType(m_ui->txtDTName->text(),
                                   m_ui->cmbDTType->currentText(),
                                   m_ui->cmbDTSQLType->currentText(),
                                   m_ui->txtWidth->text(),
                                   m_ui->txtDefaultValue->text(),
                                   cp, mv,
                                   m_ui->chkUnsigned->checkState() == Qt::Checked, m_udt))
        {
            resetContent();
        }
    }
}

void NewDataTypeForm::onReset()
{
    resetContent();
}

void NewDataTypeForm::populateCodepageCombo()
{
    QVector<Codepage*> cps = m_dbEngine->getCodepageSupplier()->getCodepages();
    QListWidget* lw = new QListWidget(this);
    for(int i=0; i<cps.size(); i++)
    {
        QString name = cps[i]->getName();
        bool header = false;
        if(cps[i]->getName().startsWith(QString("--")))
        {
            header = true;
            name = name.right(name.length() - 2);
        }
        QString iconName = "";

        if(!header)
        {
        // dig out the second string
            QStringList ls = name.split("_");
            if(ls.size() > 1)
            {

                if(ls[1] != "bin" && ls[1] != "unicode" && ls[1] != "general")
                {
                    iconName = ":/images/actions/images/small/flag_" + ls[1] + ".PNG";
                }
                else
                {
                    if(ls[0] == "greek")
                    {
                        iconName = ":/images/actions/images/small/flag_greek.PNG";
                    }
                    else
                    if(ls[0] == "armscii8")
                    {
                        iconName = ":/images/actions/images/small/flag_armenia.PNG";
                    }
                    else
                    if(ls[0] == "hebrew")
                    {
                        iconName = ":/images/actions/images/small/flag_israel.PNG";
                    }
                    else
                    {
                        iconName = ":/images/actions/images/small/flag_" + ls[1] + ".PNG";
                    }
                }

                ls[1][0] = ls[1][0].toUpper();

                name = ls[1] + " (" + ls[0];
                if(ls.size() > 2)
                {
                    name += ", " + ls[2];
                }
                name += ")";
            }
        }

        // create the lw object
        QListWidgetItem* lwi = new QListWidgetItem(name);
        QFont font = lwi->font();
        if(iconName.length() > 0)
        {
            lwi->setIcon(QIcon(iconName));
        }

        if(header)
        {
            font.setBold(true);
            font.setItalic(true);
            font.setPointSize(font.pointSize() + 1);
        }

        lwi->setFont(font);

        lw->addItem(lwi);
    }

    m_ui->cmbCharacterSets->setModel(lw->model());
    m_ui->cmbCharacterSets->setView(lw);
    m_ui->cmbCharacterSets->setCurrentIndex(-1);

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


void NewDataTypeForm::onCharacterSetCmbChanged(int i)
{
    QVector<Codepage*> cps = m_dbEngine->getCodepageSupplier()->getCodepages();
    if(cps[i]->getName()[0] == '-')
    {
        m_ui->cmbCharacterSets->setCurrentIndex(i + 1);
    }
}

void NewDataTypeForm::onAddMiscValue()
{
    QListWidgetItem* newMiscValue = new QListWidgetItem(m_ui->txtEnumCurrentValue->text());
    m_ui->lstEnumValues->addItem(newMiscValue);
    m_ui->cmbEnumItems->addItem(m_ui->txtEnumCurrentValue->text());

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

    // find the codepage
    QVector<Codepage*> cps = m_dbEngine->getCodepageSupplier()->getCodepages();
    int idx = -1;
    for(int i=0; i< cps.size(); i++)
    {
        if(cps[i]->getName().compare(udt->getCodepage()) == 0)
        {
            idx = i;
            break;
        }
    }
    m_ui->cmbCharacterSets->setCurrentIndex(idx);

    // populate the "misc" entries, if any
    for(int i=0; i<udt->getMiscValues().count(); i++)
    {
        QListWidgetItem* newMiscValue = new QListWidgetItem(udt->getMiscValues()[i]);
        m_ui->lstEnumValues->addItem(newMiscValue);
    }
}
