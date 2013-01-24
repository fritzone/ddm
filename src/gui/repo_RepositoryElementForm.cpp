#include "repo_RepositoryElementForm.h"
#include "ui_repo_RepositoryElementForm.h"

repo_RepositoryElementForm::repo_RepositoryElementForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::repo_RepositoryElementForm)
{
    ui->setupUi(this);
}

repo_RepositoryElementForm::~repo_RepositoryElementForm()
{
    delete ui;
}
