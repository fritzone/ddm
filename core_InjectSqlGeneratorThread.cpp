#include "core_InjectSqlGeneratorThread.h"
#include "Version.h"
#include "pthread.h"

#include <QDebug>

InjectSqlGeneratorThread::InjectSqlGeneratorThread(Version* v, QObject *parent) : QObject(parent), m_version(v)
{
}

void InjectSqlGeneratorThread::doWork()
{
    qDebug() << "doWork_thread" << pthread_self();
    m_sqls = m_version->getSqlScript(/*injectDialog->getCodepage()*/ "latin1");
    emit done(this);
}
