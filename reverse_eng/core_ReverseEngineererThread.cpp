#include "core_ReverseEngineererThread.h"
#include "db_DatabaseEngine.h"
#include "core_Connection.h"

ReverseEngineererThread::ReverseEngineererThread(bool c, DatabaseEngine* engine, Project* p,
                                                 const QString &host, const QString &user, const QString &pass,
                                                 const QString &db,
                                                 const QStringList& tabsToReverse, const QStringList& viewsToReverse,
                                                 const QStringList& procsToReverse, const QStringList& funcsToReverse,
                                                 const QStringList& triggersToReverse,
                                                 QObject *parent):
    QObject(parent), m_host(host), m_user(user), m_pass(pass), m_db(db),
    m_tabsToReverse(tabsToReverse), m_viewsToReverse(viewsToReverse),
    m_procsToReverse(procsToReverse), m_funcsToReverse(funcsToReverse),
    m_triggersToReverse(triggersToReverse),
    m_engine(engine), m_project(p), m_createDataTypesForColumns(c)
{

}

void ReverseEngineererThread::doWork()
{
    Connection* c = new Connection("temp", m_host, m_user, m_pass, m_db, false, false);
    m_engine->reverseEngineerDatabase(c, m_tabsToReverse, m_viewsToReverse, m_procsToReverse, m_funcsToReverse,
                                      m_triggersToReverse, m_project, m_createDataTypesForColumns);
    emit done(this);
    delete c;
}
