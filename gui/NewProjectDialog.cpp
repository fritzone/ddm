#include "NewProjectDialog.h"
#include "ui_NewProjectDialog.h"
#include "db_DatabaseEngine.h"
#include "gui_HelpWindow.h"

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
    m_ui->txtSolutionName->setFocus();
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
    DatabaseEngine* engine = DatabaseEngine::provideEngineFor(getTargetDatabase());
    return engine;
}

bool NewProjectDialog::enableOOPFeatures() const
{
    return m_ui->chkAllowOOPModel->isChecked();
}

int NewProjectDialog::getProjectType() const
{
    return (m_ui->listWidget->currentRow() + 1);
}

void NewProjectDialog::onChangeProjectType(int)
{
    int projectType = m_ui->listWidget->currentRow() + 1;
    Q_UNUSED(projectType);
}

void NewProjectDialog::onHelp()
{
    HelpWindow* hw = HelpWindow::instance();
    hw->showHelp(QString("/doc/newp.html"));
    hw->show();
    setModal(false);
}

bool NewProjectDialog::inheritDefaultDatatypes() const
{
    return m_ui->chkInheritDefaultDatatypes->isChecked();
}

void NewProjectDialog::onReject()
{
    QDialog::reject();
}

void NewProjectDialog::onAccept()
{
    if(m_ui->txtProjectName->text().size() > 0 && m_ui->txtSolutionName->text().size() > 0)
    {
        projectName = m_ui->txtProjectName->text();
        solutionName = m_ui->txtSolutionName->text();
        QDialog::accept();
    }
    else
    {
        QMessageBox::critical (this, tr("Error"), tr("Please specify a valid project and solution name"), QMessageBox::Ok);
    }
}