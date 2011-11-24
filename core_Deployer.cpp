#include "core_Deployer.h"
#include "core_DeployerThread.h"
#include "core_ConnectionManager.h"
#include "Workspace.h"

Deployer::Deployer(const QStringList& connections, const QStringList& sqls, QObject *parent) : QObject(parent), m_deployerThreads(), m_connections(connections), m_sqls(sqls)
{
}

void Deployer::deploy()
{
    for(int i=0; i<m_connections.size(); i++)
    {
        DeployerThread* thread = new DeployerThread(Workspace::getInstance()->currentProjectsEngine(), ConnectionManager::instance()->getConnection(m_connections.at(i)), m_sqls, i);
        QThread *a = new QThread(this);
        thread->moveToThread(a);
        connect(this, SIGNAL(startWork()), thread, SLOT(doWork()));
        connect(thread, SIGNAL(done(int)), this, SLOT(onDeployerThreadIsDone(int)));
        m_deployerThreads.append(thread);
        a->start();
    }

    emit startWork();
}

void Deployer::onDeployerThreadIsDone(int idx)
{
    qDebug() << idx;
    m_finishedThreads.append(idx);
    if(m_finishedThreads.size() == m_connections.size()) emit done(this);
}

bool Deployer::hadErrors()
{
    for(int i=0; i<m_deployerThreads.size(); i++)
    {
        if(m_deployerThreads.at(i)->wasFaulted()) return true;
    }
    return false;
}

QMap<QString, QString> Deployer::getErrors()
{
    QMap<QString, QString> result;
    for(int i=0; i<m_deployerThreads.size(); i++)
    {
        if(m_deployerThreads.at(i)->wasFaulted())
        {
            result[m_connections.at(i)] = m_deployerThreads.at(i)->getLastError();
        }
    }
    return result;
}