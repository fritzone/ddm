#include "WidgetForSpecificProperties.h"
#include "ui_WidgetForSpecificProperties.h"
#include "uids.h"
#include "db_SP.h"
#include "SpInstance.h"
#include "ObjectWithSpInstances.h"

#include <QLabel>
#include <QCheckBox>
#include <QDebug>

WidgetForSpecificProperties::WidgetForSpecificProperties(const DatabaseEngine* dbe, ObjectWithSpInstances* osp, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetForSpecificProperties), m_mappings(),
    m_signalMapper(new QSignalMapper(this)), m_osp(osp), m_dbEngine(dbe)
{
    ui->setupUi(this);
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

void WidgetForSpecificProperties::feedInSpecificProperties(const QVector<SpInstance*>& spInstances, const QString& dbDestinationUid)
{
    for(int i=0; i<spInstances.size(); i++)
    {
        if(spInstances.at(i)->getClass()->getReferredObjectClassUid() == dbDestinationUid)
        {
//            QLabel* label = new QLabel(this);
//            label->setText(sps.at(i)->getClass()->getPropertyGuiText());
//            ui->formLayout->setWidget(i, QFormLayout::LabelRole, label);

            if(spInstances.at(i)->getClass()->getClassUid().toString() == uidTrueFalseObj)   // create a check box
            {
                QCheckBox* checkBox = new QCheckBox(this);
                checkBox->setText(spInstances.at(i)->getClass()->getPropertyGuiText());
                ui->formLayout->setWidget(i, QFormLayout::FieldRole, checkBox);
                UidToWidget* uiw = new UidToWidget();
                uiw->objectUid = spInstances.at(i)->getObjectUid();
                uiw->w = checkBox;
                m_mappings.append(uiw);

                m_signalMapper->setMapping(checkBox, spInstances.at(i)->getObjectUid());
                connect(checkBox, SIGNAL(clicked()), m_signalMapper, SLOT(map()));

                const QString& v = spInstances.at(i)->get();
                qDebug() << v << " XXX " << spInstances.at(i)->getObjectUid() ;
                if(v == "TRUE")
                {
                    checkBox->setChecked(true);
                }
                else
                {
                    checkBox->setChecked(false);
                }
            }
        }
    }

    connect(m_signalMapper, SIGNAL(mapped(QString)), this, SLOT(checkBoxToggled(QString)));
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
        qDebug() << uiw->objectUid << " === " << uid;
        if(uiw->objectUid == uid)
        {
            return dynamic_cast<QCheckBox*>(uiw->w);
        }
    }
    return 0;
}
