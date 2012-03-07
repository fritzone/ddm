#include "WidgetForSpecificProperties.h"
#include "ui_WidgetForSpecificProperties.h"
#include "uids.h"

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

void WidgetForSpecificProperties::feedInSpecificProperties(const QVector<Sp*>& sps, const QString& dbDestinationUid)
{
    for(int i=0; i<sps.size(); i++)
    {
        qDebug() << sps.at(i)->getName();

        if(sps.at(i)->getDbObjectUid() == dbDestinationUid)
        {
            QLabel* label = new QLabel(this);
            label->setText(sps.at(i)->getPropertyGuiText());
            ui->formLayout->setWidget(i, QFormLayout::LabelRole, label);

            if(sps.at(i)->getClassUid().toString() == uidTrueFalseObj)   // create a check box
            {
                QCheckBox* checkBox = new QCheckBox(this);
                qDebug() << "bbb" << sps.at(i)->getName() << "aaaaa ";
                checkBox->setText(sps.at(i)->getName());

                ui->formLayout->setWidget(i, QFormLayout::FieldRole, checkBox);

                UidToWidget* uiw = new UidToWidget();
                uiw->objectUid = sps.at(i)->getObjectUid();
                uiw->w = checkBox;
                m_mappings.append(uiw);
            }
        }
    }
}
