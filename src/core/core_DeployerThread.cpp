#include "core_DeployerThread.h"
#include "db_DatabaseEngine.h"
#include "Connection.h"
#include "IconFactory.h"
#include "Version.h"

DeployerThread::DeployerThread(DatabaseEngine* e, Connection* c,
                               const QStringList& sqls, const QStringList& uids, int i,
                               bool injectMetadata, const Version *v,
                               QObject *parent) :
    QObject(parent), m_connection(c), m_engine(e), m_sqls(sqls), m_uids(uids), m_lastSql(),
    m_success(false), m_idx(i), m_injectMetadata(injectMetadata), m_version(v)
{
}

void DeployerThread::doWork()
{
//    qDebug() << "GGGGGGGGGGG:" << m_uids;
    m_success = m_engine->executeSql(m_connection, m_sqls, m_uids, m_lastSql, true);
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

    // Now inject the project metadata if required
    if(/*m_success && */m_injectMetadata)
    {
        m_engine->injectMetadata(m_connection, m_version);
    }
    emit(done(m_idx));
}

