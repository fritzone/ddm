#include "core_ReverseEngineerer.h"
#include "core_ReverseEngineererThread.h"
#include <QThread>

ReverseEngineerer::ReverseEngineerer(bool c, DatabaseEngine* engine, Project* p,
                                     const QString &host, const QString &user, const QString &pass, const QString &db,
                                     const QStringList& tabsToReverse, const QStringList& viewsToReverse, const QStringList& procsToReverse,
                                     QObject *parent):
        QObject(parent), m_host(host), m_user(user), m_pass(pass), m_db(db),
        m_tabsToReverse(tabsToReverse), m_viewsToReverse(viewsToReverse), m_procsToReverse(procsToReverse),
        m_engine(engine), m_project(p), m_createDataTypesForColumns(c)
{

}

void ReverseEngineerer::reverseEngineer()
{
    ReverseEngineererThread* genThread = new ReverseEngineererThread(m_createDataTypesForColumns, m_engine, m_project,
                                                                     m_host, m_user, m_pass, m_db, m_tabsToReverse, m_viewsToReverse, m_procsToReverse);
    QThread* a = new QThread(this);
    genThread->moveToThread(a);
    connect(this, SIGNAL(startWork()), genThread, SLOT(doWork()));
    connect(genThread, SIGNAL(done(ReverseEngineererThread*)), this, SLOT(onReverseEngineerThreadIsDone(ReverseEngineererThread*)));
    a->start();
    emit startWork();
}

void ReverseEngineerer::onReverseEngineerThreadIsDone(ReverseEngineererThread*)
{
    emit done(this);
}
