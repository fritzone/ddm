#include "core_Deployer.h"
#include "core_DeployerThread.h"
#include "ConnectionManager.h"
#include "Workspace.h"

Deployer::Deployer(const QStringList& connections, const QMap<Connection *,
                   QStringList> &sqls, const QMap<QString,
                   QStringList> &uids, bool injectMetadata, const Version* v,
                   QObject *parent) :
    QObject(parent), m_deployerThreads(), m_connections(connections),
    m_sqls(sqls), m_uids(uids), m_injectMetadata(injectMetadata), m_version(v)
{
//    qDebug() << "OOOOOOOOOOOOOOOOOOO:" << m_uids;
}

void Deployer::deploy()
{
    for(int i=0; i<m_connections.size(); i++)
    {
        Connection* c = ConnectionManager::instance()->getConnection(m_connections.at(i));
//        qDebug() << "JJJJJJJJJJJJJ:" << c->getName() << m_uids[c->getName()];
        DeployerThread* thread = new DeployerThread(Workspace::getInstance()->currentProjectsEngine(),
                                                    c, m_sqls[c], m_uids[c->getName()], i, m_injectMetadata, m_version, 0);
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

QMap<QString, QString> Deployer::getErrors(QMap<QString, QStringList> & uids)
{
    QMap<QString, QString> result;
    for(int i=0; i<m_deployerThreads.size(); i++)
    {
        if(m_deployerThreads.at(i)->wasFaulted())
        {
            result[m_connections.at(i)] = m_deployerThreads.at(i)->getLastError();
            uids[m_connections.at(i)] = m_deployerThreads.at(i)->getUids();
        }
    }
    return result;
}
