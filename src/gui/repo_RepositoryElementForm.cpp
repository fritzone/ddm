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
    m_signalMapperForCombosRoleInCollections = new QSignalMapper(this);
    m_signalMapperForCombosEntityInCollections = new QSignalMapper(this);
    m_signalMapperForCombosRoleInReferences = new QSignalMapper(this);
    m_signalMapperForCombosEntityInReferences = new QSignalMapper(this);

    ui->txtEntityName->setText(ent->getName());

    populateAttributes();
    populateCollections();
    populateReferences();

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
    QComboBox* cmbox = m_collectionsCombosForRoles[a];
    if(!cmbox) return;
    //m_entity->setAttribute(a, cmbox->currentText());
}

void RepositoryEntityForm::onConnectionsEntitySelected(const QString & a)
{
    QComboBox* cmbox = m_collectionsCombosForEntities[a];
    if(!cmbox) return;
    //m_entity->setAttribute(a, cmbox->currentText());
}

void RepositoryEntityForm::onReferencesRolesSelected(const QString & a)
{
    QComboBox* cmbox = m_referenceCombosForRole[a];
    if(!cmbox) return;
    //m_entity->setAttribute(a, cmbox->currentText());
}

void RepositoryEntityForm::onReferencesEntitySelected(const QString & a)
{
    QComboBox* cmbox = m_referenceCombosForEntities[a];
    if(!cmbox) return;
    //m_entity->setAttribute(a, cmbox->currentText());
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

void RepositoryEntityForm::newReference(QTableWidgetItem* itm)
{

}

void RepositoryEntityForm::populateCollections()
{
    const QVector<Entity::Collection*>& collections = m_entity->collections();
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
        m_collectionsCombosForRoles[collections[i]->name] = cmbCollRole;

        connect(cmbCollRole, SIGNAL(activated(QString)), m_signalMapperForCombosRoleInCollections, SLOT(map()));
        m_signalMapperForCombosRoleInCollections->setMapping(cmbCollRole, collections[i]->name);

        // and the Entity which this collection refers to

        QComboBox* cmbConnEntity = new QComboBox(0);
        cmbConnEntity->setAutoFillBackground(true);
        ui->tblCollections->setCellWidget(i, 2, cmbConnEntity);

        QVector<Entity*> entities = Repository::instance()->getEntities();
        idx = -1;
        for(int j=0; j<entities .size(); j++)
        {
            if(entities[j]->getTargetClassUid() == collections[i]->targetUid) idx = j;
            cmbConnEntity->addItem(entities [j]->getName());
        }

        cmbConnEntity->setCurrentIndex(idx);
        m_collectionsCombosForEntities[collections[i]->name] = cmbCollRole;

        connect(cmbCollRole, SIGNAL(activated(QString)), m_signalMapperForCombosEntityInCollections, SLOT(map()));
        m_signalMapperForCombosEntityInCollections->setMapping(cmbCollRole, collections[i]->name);

        if(collections[i]->builtin)
        {
            QCheckBox*checkBoxItem = new QCheckBox();
            checkBoxItem->setCheckState(Qt::Checked);
            checkBoxItem->setEnabled(false);
            ui->tblCollections->setCellWidget(i, 3, checkBoxItem);
        }

    }

    connect(m_signalMapperForCombosRoleInCollections, SIGNAL(mapped(const QString&)), this, SLOT(onConnectionsRolesSelected(const QString&)));
    connect(m_signalMapperForCombosEntityInCollections, SIGNAL(mapped(const QString&)), this, SLOT(onConnectionsEntitySelected(const QString&)));

    // and insert a row above to indicate a new entry
    ui->tblCollections->insertRow(0);
    QTableWidgetItem *twl = new QTableWidgetItem("New Collection element");
    QFont f = twl->font();
    f.setItalic(true);
    twl->setFont(f);
    twl->setForeground(Qt::gray);
    ui->tblCollections->setItem(0, 0, twl);

    QObject::connect(ui->tblCollections, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(newCollection(QTableWidgetItem*)));

    ui->tblCollections->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignLeft);
    ui->tblCollections->horizontalHeaderItem(1)->setTextAlignment(Qt::AlignLeft);
    ui->tblCollections->horizontalHeaderItem(2)->setTextAlignment(Qt::AlignLeft);
}

void RepositoryEntityForm::populateAttributes()
{
    const QVector<Entity::Attribute*> & attrs = m_entity->attributes();
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

void RepositoryEntityForm::populateReferences()
{
    const QVector<Entity::Reference*>& references= m_entity->references();
    for(int i=0; i<references.size(); i++)
    {
        ui->tblReferences->insertRow(i);
        QTableWidgetItem *twl = new QTableWidgetItem(references[i]->name);
        ui->tblReferences->setItem(i, 0, twl);

        // the role of this collection
        QComboBox* cmbCollRole = new QComboBox(0);
        cmbCollRole->setAutoFillBackground(true);
        ui->tblReferences->setCellWidget(i, 1, cmbCollRole);

        QVector<Role*> roles = Repository::instance()->getRoles();
        int idx = -1;
        for(int j=0; j<roles.size(); j++)
        {
            if(roles[j]->getUid() == references[i]->srcRoleUid) idx = j;
            cmbCollRole->addItem(roles[j]->getName());
        }

        cmbCollRole->setCurrentIndex(idx);
        m_collectionsCombosForRoles[references[i]->name] = cmbCollRole;

        connect(cmbCollRole, SIGNAL(activated(QString)), m_signalMapperForCombosRoleInReferences, SLOT(map()));
        m_signalMapperForCombosRoleInReferences->setMapping(cmbCollRole, references[i]->name);

        // and the Entity which this collection refers to

        QComboBox* cmbConnEntity = new QComboBox(0);
        cmbConnEntity->setAutoFillBackground(true);
        ui->tblReferences->setCellWidget(i, 2, cmbConnEntity);

        QVector<Entity*> entities = Repository::instance()->getEntities();
        idx = -1;
        for(int j=0; j<entities .size(); j++)
        {
            if(entities[j]->getTargetClassUid() == references[i]->srcEntityUid) idx = j;
            cmbConnEntity->addItem(entities [j]->getName());
        }

        cmbConnEntity->setCurrentIndex(idx);
        m_collectionsCombosForEntities[references[i]->name] = cmbCollRole;

        connect(cmbCollRole, SIGNAL(activated(QString)), m_signalMapperForCombosEntityInReferences, SLOT(map()));
        m_signalMapperForCombosEntityInReferences->setMapping(cmbCollRole, references[i]->name);

        if(references[i]->builtin)
        {
            QCheckBox*checkBoxItem = new QCheckBox();
            checkBoxItem->setCheckState(Qt::Checked);
            checkBoxItem->setEnabled(false);
            ui->tblReferences->setCellWidget(i, 3, checkBoxItem);
        }
    }

    connect(m_signalMapperForCombosRoleInReferences, SIGNAL(mapped(const QString&)), this, SLOT(onConnectionsRolesSelected(const QString&)));
    connect(m_signalMapperForCombosEntityInReferences, SIGNAL(mapped(const QString&)), this, SLOT(onConnectionsEntitySelected(const QString&)));

    // and insert a row above to indicate a new entry
    ui->tblReferences->insertRow(0);
    QTableWidgetItem *twl = new QTableWidgetItem("New Collection element");
    QFont f = twl->font();
    f.setItalic(true);
    twl->setFont(f);
    twl->setForeground(Qt::gray);
    ui->tblReferences->setItem(0, 0, twl);

    QObject::connect(ui->tblReferences, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(newReference(QTableWidgetItem*)));

    ui->tblReferences->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignLeft);
    ui->tblReferences->horizontalHeaderItem(1)->setTextAlignment(Qt::AlignLeft);
    ui->tblReferences->horizontalHeaderItem(2)->setTextAlignment(Qt::AlignLeft);
}
