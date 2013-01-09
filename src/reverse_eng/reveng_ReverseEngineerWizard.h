#ifndef REVERSEENGINEERWIZARD_H
#define REVERSEENGINEERWIZARD_H

#include <QWizard>

class DatabaseEngine;
class ReverseEngineerWizardWelcomeForm;
class ReverseEngineerWizardDatabasesForm;
class ReverseEngineerWizardObjectListForm;
class ReverseEngineerWizardOptionsForm;

class ReverseEngineerWizard : public QWizard
{
public:

    ReverseEngineerWizard(DatabaseEngine* engine);

    void gatherConnectionData();
    bool connectAndRetrieveDatabases();
    bool selectDatabase();
    bool connectAndRetrieveTables();
    bool connectAndRetrieveViews();
    bool connectAndRetrieveProcedures();
    bool connectAndRetrieveFunctions();
    bool connectAndRetrieveTriggers();

    QStringList getTablesToReverse();
    QStringList getViewsToReverse();
    QStringList getProceduresToReverse();
    QStringList getFunctionsToReverse();
    QStringList getTriggersToReverse();

    QString getHost() {return m_host; }
    QString getUser() {return m_user; }
    QString getPasword() {return m_pass; }
    QString getDatabase() {return m_database; }
    int getPort() {return m_port; }

    bool createDataTypesForColumns();

private:

    DatabaseEngine* m_engine;

    ReverseEngineerWizardWelcomeForm* m_welcomePage;
    ReverseEngineerWizardDatabasesForm* m_databasesPage;
    ReverseEngineerWizardObjectListForm* m_tablesPage;
    ReverseEngineerWizardObjectListForm* m_viewsPage;
    ReverseEngineerWizardObjectListForm* m_proceduresPage;
    ReverseEngineerWizardObjectListForm* m_functionsPage;
    ReverseEngineerWizardObjectListForm* m_triggersPage;
    ReverseEngineerWizardOptionsForm* m_optionsPage;

    QString m_host;
    QString m_user;
    QString m_pass;
    QString m_database;
    int m_port;
};

#endif // REVERSEENGINEERWIZARD_H
