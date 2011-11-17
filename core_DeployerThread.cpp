#include "core_DeployerThread.h"
#include "db_DatabaseEngine.h"
#include "core_Connection.h"

DeployerThread::DeployerThread(DatabaseEngine* e, Connection* c, const QStringList& sqls, int i, QObject *parent) :
    QThread(parent), m_connection(c), m_engine(e), m_sqls(sqls), m_lastSql(), m_success(false), m_idx(i)
{
}

void DeployerThread::run()
{
    m_success = m_engine->injectSql(m_connection->getHost(), m_connection->getUser(), m_connection->getPassword(), m_connection->getDb(), m_sqls, m_lastSql, true, false);
    if(!m_success)
    {
        m_lastError = m_engine->getLastError();
    }
    emit(done(m_idx));
}
