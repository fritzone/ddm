#include "NewProjectDialog.h"
#include "ui_NewProjectDialog.h"
#include "db_DatabaseEngine.h"
#include "db_DatabaseEngineManager.h"
#include "gui_HelpWindow.h"

#include <QMessageBox>

NewProjectDialog::NewProjectDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::NewProjectDialog)
{
    m_ui->setupUi(this);
    m_ui->grpOptions->show();
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
    if(m_ui->radioMySql->isChecked()) return strCamelMySql;
    if(m_ui->radioSqlite->isChecked()) return strCamelSqlite;
    if(m_ui->radioCubrid->isChecked()) return strCUBRID;

    return "";
}

DatabaseEngine* NewProjectDialog::getDatabaseEngine() const
{
    DatabaseEngine* engine = DatabaseEngineManager::instance().engine(getTargetDatabase());
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

    m_ui->radioCubrid->show();
    m_ui->radioSqlite->show();
    m_ui->radioMySql->show();

    switch(projectType)
    {
    case 1: // Datamodel Project
        m_ui->grpOptions->show();
        break;
    case 2: // Bind to ... Project
        m_ui->grpOptions->hide();
        break;
    case 3: // Rev. Eng. Project
        m_ui->grpOptions->show();

        // and now remove the unneeded radio buttons from the GUI based on what DB we support
        QStringList supportedDbs = QSqlDatabase::drivers();
        if(!supportedDbs.contains(strQCUBRID)) m_ui->radioCubrid->hide();
        if(!supportedDbs.contains(strQSqlite)) m_ui->radioSqlite->hide();
        if(!supportedDbs.contains(strQMySql)) m_ui->radioMySql->hide();
        break;
    }

    m_ui->txtSolutionName->setFocus();
}

void NewProjectDialog::onChangeDbType()
{
    m_ui->txtSolutionName->setFocus();
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
    if(!m_ui->radioCubrid->isChecked() && !m_ui->radioMySql->isChecked() && !m_ui->radioSqlite->isChecked())
    {
        QMessageBox::critical (this, tr("Error"), tr("Please select a database"), QMessageBox::Ok);
        return;
    }

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
