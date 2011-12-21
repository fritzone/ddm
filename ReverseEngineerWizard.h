#ifndef REVERSEENGINEERWIZARD_H
#define REVERSEENGINEERWIZARD_H

#include <QWizard>

class DatabaseEngine;
class ReverseEngineerWizardWelcomeForm;
class ReverseEngineerWizardDatabasesForm;
class ReverseEngineerWizardTablesForm;
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

    QStringList getTablesToReverse();
    QStringList getViewsToReverse();

    QString getHost() {return m_host; }
    QString getUser() {return m_user; }
    QString getPasword() {return m_pass; }
    QString getDatabase() {return m_database; }
    bool createDataTypesForColumns();

private:

    DatabaseEngine* m_engine;

    ReverseEngineerWizardWelcomeForm* m_welcomePage;
    ReverseEngineerWizardDatabasesForm* m_databasesPage;
    ReverseEngineerWizardTablesForm* m_tablesPage;
    ReverseEngineerWizardTablesForm* m_viewsPage;
    ReverseEngineerWizardOptionsForm* m_optionsPage;

    QString m_host;
    QString m_user;
    QString m_pass;
    QString m_database;
};

#endif // REVERSEENGINEERWIZARD_H
