#include "ReverseEngineerWizard.h"
#include "ReverseEngineerWizardWelcomeForm.h"
#include "ReverseEngineerWizardDatabasesForm.h"
#include "ReverseEngineerWizardTablesForm.h"
#include "DatabaseEngine.h"

#include <QMessageBox>

ReverseEngineerWizard::ReverseEngineerWizard(DatabaseEngine* engine) : QWizard(), m_engine(engine), m_welcomePage(new ReverseEngineerWizardWelcomeForm),
    m_databasesPage(new ReverseEngineerWizardDatabasesForm), m_tablesPage(new ReverseEngineerWizardTablesForm), m_host(""), m_user(""), m_pass("")
{
    addPage(m_welcomePage);
    addPage(m_databasesPage);
    addPage(m_tablesPage);
    setWindowTitle(QObject::tr("Reverse Engineer a Database"));
}

void ReverseEngineerWizard::gatherConnectionData()
{
    m_host = m_welcomePage->getHost();
    m_user = m_welcomePage->getUser();
    m_pass = m_welcomePage->getPassword();
}

bool ReverseEngineerWizard::connectAndRetrieveDatabases()
{
    QVector<QString> databases = m_engine->getAvailableDatabases(m_host, m_user, m_pass);
    if(databases.size() == 0)
    {
        QMessageBox::critical(this, tr("Error"), QObject::tr("Seems there are no databases at the given location.\n") + m_engine->getLastError(), QMessageBox::Ok)        ;
        return false;
    }

    for(int i=0; i<databases.size(); i++)
    {
        m_databasesPage->addDatabase(databases.at(i));
    }

    return true;
}
