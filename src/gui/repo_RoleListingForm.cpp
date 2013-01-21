#include "repo_RoleListingForm.h"
#include "ui_repo_RoleListingForm.h"

RoleListingForm::RoleListingForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RoleListingForm)
{
    ui->setupUi(this);
}

RoleListingForm::~RoleListingForm()
{
    delete ui;
}
