#include "repo_RepositoryElementForm.h"
#include "ui_repo_RepositoryElementForm.h"
#include "core_Entity.h"
#include "core_Role.h"
#include "core_Repository.h"

#include <QComboBox>

RepositoryEntityForm::RepositoryEntityForm(QWidget *parent, Entity *ent) :
    QWidget(parent),
    ui(new Ui::repo_RepositoryElementForm),
    m_entity(ent)
{
    ui->setupUi(this);

    ui->txtEntityName->setText(ent->getName());
    const QVector<Entity::Attribute*> & attrs = ent->attributes();
    for(int i=0; i<attrs.size(); i++)
    {
        ui->tblAttributes->insertRow(i);
        QTableWidgetItem *twl = new QTableWidgetItem(attrs[i]->name);
        ui->tblAttributes->setItem(i, 0, twl);

        QComboBox* attrRole = new QComboBox(0);
        attrRole->setAutoFillBackground(true);
        ui->tblAttributes->setCellWidget(i, 1, attrRole);
        QVector<Role*> roles = Repository::instance()->getRoles();
        int idx = -1;
        for(int j=0; j<roles.size(); j++)
        {
            if(roles[j]->getUid() == attrs[i]->roleUid) idx = j;
            attrRole->addItem(roles[j]->getName());
        }
        attrRole->setCurrentIndex(idx);
    }

}

RepositoryEntityForm::~RepositoryEntityForm()
{
    delete ui;
}
