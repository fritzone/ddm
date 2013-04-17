#include "reveng_ReverseEngineererThread.h"
#include "db_DatabaseEngine.h"
#include "MySqlConnection.h"
#include "SqliteConnection.h"

ReverseEngineererThread::ReverseEngineererThread(bool createDataTypesForColumns, DatabaseEngine* engine, Project* p,
                                                 const QString &host, const QString &user, const QString &pass,
                                                 const QString &db, int port,
                                                 const QStringList& tabsToReverse, const QStringList& viewsToReverse,
                                                 const QStringList& procsToReverse, const QStringList& funcsToReverse,
                                                 const QStringList& triggersToReverse,
                                                 QObject *parent):
    QObject(parent), m_host(host), m_user(user), m_pass(pass), m_db(db),
    m_tabsToReverse(tabsToReverse), m_viewsToReverse(viewsToReverse),
    m_procsToReverse(procsToReverse), m_funcsToReverse(funcsToReverse),
    m_triggersToReverse(triggersToReverse),
    m_engine(engine), m_project(p), m_createDataTypesForColumns(createDataTypesForColumns), m_port(port), m_fileName()
{
}


ReverseEngineererThread::ReverseEngineererThread(bool createDataTypesForColumns, DatabaseEngine* engine, Project* p,
                                                 const QString& fileName, int sqliteVersion,
                                                 const QStringList& tabsToReverse, const QStringList& viewsToReverse,
                                                 const QStringList& procsToReverse, const QStringList& funcsToReverse,
                                                 const QStringList& triggersToReverse,
                                                 QObject *parent):
    QObject(parent), m_host(), m_user(), m_pass(), m_db(),
    m_tabsToReverse(tabsToReverse), m_viewsToReverse(viewsToReverse),
    m_procsToReverse(procsToReverse), m_funcsToReverse(funcsToReverse),
    m_triggersToReverse(triggersToReverse),
    m_engine(engine), m_project(p), m_createDataTypesForColumns(createDataTypesForColumns), m_port(-1),
    m_fileName(fileName), m_sqliteVersion(sqliteVersion)

{
}

void ReverseEngineererThread::doWork()
{
    Connection *c = 0;
    if(m_engine->getDatabaseEngineName().toUpper() == "MYSQL")
    {
        c = new MySqlConnection("temp", m_host, m_user, m_pass, m_db, false, false, m_port);
    }
    else
    {
        c = new SqliteConnection("temp", m_fileName, false, m_sqliteVersion);
    }

    if(!m_engine->reverseEngineerDatabase(c, m_tabsToReverse, m_viewsToReverse, m_procsToReverse, m_funcsToReverse,
                                      m_triggersToReverse, m_project, m_createDataTypesForColumns))
    {
        qDebug() << "exception caught";
    }
    emit done(this);
    delete c;

}
