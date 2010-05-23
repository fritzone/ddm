#include "ProjectDetailsForm.h"
#include "ui_ProjectDetailsForm.h"

ProjectDetailsForm::ProjectDetailsForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProjectDetailsForm)
{
    ui->setupUi(this);
}

ProjectDetailsForm::~ProjectDetailsForm()
{
    delete ui;
}

void ProjectDetailsForm::changeEvent(QEvent *e)
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
