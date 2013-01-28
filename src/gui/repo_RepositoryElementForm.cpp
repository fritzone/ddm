#include "repo_RepositoryElementForm.h"
#include "ui_repo_RepositoryElementForm.h"
#include "core_Entity.h"
#include "core_Role.h"
#include "core_Repository.h"

#include <QTableWidgetItem>
#include <QComboBox>
#include <QSignalMapper>

RepositoryEntityForm::RepositoryEntityForm(QWidget *parent, Entity *ent) :
    QWidget(parent),
    ui(new Ui::repo_RepositoryElementForm),
    m_entity(ent)
{
    ui->setupUi(this);

    m_signalMapperForCombosInAttributes = new QSignalMapper(this);

    ui->txtEntityName->setText(ent->getName());
    const QVector<Entity::Attribute*> & attrs = ent->attributes();
    for(int i=0; i<attrs.size(); i++)
    {
        ui->tblAttributes->insertRow(i);
        QTableWidgetItem *twl = new QTableWidgetItem(attrs[i]->name);
        ui->tblAttributes->setItem(i, 0, twl);

        QComboBox* cmbAttrRole = new QComboBox(0);
        cmbAttrRole->setAutoFillBackground(true);
        ui->tblAttributes->setCellWidget(i, 1, cmbAttrRole);
        QVector<Role*> roles = Repository::instance()->getRoles();
        int idx = -1;
        for(int j=0; j<roles.size(); j++)
        {
            if(roles[j]->getUid() == attrs[i]->roleUid) idx = j;
            cmbAttrRole->addItem(roles[j]->getName());
        }

        if(attrs[i]->builtin)
        {
            QCheckBox*checkBoxItem = new QCheckBox();
            checkBoxItem->setCheckState(Qt::Checked);
            checkBoxItem->setEnabled(false);
            ui->tblAttributes->setCellWidget(i, 2, checkBoxItem);

        }

        cmbAttrRole->setCurrentIndex(idx);
        m_attributeCombos[attrs[i]->name] = cmbAttrRole;

        connect(cmbAttrRole, SIGNAL(activated(QString)), m_signalMapperForCombosInAttributes, SLOT(map()));
        m_signalMapperForCombosInAttributes->setMapping(cmbAttrRole, attrs[i]->name);
    }

    connect(m_signalMapperForCombosInAttributes, SIGNAL(mapped(const QString&)), this, SLOT(onAttributeRolesSelected(const QString&)));

    // and insert a row above to indicate a new entry
    ui->tblAttributes->insertRow(0);
    QTableWidgetItem *twl = new QTableWidgetItem("New Attribute");
    QFont f = twl->font();
    f.setItalic(true);
    twl->setFont(f);
    twl->setForeground(Qt::gray);
    ui->tblAttributes->setItem(0, 0, twl);

    QObject::connect(ui->tblAttributes, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(newAttribute(QTableWidgetItem*)));

    ui->tblAttributes->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignLeft);
    ui->tblAttributes->horizontalHeaderItem(1)->setTextAlignment(Qt::AlignLeft);
    ui->tblAttributes->horizontalHeaderItem(2)->setTextAlignment(Qt::AlignLeft);

}

RepositoryEntityForm::~RepositoryEntityForm()
{
    delete ui;
}

void RepositoryEntityForm::newAttribute(QTableWidgetItem* itm)
{
    if(itm->row() == 0 && itm->column() == 0)
    {
        if(itm->text() == "New Role") return;
        // See that this is already NOT there
        for(int i=1; i<ui->tblAttributes->rowCount(); i++)
        {
            if(itm->text() == ui->tblAttributes->item(i, 0)->text())
            {
                return;
            }
        }

        ui->tblAttributes->blockSignals(true);

        QFont f = itm->font();
        f.setItalic(false);
        itm->setFont(f);
        itm->setForeground(Qt::black);

        ui->tblAttributes->insertRow(0);
        QTableWidgetItem *twl = new QTableWidgetItem("New Role");
        f = twl->font();
        f.setItalic(true);
        twl->setFont(f);
        twl->setForeground(Qt::gray);
        ui->tblAttributes->setItem(0, 0, twl);

        QComboBox* cmbAttrRole = new QComboBox(0);
        cmbAttrRole->setAutoFillBackground(true);
        ui->tblAttributes->setCellWidget(1, 1, cmbAttrRole);
        QVector<Role*> roles = Repository::instance()->getRoles();
        int idx = -1;
        for(int j=0; j<roles.size(); j++)
        {
            cmbAttrRole->addItem(roles[j]->getName());
        }
        cmbAttrRole->setCurrentIndex(idx);
        m_attributeCombos[itm->text()] = cmbAttrRole;

        connect(cmbAttrRole, SIGNAL(activated(QString)), m_signalMapperForCombosInAttributes, SLOT(map()));
        m_signalMapperForCombosInAttributes->setMapping(cmbAttrRole, itm->text());

        ui->tblAttributes->blockSignals(false);

        Entity::Attribute* newAttr = new Entity::Attribute;
        newAttr->name = itm->text();
        m_entity->addAttribute(newAttr);
    }

}

void RepositoryEntityForm::onAttributeRolesSelected(const QString & a)
{
    QComboBox* cmbox = m_attributeCombos[a];
    if(!cmbox) return;
    m_entity->setAttribute(a, cmbox->currentText());
}


void RepositoryEntityForm::onDeleteAttribute()
{
    if(ui->tblAttributes->currentRow() != -1)
    {
        qDebug() << ui->tblAttributes->item(ui->tblAttributes->currentRow(), 0)->text();
    }
}
