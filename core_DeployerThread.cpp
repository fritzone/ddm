#include "core_DeployerThread.h"
#include "db_DatabaseEngine.h"
#include "core_Connection.h"

DeployerThread::DeployerThread(DatabaseEngine* e, Connection* c, const QStringList& sqls, int i, QObject *parent) :
    QObject(parent), m_connection(c), m_engine(e), m_sqls(sqls), m_lastSql(), m_success(false), m_idx(i)
{
}

void DeployerThread::doWork()
{
    m_success = m_engine->executeSql(m_connection, m_sqls, m_lastSql, true);
    if(!m_success)
    {
        m_lastError = m_engine->getLastError();
    }
    emit(done(m_idx));
}

