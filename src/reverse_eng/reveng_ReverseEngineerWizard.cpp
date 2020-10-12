#include "reveng_ReverseEngineerWizard.h"
#include "reveng_ReverseEngineerWizardWelcomeForm.h"
#include "reveng_ReverseEngineerWizardDatabasesForm.h"
#include "reveng_ReverseEngineerWizardObjectListForm.h"
#include "reveng_ReverseEngineerWizardOptionsForm.h"
#include "Connection.h"
#include "db_DatabaseEngine.h"
#include "MySqlConnection.h"
#include "SqliteConnection.h"
#include "conn_Postgres.h"
#include "conn_CUBRID.h"
#include "ddm_strings.h"

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
    QString dbName = getDbTypeName();

    if(dbName == strMySql)
    {
        m_welcomePage->setMySqlMode();
    }
    else
    if(dbName == strCUBRID)
    {
        m_welcomePage->setCubridMode();
    }
    else
    if(dbName == strSqlite)
    {
        m_welcomePage->setSqliteMode();
    }
    else
    if(dbName.toUpper() == strPostgres.toUpper())
    {
        m_welcomePage->setPostgresqlMode();
    }

    // the wizard is highly fragmented because the SQLITE DB does not support procedures and functions
    // the CUBRID DB does not support database selection, etc...

    addPage(m_welcomePage);

    // add the databases page only if it works as a MySQL wizard
    if(dbName == strMySql || dbName.toUpper() == strPostgres.toUpper())
    {
        addPage(m_databasesPage);
    }

    addPage(m_tablesPage);
    addPage(m_viewsPage);
    // add the procedures and functions only if it acts like a mysql wizard
    if(dbName == strMySql || dbName == strCUBRID || dbName.toUpper() == strPostgres.toUpper())
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
    QString dbName = getDbTypeName();
    if(dbName == strMySql || dbName == strCUBRID || dbName.toUpper() == strPostgres.toUpper())
    {
        m_host = m_welcomePage->getHost();
        m_user = m_welcomePage->getUser();
        m_pass = m_welcomePage->getPassword();
        m_port = m_welcomePage->getPort();

        if(dbName == strCUBRID)
        {
            m_database = m_welcomePage->getDatabase();
        }
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
        QMessageBox::critical(this, tr("Error"), QObject::tr("Seems there are no databases at the given location.<p>") + m_engine->getLastError(), QMessageBox::Ok);
        return false;
    }

    m_databasesPage->clearList();
    for(int i=0; i<databases.size(); i++)
    {
        m_databasesPage->addDatabase(databases.at(i));
    }

    return true;
}

bool ReverseEngineerWizard::databaseWasSelected()
{
    bool selected = false;
    QString dbName = getDbTypeName();
    if(dbName == strMySql || dbName.toUpper() == strPostgres.toUpper())
    {
        m_database = m_databasesPage->getSelectedDatabase();
        if(m_database.length() > 0)
        {
            selected = true;
        }
    }
    else
    if(dbName == strCUBRID)
    {
        m_database = m_welcomePage->getDatabase();
        if(m_database.length() > 0)
        {
            selected = true;
        }
    }
    else
    if(dbName == strSqlite)
    {
        if(m_sqliteFile.length() > 0)
        {
            selected = true;
        }
    }

    if(!selected)
    {
        return false;
    }

    // now see if we can connect or not to it
    Connection* c = getConnectionforDb();
    if(!c)
    {
        qDebug() << "get connection connect";
        return false;
    }

    if(!c->tryConnect())
    {
        qDebug() << "cannot connect";
        return false;
    }


    return true;
}

bool ReverseEngineerWizard::connectAndRetrieveViews()
{
    Connection* c = getConnectionforDb();
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
    Connection* c = getConnectionforDb();
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
    Connection* c = getConnectionforDb();
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
    Connection* c = getConnectionforDb();
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

bool ReverseEngineerWizard::gatherConnectionDataAndTablesStep()
{
    gatherConnectionData();
    if(!databaseWasSelected()) // did he?
    {
        QMessageBox::critical(this, tr("Error"), tr("Please select an existing database"), QMessageBox::Ok);
        back();
        return false;
    }
    connectAndRetrieveTables();
    return true;
}

bool ReverseEngineerWizard::gatherConnectionDataAndCheckDatabase()
{
    gatherConnectionData();
    if(!connectAndRetrieveDatabases())  //failed?... don't go forward, there won't be databases
    {
        back();
        return false;
    }
    return true;
}

bool ReverseEngineerWizard::checkDatabaseAndCollectTables()
{
    if(!databaseWasSelected()) // did he?
    {
        QMessageBox::critical(this, tr("Error"), tr("Please select a valid database"), QMessageBox::Ok);
        back();
        return false;
    }
    connectAndRetrieveTables();
    return true;
}

bool ReverseEngineerWizard::connectAndRetrieveTables()
{
    Connection *c = getConnectionforDb();
    if(!c)
    {
        QMessageBox::critical(this, tr("Error"), tr("Can't get a DB connection"), QMessageBox::Ok);
        return false;
    }

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

Connection *ReverseEngineerWizard::getConnectionforDb() const
{
    Connection* c = 0;
    QString dbName = getDbTypeName();
    QString temp("temp");
    if(dbName == strMySql)
    {
        c = new MySqlConnection(temp, m_host, m_user, m_pass, m_database, false, false, m_port);
    }
    else
    if(dbName == strCUBRID)
    {
        c = new CUBRIDConnection(temp, m_host, m_user, m_pass, m_database, false, false, m_port);
    }
    else
    if(dbName == strSqlite)
    {
        c = new SqliteConnection(temp, m_sqliteFile, false, m_sqliteVersion);
    }
    else
    if(dbName.toUpper() == strPostgres.toUpper())
    {
        c = new PostgresConnection(temp, m_host, m_user, m_pass, m_database, false, false, m_port);
    }
    return c;
}
