#include "ProjectDetailsForm.h"
#include "ui_ProjectDetailsForm.h"
#include "Project.h"
#include "db_DatabaseEngine.h"
#include "ddm_strings.h"
#include "IconFactory.h"

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

void ProjectDetailsForm::setProject(Project *prj, const QString & onDisk)
{
    m_project = prj;

    ui->txtProjectName->setText(m_project->getName());
    ui->textEdit->setText(m_project->getDescription());
    ui->checkBox->setCheckState(prj->oopProject()?Qt::Checked:Qt::Unchecked);
    ui->txtLocationOnDisk->setText(onDisk);

    ui->lblDbName->setText(prj->getEngine()->getDatabaseEngineName());
    ui->lblIcon->setPixmap(IconFactory::getIconForDatabase(prj->getEngine()->getDatabaseEngineName()).pixmap(16, 16));
}

void ProjectDetailsForm::onBtnUpdate()
{
    m_project->setName(ui->txtProjectName->text());
    m_project->setDescription(ui->textEdit->toPlainText());
}

void ProjectDetailsForm::onOopCheckChange(int)
{
    ui->checkBox->setCheckState(m_project->oopProject()?Qt::Checked:Qt::Unchecked);
}

void ProjectDetailsForm::onNameChanged(QString a)
{
    m_project->setName(ui->txtProjectName->text());
    m_project->setDisplayText(a);
}

void ProjectDetailsForm::decriptionChanged()
{
    m_project->setDescription(ui->textEdit->toPlainText());
}
