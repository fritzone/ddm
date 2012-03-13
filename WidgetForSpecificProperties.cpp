#include "WidgetForSpecificProperties.h"
#include "ui_WidgetForSpecificProperties.h"
#include "uids.h"
#include "db_SP.h"
#include "SpInstance.h"

#include <QLabel>
#include <QCheckBox>
#include <QDebug>

WidgetForSpecificProperties::WidgetForSpecificProperties(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetForSpecificProperties), m_mappings()
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

void WidgetForSpecificProperties::feedInSpecificProperties(const QVector<SpInstance*>& sps, const QString& dbDestinationUid)
{
    for(int i=0; i<sps.size(); i++)
    {
        if(sps.at(i)->getClass()->getReferredObjectClassUid() == dbDestinationUid)
        {
            QLabel* label = new QLabel(this);
            label->setText(sps.at(i)->getClass()->getPropertyGuiText());
            ui->formLayout->setWidget(i, QFormLayout::LabelRole, label);

            if(sps.at(i)->getClass()->getClassUid().toString() == uidTrueFalseObj)   // create a check box
            {
                QCheckBox* checkBox = new QCheckBox(this);
                checkBox->setText(sps.at(i)->getClass()->getName());
                ui->formLayout->setWidget(i, QFormLayout::FieldRole, checkBox);
                UidToWidget* uiw = new UidToWidget();
                uiw->objectUid = sps.at(i)->getClass()->getObjectUid();
                uiw->w = checkBox;
                m_mappings.append(uiw);
            }
        }
    }
}
