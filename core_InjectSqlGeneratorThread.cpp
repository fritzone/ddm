#include "core_InjectSqlGeneratorThread.h"
#include "Version.h"

#include <QDebug>

InjectSqlGeneratorThread::InjectSqlGeneratorThread(Version* v, QObject *parent, Connection * c) :
    QObject(parent), m_version(v), m_connection(c)
{
}

void InjectSqlGeneratorThread::doWork()
{
    m_sqls = m_version->getSqlScript(false, m_connection);
    emit done(this);
}
