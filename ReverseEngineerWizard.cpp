#include "ReverseEngineerWizard.h"
#include "ReverseEngineerWizardWelcomeForm.h"
#include "ReverseEngineerWizardDatabasesForm.h"
#include "ReverseEngineerWizardTablesForm.h"
#include "ReverseEngineerWizardOptionsForm.h"

#include "db_DatabaseEngine.h"

#include <QMessageBox>

ReverseEngineerWizard::ReverseEngineerWizard(DatabaseEngine* engine) : QWizard(), m_engine(engine), m_welcomePage(new ReverseEngineerWizardWelcomeForm),
    m_databasesPage(new ReverseEngineerWizardDatabasesForm), m_tablesPage(new ReverseEngineerWizardTablesForm), m_optionsPage(new ReverseEngineerWizardOptionsForm),
    m_host(""), m_user(""), m_pass(""), m_database("")
{
    addPage(m_welcomePage);
    addPage(m_databasesPage);
    addPage(m_tablesPage);
    addPage(m_optionsPage);
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

    m_databasesPage->clearList();
    for(int i=0; i<databases.size(); i++)
    {
        m_databasesPage->addDatabase(databases.at(i));
    }

    return true;
}


bool ReverseEngineerWizard::selectDatabase()
{
    m_database = m_databasesPage->getSelectedDatabase();
    if(m_database.length() > 0) return true;
    return false;
}

bool ReverseEngineerWizard::connectAndRetrieveTables()
{
    QVector<QString> tables = m_engine->getAvailableTables(m_host, m_user, m_pass, m_database);
    if(tables.size() == 0)
    {
        QMessageBox::critical(this, tr("Error"), QObject::tr("Seems there are no tables in the given database.\n") + m_engine->getLastError(), QMessageBox::Ok)        ;
        return false;
    }

    m_tablesPage->clearList();
    for(int i=0; i<tables.size(); i++)
    {
        m_tablesPage->addTable(tables.at(i));
    }

    return true;
}

QVector<QString> ReverseEngineerWizard::getTablesToReverse()
{
    return m_tablesPage->getSelectedTables();
}

bool ReverseEngineerWizard::createDataTypesForColumns()
{
    return m_optionsPage->createDataTypesForColumns();
}
