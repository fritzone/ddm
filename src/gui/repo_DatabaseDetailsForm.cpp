#include "repo_DatabaseDetailsForm.h"
#include "ui_repo_DatabaseDetailsForm.h"

repo_DatabaseDetailsForm::repo_DatabaseDetailsForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::repo_DatabaseDetailsForm)
{
    ui->setupUi(this);
}

repo_DatabaseDetailsForm::~repo_DatabaseDetailsForm()
{
    delete ui;
}
