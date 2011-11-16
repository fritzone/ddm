#include "core_DeployerThread.h"
#include "db_DatabaseEngine.h"
#include "core_Connection.h"

DeployerThread::DeployerThread(DatabaseEngine* e, Connection* c, const QStringList& sqls, QObject *parent) :
    QThread(parent), m_connection(c), m_engine(e), m_sqls(sqls)
{
}

void DeployerThread::run()
{
    QString tSql;
    m_engine->injectSql(m_connection->getHost(), m_connection->getUser(), m_connection->getPassword(), m_connection->getDb(), m_sqls, tSql, true, false);
}
