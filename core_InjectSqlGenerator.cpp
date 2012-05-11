#include "core_InjectSqlGenerator.h"
#include "core_InjectSqlGeneratorThread.h"
#include "core_ConnectionManager.h"
#include <QThread>

InjectSqlGenerator::InjectSqlGenerator(Version* v, QStringList connectionNames, QObject *parent) : QObject(parent), m_version(v), m_connectionNames(connectionNames)
{
}

void InjectSqlGenerator::generate()
{
    for(int i=0; i<m_connectionNames.size(); i++)
    {
        InjectSqlGeneratorThread* genThread = new InjectSqlGeneratorThread(m_version, parent(), ConnectionManager::instance()->getConnection(m_connectionNames[i]));
        QThread* a = new QThread(this);
        genThread->moveToThread(a);
        connect(this, SIGNAL(startWork()), genThread, SLOT(doWork()));
        connect(genThread, SIGNAL(done(InjectSqlGeneratorThread*)), this, SLOT(onSqlGeneratorThreadIsDone(InjectSqlGeneratorThread*)));
        a->start();
    }
    emit startWork();
}

void InjectSqlGenerator::onSqlGeneratorThreadIsDone(InjectSqlGeneratorThread* t)
{
    m_sqls[t->getConnection()] = t->getSqls();
    if(m_sqls.keys().size() == m_connectionNames.size()) emit done(this);
}
