#include "core_ReverseEngineererThread.h"
#include "db_DatabaseEngine.h"

ReverseEngineererThread::ReverseEngineererThread(bool c, DatabaseEngine* engine, Project* p, const QString &host, const QString &user, const QString &pass, const QString &db, QVector<QString> tabsToReverse, QVector<QString> viewsToReverse, QObject *parent):
        QObject(parent), m_host(host), m_user(user), m_pass(pass), m_db(db), m_tabsToReverse(tabsToReverse), m_viewsToReverse(viewsToReverse), m_engine(engine), m_project(p), m_createDataTypesForColumns(c)
{

}

void ReverseEngineererThread::doWork()
{
    m_engine->reverseEngineerDatabase(m_host, m_user, m_pass, m_db, m_tabsToReverse, m_viewsToReverse, m_project, m_createDataTypesForColumns);
    emit done(this);
}
