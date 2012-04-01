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
#include <QListWidget>
#include <QComboBox>

WidgetForSpecificProperties::WidgetForSpecificProperties(const DatabaseEngine* dbe, ObjectWithSpInstances* osp, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetForSpecificProperties), m_mappings(),
    m_signalMapper(new QSignalMapper(this)), m_osp(osp), m_dbEngine(dbe), m_GroupToIndex()
{
    ui->setupUi(this);

    toolBox = new QToolBox(this);
    toolBox->setObjectName(QString::fromUtf8("toolBox"));
    toolBox->layout()->setSpacing(0);

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

void WidgetForSpecificProperties::populateCodepageCombo(QComboBox* comboBox, const QStringList& cps)
{
    QListWidget* lw = new QListWidget(this);
    for(int i=0; i<cps.size(); i++)
    {
        QString name = cps[i];
        bool header = false;
        if(cps[i].startsWith(QString("--")))
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
                    iconName = ":/images/actions/images/small/flag_" + ls[1] + ".png";
                }
                else
                {
                    if(ls[0] == "greek")
                    {
                        iconName = ":/images/actions/images/small/flag_greek.png";
                    }
                    else
                    if(ls[0] == "armscii8")
                    {
                        iconName = ":/images/actions/images/small/flag_armenia.png";
                    }
                    else
                    if(ls[0] == "hebrew")
                    {
                        iconName = ":/images/actions/images/small/flag_israel.png";
                    }
                    else
                    {
                        iconName = ":/images/actions/images/small/flag_" + ls[1] + ".png";
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
        lwi->setData(Qt::UserRole, QVariant(cps[i]));

        lw->addItem(lwi);
    }

    comboBox->setModel(lw->model());
    comboBox->setView(lw);
    comboBox->setCurrentIndex(-1);
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

void WidgetForSpecificProperties::taylorToSpecificObject(const ObjectWithSpInstances *dest)
{
    // first specific test: see the storage engine type
    SpInstance* storageEngineInstance = dest->getInstanceForSqlRoleUid(m_dbEngine, uidMysqlStorageEngineTable);
    if(storageEngineInstance)
    {
        QString value = storageEngineInstance->get();
        if(value == "MyISAM" || value == "InnoDB")
        {
            for(int i=0; i<m_mappings.size(); i++)
            {
                UidToWidget* uiw = m_mappings.at(i);
                if(uiw->objectRoleUid == uidMysqlIndexType)
                {
                    QComboBox* cmb = qobject_cast<QComboBox*>(uiw->w);
                    if(cmb)
                    {
                        cmb->removeItem(cmb->findText("HASH"));
                    }
                }
            }
        }
    }
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
            if(spInstances.at(i)->getClass()->getClassUid().toString() == uidTrueFalseSp)   // create a check box
            {
                QCheckBox* checkBox = new QCheckBox(page);
                checkBox->setText(spInstances.at(i)->getClass()->getPropertyGuiText());
                checkBox->setToolTip(SpsTooltipProviderForUid::provideTooltipForUid(spInstances.at(i)->getClass()->getSqlRoleUid()));
                formLayout->setWidget(m_rowsForGroups[group], QFormLayout::FieldRole, checkBox);
                UidToWidget* uiw = new UidToWidget();
                uiw->objectUid = spInstances.at(i)->getObjectUid();
                uiw->objectRoleUid = spInstances.at(i)->getClass()->getSqlRoleUid();
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
                    if(spi->getSqlRoleUid() == uidMysqlCodepageTable)
                    {
                        populateCodepageCombo(comboBox, spi->getValues());
                    }
                    else
                    {
                        QStringList values = spi->getValues();
                        for(int j=0; j<values.size(); j++)
                        {
                            comboBox->addItem(values.at(j));
                        }
                    }
                }

                QLabel* label = new QLabel(this);
                label->setText(spInstances.at(i)->getClass()->getPropertyGuiText());
                formLayout->setWidget(m_rowsForGroups[group], QFormLayout::LabelRole, label);
                formLayout->setWidget(m_rowsForGroups[group], QFormLayout::FieldRole, comboBox);
                UidToWidget* uiw = new UidToWidget();
                uiw->objectUid = spInstances.at(i)->getObjectUid();
                uiw->w = comboBox;
                uiw->objectRoleUid = spInstances.at(i)->getClass()->getSqlRoleUid();
                m_mappings.append(uiw);
                m_signalMapper->setMapping(comboBox, spInstances.at(i)->getObjectUid());
                comboBox->setCurrentIndex(comboBox->findText(spInstances.at(i)->get()));
                connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBoxSelected(int))); // must be the last line
            }
        }
    }
    connect(m_signalMapper, SIGNAL(mapped(QString)), this, SLOT(checkBoxToggled(QString)));
}

void WidgetForSpecificProperties::comboBoxSelected(int idx)
{
    QComboBox *combo = qobject_cast<QComboBox *>(sender());
    if(combo != 0)
    {
        QString uid = getObjectUidForWidget(combo);
        if(uid.length())
        {
            SpInstance* spi = m_osp->getInstance(m_dbEngine, uid);
            if(spi)
            {
                QVariant t = combo->itemData(idx);
                if(t.isValid())
                {
                    QString s = t.toString();
                    spi->set(s);
                }
                else
                {
                    spi->set(combo->currentText());
                    // and now: 1. give a feedback to the "parent form" (ie. NewTableForm) to taylor all the WidgetForSpecificProperties
                    // objects to match the newly configured property... such as: user changes storage engine, we allow index type
                    // 2. right now we don't give feedback to the object (table) which is held by the "parent" form...
                    // it would be too complicate and the benefits would be minimal
                }
            }
        }
    }
}

void WidgetForSpecificProperties::checkBoxToggled(QString uid)
{
    QCheckBox* cb = getCheckBoxForObjectUid(uid);
    if(cb)
    {
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
    return "";
}
