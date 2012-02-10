#include "ReverseEngineerWizard.h"
#include "ReverseEngineerWizardWelcomeForm.h"
#include "ReverseEngineerWizardDatabasesForm.h"
#include "ReverseEngineerWizardObjectListForm.h"
#include "ReverseEngineerWizardOptionsForm.h"
#include "core_Connection.h"
#include "db_DatabaseEngine.h"

#include <QMessageBox>

ReverseEngineerWizard::ReverseEngineerWizard(DatabaseEngine* engine) : QWizard(), m_engine(engine), m_welcomePage(new ReverseEngineerWizardWelcomeForm),
    m_databasesPage(new ReverseEngineerWizardDatabasesForm), m_tablesPage(new ReverseEngineerWizardObjectListForm),
    m_viewsPage(new ReverseEngineerWizardObjectListForm(0, ReverseEngineerWizardObjectListForm::REVERSE_ENGINEER_VIEWS)),
    m_proceduresPage(new ReverseEngineerWizardObjectListForm(0, ReverseEngineerWizardObjectListForm::REVERSE_ENGINEER_PROCS)),
    m_functionsPage(new ReverseEngineerWizardObjectListForm(0, ReverseEngineerWizardObjectListForm::REVERSE_ENGINEER_FUNCS)),
    m_triggersPage(new ReverseEngineerWizardObjectListForm(0, ReverseEngineerWizardObjectListForm::REVERSE_ENGINEER_TRIGGERS)),
    m_optionsPage(new ReverseEngineerWizardOptionsForm),
    m_host(""), m_user(""), m_pass(""), m_database("")
{
    addPage(m_welcomePage);
    addPage(m_databasesPage);
    addPage(m_tablesPage);
    addPage(m_viewsPage);
    addPage(m_proceduresPage);
    addPage(m_functionsPage);
    addPage(m_triggersPage);
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
    QStringList databases = m_engine->getAvailableDatabases(m_host, m_user, m_pass);
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

bool ReverseEngineerWizard::connectAndRetrieveViews()
{
    Connection* c = new Connection("temp", m_host, m_user, m_pass, m_database, false, false);

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
    Connection* c = new Connection("temp", m_host, m_user, m_pass, m_database, false, false);

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
    Connection* c = new Connection("temp", m_host, m_user, m_pass, m_database, false, false);

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
    Connection* c = new Connection("temp", m_host, m_user, m_pass, m_database, false, false);

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
    Connection* c = new Connection("temp", m_host, m_user, m_pass, m_database, false, false);
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

