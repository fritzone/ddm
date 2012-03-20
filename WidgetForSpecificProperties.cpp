#include "WidgetForSpecificProperties.h"
#include "ui_WidgetForSpecificProperties.h"
#include "uids.h"
#include "db_SP.h"
#include "SpInstance.h"
#include "ObjectWithSpInstances.h"
#include "SpsTooltipProviderForUid.h"
#include "ValueListSp.h"
#include "ValueListSpInstance.h"

#include <QLabel>
#include <QCheckBox>
#include <QDebug>
#include <QWidget>
#include <QComboBox>

WidgetForSpecificProperties::WidgetForSpecificProperties(const DatabaseEngine* dbe, ObjectWithSpInstances* osp, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetForSpecificProperties), m_mappings(),
    m_signalMapper(new QSignalMapper(this)), m_osp(osp), m_dbEngine(dbe), m_GroupToIndex()
{
    ui->setupUi(this);

    toolBox = new QToolBox(this);
    toolBox->setObjectName(QString::fromUtf8("toolBox"));

    ui->verticalLayout->addWidget(toolBox);
}

WidgetForSpecificProperties::~WidgetForSpecificProperties()
{
    delete ui;
}

void WidgetForSpecificProperties::changeEvent(QEvent *e)
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

QWidget* WidgetForSpecificProperties::getToolboxPageForText(const QString& s)
{
    for(int i=0; i<toolBox->count(); i++)
    {
        if(toolBox->widget(i)->windowTitle() == s)
        {
            return toolBox->widget(i);
        }
    }

    return 0;
}

void WidgetForSpecificProperties::feedInSpecificProperties(const QVector<SpInstance*>& spInstances, const QString& dbDestinationUid)
{
    QStringList alreadyAddedPages;
    m_GroupToIndex.clear();

    QMap<QString,int> m_rowsForGroups;

    for(int i=0; i<spInstances.size(); i++)
    {
        QString group = spInstances.at(i)->getClass()->getSpGroup();
        QWidget *page = 0;
        if(!alreadyAddedPages.contains(group))
        {
            page = new QWidget();
            page->setObjectName(QString::fromUtf8("generalPage"));
            page->setGeometry(QRect(0, 0, 382, 251));
            formLayout = new QFormLayout(page);
            formLayout->setObjectName(QString::fromUtf8("formLayout_2"));
            formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
            int idx = toolBox->addItem(page, group);
            m_GroupToIndex[group] = idx;
            alreadyAddedPages.append(group);
            m_rowsForGroups[group] = 0;
        }
        if(page == 0)
        {
            int idx = m_GroupToIndex[group];
            page = toolBox->widget(idx);
            if(page == 0)
            {
                return ;
            }

            m_rowsForGroups[group] ++;
        }

        if(spInstances.at(i)->getClass()->getReferredObjectClassUid() == dbDestinationUid)
        {
//            QLabel* label = new QLabel(this);
//            label->setText(sps.at(i)->getClass()->getPropertyGuiText());
//            ui->formLayout->setWidget(i, QFormLayout::LabelRole, label);

            if(spInstances.at(i)->getClass()->getClassUid().toString() == uidTrueFalseSp)   // create a check box
            {
                QCheckBox* checkBox = new QCheckBox(page);
                checkBox->setText(spInstances.at(i)->getClass()->getPropertyGuiText());
                checkBox->setToolTip(SpsTooltipProviderForUid::provideTooltipForUid(spInstances.at(i)->getClass()->getSqlRoleUid()));
                formLayout->setWidget(m_rowsForGroups[group], QFormLayout::FieldRole, checkBox);
                UidToWidget* uiw = new UidToWidget();
                uiw->objectUid = spInstances.at(i)->getObjectUid();
                uiw->w = checkBox;
                m_mappings.append(uiw);

                m_signalMapper->setMapping(checkBox, spInstances.at(i)->getObjectUid());
                connect(checkBox, SIGNAL(clicked()), m_signalMapper, SLOT(map()));

                const QString& v = spInstances.at(i)->get();
                if(v == "TRUE")
                {
                    checkBox->setChecked(true);
                }
                else
                {
                    checkBox->setChecked(false);
                }
            }

            if(spInstances.at(i)->getClass()->getClassUid().toString() == uidValueListSp)   // create a combo box
            {
                QComboBox* comboBox = new QComboBox(this);
                const ValueListSp* spi = dynamic_cast<const ValueListSp*>(spInstances.at(i)->getClass());
                if(spi)
                {
                    QStringList values = spi->getValues();
                    for(int j=0; j<values.size(); j++)
                    {
                        comboBox->addItem(values.at(j));
                    }
                }

                QLabel* label = new QLabel(this);
                label->setText(spInstances.at(i)->getClass()->getPropertyGuiText());
                formLayout->setWidget(m_rowsForGroups[group], QFormLayout::LabelRole, label);
                formLayout->setWidget(m_rowsForGroups[group], QFormLayout::FieldRole, comboBox);
                UidToWidget* uiw = new UidToWidget();
                uiw->objectUid = spInstances.at(i)->getObjectUid();
                uiw->w = comboBox;
                m_mappings.append(uiw);
                m_signalMapper->setMapping(comboBox, spInstances.at(i)->getObjectUid());
                connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBoxSelected(int)));
            }
        }
    }

    connect(m_signalMapper, SIGNAL(mapped(QString)), this, SLOT(checkBoxToggled(QString)));

}

void WidgetForSpecificProperties::comboBoxSelected(int i)
{
    qDebug() << i;
    QComboBox *combo = qobject_cast<QComboBox *>(sender());
    if(combo != 0)
    {
        qDebug() << combo->currentText();
    }

}

void WidgetForSpecificProperties::checkBoxToggled(QString uid)
{
    QCheckBox* cb = getCheckBoxForObjectUid(uid);
    if(cb)
    {
        qDebug() << cb->isChecked() << " " << uid;
        bool b = cb->isChecked();
        SpInstance* spi = m_osp->getInstance(m_dbEngine, uid);
        if(spi)
        {
            spi->set(b?"TRUE":"FALSE");
        }
    }
}

QCheckBox* WidgetForSpecificProperties::getCheckBoxForObjectUid(const QString& uid)
{
    for(int i=0; i<m_mappings.size(); i++)
    {
        UidToWidget* uiw = m_mappings.at(i);
        if(uiw->objectUid == uid)
        {
            return dynamic_cast<QCheckBox*>(uiw->w);
        }
    }
    return 0;
}

QString WidgetForSpecificProperties::getObjectUidForWidget(const QWidget* w)
{
    for(int i=0; i<m_mappings.size(); i++)
    {
        UidToWidget* uiw = m_mappings.at(i);
        if(uiw->w == w)
        {
            return (uiw->objectUid);
        }
    }
    return 0;
}
