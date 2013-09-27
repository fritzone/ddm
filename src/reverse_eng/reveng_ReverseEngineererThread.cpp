#include "reveng_ReverseEngineererThread.h"
#include "db_DatabaseEngine.h"
#include "MySqlConnection.h"
#include "SqliteConnection.h"
#include "conn_CUBRID.h"

ReverseEngineererThread::ReverseEngineererThread(bool createDataTypesForColumns, DatabaseEngine* engine, Project* p,
                                                 QSharedPointer<Connection> conn,
                                                 const QStringList& tabsToReverse, const QStringList& viewsToReverse,
                                                 const QStringList& procsToReverse, const QStringList& funcsToReverse,
                                                 const QStringList& triggersToReverse,
                                                 QObject *parent):
    QObject(parent),
    m_tabsToReverse(tabsToReverse), m_viewsToReverse(viewsToReverse),
    m_procsToReverse(procsToReverse), m_funcsToReverse(funcsToReverse),
    m_triggersToReverse(triggersToReverse),
    m_engine(engine), m_project(p), m_createDataTypesForColumns(createDataTypesForColumns),
    m_conn(conn)
{
}

void ReverseEngineererThread::doWork()
{
    if(!m_engine->reverseEngineerDatabase(m_conn.data(), m_tabsToReverse, m_viewsToReverse, m_procsToReverse, m_funcsToReverse,
                                      m_triggersToReverse, m_project, m_createDataTypesForColumns))
    {
        qDebug() << "Exception caught while reerse engineering";
    }
    emit done(this);
}
