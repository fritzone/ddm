#include "newprojectdialog.h"
#include "ui_newprojectdialog.h"
#include "DatabaseEngine.h"

#include <QMessageBox>

NewProjectDialog::NewProjectDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::NewProjectDialog)
{
    m_ui->setupUi(this);

}

NewProjectDialog::~NewProjectDialog()
{
    delete m_ui;
}

void NewProjectDialog::focusOnEditField() const
{
    m_ui->txtProjectName->setFocus();
}

void NewProjectDialog::accept()
{
    if(m_ui->txtProjectName->text().size() > 0)
    {
	projectName = m_ui->txtProjectName->text();
	QDialog::accept();
    }
    else
    {
	QMessageBox::critical (this, tr("Error"), tr("Please specify a valid project name"), QMessageBox::Ok);
    }
}

void NewProjectDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
	m_ui->retranslateUi(this);
	break;
    default:
	break;
    }
}

QString NewProjectDialog::getTargetDatabase() const
{
    return m_ui->cmbDatabase->currentText();
}

DatabaseEngine* NewProjectDialog::getDatabaseEngine() const
{
    DatabaseEngine* engine = new DatabaseEngine(getTargetDatabase());
    return engine;
}