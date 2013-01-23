#include "repo_RoleListingForm.h"
#include "ui_repo_RoleListingForm.h"
#include "core_Repository.h"
#include "core_Role.h"
#include <QTableWidgetItem>

RoleListingForm::RoleListingForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RoleListingForm)
{
    ui->setupUi(this);

    ui->tblRoles->clearContents();


    const QVector<Role*> &roles = Repository::instance()->getRoles();
    for(int i=0; i<roles.size(); i++)
    {
        ui->tblRoles->insertRow(i);
        QTableWidgetItem *twlName = new QTableWidgetItem(roles.at(i)->getName());
        ui->tblRoles->setItem(i, 0, twlName);
        QTableWidgetItem *twlDesc = new QTableWidgetItem(roles.at(i)->getDescription());
        ui->tblRoles->setItem(i, 1, twlDesc);
    }

    ui->tblRoles->resizeColumnsToContents();

    ui->tblRoles->insertRow(0);
    QTableWidgetItem *twl = new QTableWidgetItem("New Role");
    QFont f = twl->font();
    f.setItalic(true);
    twl->setFont(f);
    twl->setForeground(Qt::gray);
    ui->tblRoles->setItem(0, 0, twl);

    QObject::connect(ui->tblRoles, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(newRole(QTableWidgetItem*)));

}

RoleListingForm::~RoleListingForm()
{
    delete ui;
}

void RoleListingForm::newRole(QTableWidgetItem *itm)
{

    if(itm->row() == 0 && itm->column() == 0)
    {
        if(itm->text() == "New Role") return;
        QFont f = itm->font();
        f.setItalic(false);
        itm->setFont(f);
        itm->setForeground(Qt::black);


        ui->tblRoles->blockSignals(true);
        ui->tblRoles->insertRow(0);
        QTableWidgetItem *twl = new QTableWidgetItem("New Role");
        f = twl->font();
        f.setItalic(true);
        twl->setFont(f);
        twl->setForeground(Qt::gray);
        ui->tblRoles->setItem(0, 0, twl);
        ui->tblRoles->blockSignals(false);
    }
}
