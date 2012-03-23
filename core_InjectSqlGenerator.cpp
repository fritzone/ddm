#include "core_InjectSqlGenerator.h"
#include "core_InjectSqlGeneratorThread.h"

#include <QThread>

InjectSqlGenerator::InjectSqlGenerator(Version* v, QStringList connectionNames, QObject *parent) : QObject(parent), m_version(v), m_connectionNames(connectionNames)
{
}

void InjectSqlGenerator::generate()
{
    InjectSqlGeneratorThread* genThread = new InjectSqlGeneratorThread(m_version);
    QThread* a = new QThread(this);
    genThread->moveToThread(a);
    connect(this, SIGNAL(startWork()), genThread, SLOT(doWork()));
    connect(genThread, SIGNAL(done(InjectSqlGeneratorThread*)), this, SLOT(onSqlGeneratorThreadIsDone(InjectSqlGeneratorThread*)));
    a->start();
    emit startWork();
}

void InjectSqlGenerator::onSqlGeneratorThreadIsDone(InjectSqlGeneratorThread* t)
{
    m_sqls = t->getSqls();
    emit done(this);
}
