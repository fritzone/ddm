#include "ProjectDetailsForm.h"
#include "ui_ProjectDetailsForm.h"
#include "Project.h"

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

void ProjectDetailsForm::setProject(Project *prj)
{
    m_project = prj;

    ui->txtProjectName->setText(m_project->getName());
    ui->textEdit->setText(m_project->getDescription());
}

void ProjectDetailsForm::onBtnUpdate()
{
    m_project->setName(ui->txtProjectName->text());
    m_project->setDescription(ui->textEdit->toPlainText());
}
