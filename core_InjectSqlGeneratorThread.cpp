#include "core_InjectSqlGeneratorThread.h"
#include "Version.h"

#include <QDebug>

InjectSqlGeneratorThread::InjectSqlGeneratorThread(const QString& codePage, Version* v, QObject *parent) : QObject(parent), m_version(v), m_codePage(codePage)
{
}

void InjectSqlGeneratorThread::doWork()
{
    m_sqls = m_version->getSqlScript(m_codePage);
    emit done(this);
}
