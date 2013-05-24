#include "reveng_ReverseEngineerWizard.h"
#include "reveng_ReverseEngineerWizardWelcomeForm.h"
#include "reveng_ReverseEngineerWizardDatabasesForm.h"
#include "reveng_ReverseEngineerWizardObjectListForm.h"
#include "reveng_ReverseEngineerWizardOptionsForm.h"
#include "Connection.h"
#include "db_DatabaseEngine.h"
#include "MySqlConnection.h"
#include "SqliteConnection.h"

#include <QMessageBox>

ReverseEngineerWizard::ReverseEngineerWizard(DatabaseEngine* engine) : QWizard(), m_engine(engine),
    m_welcomePage(new ReverseEngineerWizardWelcomeForm),
    m_databasesPage(new ReverseEngineerWizardDatabasesForm),
    m_tablesPage(new ReverseEngineerWizardObjectListForm),
    m_viewsPage(new ReverseEngineerWizardObjectListForm(0, ReverseEngineerWizardObjectListForm::REVERSE_ENGINEER_VIEWS)),
    m_proceduresPage(new ReverseEngineerWizardObjectListForm(0, ReverseEngineerWizardObjectListForm::REVERSE_ENGINEER_PROCS)),
    m_functionsPage(new ReverseEngineerWizardObjectListForm(0, ReverseEngineerWizardObjectListForm::REVERSE_ENGINEER_FUNCS)),
    m_triggersPage(new ReverseEngineerWizardObjectListForm(0, ReverseEngineerWizardObjectListForm::REVERSE_ENGINEER_TRIGGERS)),
    m_optionsPage(new ReverseEngineerWizardOptionsForm),
    m_host(""), m_user(""), m_pass(""), m_database(""), m_port(0), m_sqliteFile(), m_sqliteVersion(3)
{
    if(getDbTypeName() == "MYSQL")
    {
        m_welcomePage->setMySqlMode();
    }
    else
    {
        m_welcomePage->setSqliteMode();
    }

    // the wizard is highly fragmented because the SQLITE DB does not support procedures and functions

    addPage(m_welcomePage);

    // add the databases page only if it works as a MySQL wizard
    if(getDbTypeName() == "MYSQL")
    {
        addPage(m_databasesPage);
    }

    addPage(m_tablesPage);
    addPage(m_viewsPage);
    // add the procedures and functions only if it acts like a mysql wizard
    if(getDbTypeName() == "MYSQL")
    {
        addPage(m_proceduresPage);
        addPage(m_functionsPage);
    }
    addPage(m_triggersPage);
    addPage(m_optionsPage);
    setWindowTitle(QObject::tr("Reverse Engineer a Database"));
}

void ReverseEngineerWizard::gatherConnectionData()
{
    if(getDbTypeName() == "MYSQL")
    {
        m_host = m_welcomePage->getHost();
        m_user = m_welcomePage->getUser();
        m_pass = m_welcomePage->getPassword();
        m_port = m_welcomePage->getPort();
    }
    else
    {
        m_sqliteFile = m_welcomePage->getSqliteFile();
        m_sqliteVersion = m_welcomePage->getSqliteVersion();
    }
}

bool ReverseEngineerWizard::connectAndRetrieveDatabases()
{
    QStringList databases = m_engine->getAvailableDatabases(m_host, m_user, m_pass, m_port);
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
    if(getDbTypeName() == "MYSQL")
    {
        m_database = m_databasesPage->getSelectedDatabase();
        if(m_database.length() > 0) return true;
    }
    else
    {
        if(m_sqliteFile.length() > 0) return true;
    }
    return false;
}

bool ReverseEngineerWizard::connectAndRetrieveViews()
{
    Connection* c = 0;

    if(getDbTypeName() == "MYSQL")
    {
        c = new MySqlConnection("temp", m_host, m_user, m_pass, m_database, false, false, m_port);
    }
    else
    {
        c = new SqliteConnection("temp", m_sqliteFile, false, m_sqliteVersion);
    }

    if(!c) return false;

    QStringList views = m_engine->getAvailableViews(c);
    m_viewsPage->clearList();
    for(int i=0; i<views.size(); i++)
    {
        m_viewsPage->addObject(views.at(i));
    }
    delete c;
    return true;
}

bool ReverseEngineerWizard::connectAndRetrieveFunctions()
{
    Connection* c = 0;

    if(getDbTypeName() == "MYSQL")
    {
        c = new MySqlConnection("temp", m_host, m_user, m_pass, m_database, false, false, m_port);
    }
    else
    {
        c = new SqliteConnection("temp", m_sqliteFile, false, m_sqliteVersion);
    }

    if(!c) return false;

    QStringList funcs = m_engine->getAvailableStoredFunctions(c);
    m_functionsPage->clearList();
    for(int i=0; i<funcs.size(); i++)
    {
        m_functionsPage->addObject(funcs.at(i));
    }
    delete c;
    return true;
}

bool ReverseEngineerWizard::connectAndRetrieveProcedures()
{
    Connection* c = 0;

    if(getDbTypeName() == "MYSQL")
    {
        c = new MySqlConnection("temp", m_host, m_user, m_pass, m_database, false, false, m_port);
    }
    else
    {
        c = new SqliteConnection("temp", m_sqliteFile, false, m_sqliteVersion);
    }

    if(!c) return false;

    QStringList procs = m_engine->getAvailableStoredProcedures(c);
    m_proceduresPage->clearList();
    for(int i=0; i<procs.size(); i++)
    {
        m_proceduresPage->addObject(procs.at(i));
    }
    delete c;
    return true;
}

bool ReverseEngineerWizard::connectAndRetrieveTriggers()
{
    Connection* c = 0;

    if(getDbTypeName() == "MYSQL")
    {
        c = new MySqlConnection("temp", m_host, m_user, m_pass, m_database, false, false, m_port);
    }
    else
    {
        c = new SqliteConnection("temp", m_sqliteFile, false, m_sqliteVersion);
    }

    if(!c) return false;

    QStringList triggers = m_engine->getAvailableTriggers(c);
    m_triggersPage->clearList();
    for(int i=0; i<triggers.size(); i++)
    {
        m_triggersPage->addObject(triggers.at(i));
    }
    delete c;
    return true;
}

bool ReverseEngineerWizard::connectAndRetrieveTables()
{
    Connection* c = 0;

    if(getDbTypeName() == "MYSQL")
    {
        c = new MySqlConnection("temp", m_host, m_user, m_pass, m_database, false, false, m_port);
    }
    else
    {
        c = new SqliteConnection("temp", m_sqliteFile, false, m_sqliteVersion);
    }

    if(!c) return false;

    QStringList tables = m_engine->getAvailableTables(c);
    m_tablesPage->clearList();
    for(int i=0; i<tables.size(); i++)
    {
        m_tablesPage->addObject(tables.at(i));
    }

    delete c;
    return true;
}

QStringList ReverseEngineerWizard::getTablesToReverse()
{
    return m_tablesPage->getSelectedItems();
}

QStringList ReverseEngineerWizard::getViewsToReverse()
{
    return m_viewsPage->getSelectedItems();
}

QStringList ReverseEngineerWizard::getProceduresToReverse()
{
    return m_proceduresPage->getSelectedItems();
}

QStringList ReverseEngineerWizard::getFunctionsToReverse()
{
    return m_functionsPage->getSelectedItems();
}

QStringList ReverseEngineerWizard::getTriggersToReverse()
{
    return m_triggersPage->getSelectedItems();
}

bool ReverseEngineerWizard::createDataTypesForColumns()
{
    return m_optionsPage->createDataTypesForColumns();
}

QString ReverseEngineerWizard::getDbTypeName() const
{
    return m_engine->getDatabaseEngineName().toUpper();
}
