#include "core_InjectSqlGeneratorThread.h"
#include "Version.h"

#include <QDebug>

InjectSqlGeneratorThread::InjectSqlGeneratorThread(Version* v, QObject *parent) : QObject(parent), m_version(v)
{
}

void InjectSqlGeneratorThread::doWork()
{
    m_sqls = m_version->getSqlScript(false);
    emit done(this);
}
