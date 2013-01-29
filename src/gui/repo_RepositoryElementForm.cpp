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
    m_signalMapperForCombosInAttributes = new QSignalMapper(this);
    m_signalMapperForCombosRoleInCollections = new QSignalMapper(this);

    ui->setupUi(this);
    {

    ui->txtEntityName->setText(ent->getName());

    // populate the attributes
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
    // populate the collections

    {
    const QVector<Entity::Collection*>& collections = ent->collections();
    for(int i=0; i<collections.size(); i++)
    {
        ui->tblCollections->insertRow(i);
        QTableWidgetItem *twl = new QTableWidgetItem(collections[i]->name);
        ui->tblCollections->setItem(i, 0, twl);

        // the role of this collection
        QComboBox* cmbCollRole = new QComboBox(0);
        cmbCollRole->setAutoFillBackground(true);
        ui->tblCollections->setCellWidget(i, 1, cmbCollRole);

        QVector<Role*> roles = Repository::instance()->getRoles();
        int idx = -1;
        for(int j=0; j<roles.size(); j++)
        {
            if(roles[j]->getUid() == collections[i]->roleUid) idx = j;
            cmbCollRole->addItem(roles[j]->getName());
        }

        cmbCollRole->setCurrentIndex(idx);
        m_collectionsCombos[collections[i]->name] = cmbCollRole;

        connect(cmbCollRole, SIGNAL(activated(QString)), m_signalMapperForCombosRoleInCollections, SLOT(map()));
        m_signalMapperForCombosRoleInCollections->setMapping(cmbCollRole, collections[i]->name);

        // and the Entity which this collection refers to

        QComboBox* cmbConnEntity = new QComboBox(0);
        cmbConnEntity->setAutoFillBackground(true);
        ui->tblCollections->setCellWidget(i, 1, cmbConnEntity);

        QVector<Role*> roles = Repository::instance()->getRoles();
        int idx = -1;
        for(int j=0; j<roles.size(); j++)
        {
            if(roles[j]->getUid() == collections[i]->roleUid) idx = j;
            cmbConnEntity->addItem(roles[j]->getName());
        }

        cmbConnEntity->setCurrentIndex(idx);
        m_collectionsCombosForEntities[collections[i]->name] = cmbCollRole;

        connect(cmbCollRole, SIGNAL(activated(QString)), m_signalMapperForCombosRoleInCollections, SLOT(map()));
        m_signalMapperForCombosRoleInCollections->setMapping(cmbCollRole, collections[i]->name);
    }

    connect(m_signalMapperForCombosRoleInCollections, SIGNAL(mapped(const QString&)), this, SLOT(onConnectionsRolesSelected(const QString&)));


    // and insert a row above to indicate a new entry
    ui->tblCollections->insertRow(0);
    QTableWidgetItem *twl = new QTableWidgetItem("New Collection element");
    QFont f = twl->font();
    f.setItalic(true);
    twl->setFont(f);
    twl->setForeground(Qt::gray);
    ui->tblCollections->setItem(0, 0, twl);

    QObject::connect(ui->tblCollections, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(newAttribute(QTableWidgetItem*)));

    ui->tblCollections->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignLeft);
    ui->tblCollections->horizontalHeaderItem(1)->setTextAlignment(Qt::AlignLeft);
    ui->tblCollections->horizontalHeaderItem(2)->setTextAlignment(Qt::AlignLeft);
    }
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
        newAttr->builtin = false;
        newAttr->name = itm->text();
        m_entity->addAttribute(newAttr);
    }

}

void RepositoryEntityForm::onConnectionsRolesSelected(const QString & a)
{
    QComboBox* cmbox = m_attributeCombos[a];
    if(!cmbox) return;
    m_entity->setAttribute(a, cmbox->currentText());
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


void RepositoryEntityForm::newCollection(QTableWidgetItem* itm)
{

}
