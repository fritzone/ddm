#include "core_DeployerThread.h"
#include "db_DatabaseEngine.h"
#include "core_Connection.h"
#include "IconFactory.h"

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
    else
    {
        if(m_connection->getState() == CONNECTED)
        {
            while(m_connection->getLocation()->childCount()) m_connection->getLocation()->removeChild(m_connection->getLocation()->child(0));
            m_connection->setState(DID_NOT_TRY);
            m_connection->getLocation()->setIcon(0, IconFactory::getConnectionStateIcon(m_connection->getState()));
        }
    }
    emit(done(m_idx));
}

