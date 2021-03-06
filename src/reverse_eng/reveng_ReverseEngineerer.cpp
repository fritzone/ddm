#include "reveng_ReverseEngineerer.h"
#include "reveng_ReverseEngineererThread.h"
#include <QThread>
#include <QDebug>

ReverseEngineerer::ReverseEngineerer(bool c, DatabaseEngine* engine, Project* p,
                                     QSharedPointer<Connection> conn,
                                     const QStringList& tabsToReverse, const QStringList& viewsToReverse,
                                     const QStringList& procsToReverse, const QStringList& funcsToReverse,
                                     const QStringList& triggersToReverse,
                                     QObject *parent) :
    QObject(parent),
    m_tabsToReverse(tabsToReverse), m_viewsToReverse(viewsToReverse), m_procsToReverse(procsToReverse),
    m_funcsToReverse(funcsToReverse), m_triggersToReverse(triggersToReverse),
    m_engine(engine), m_project(p), m_createDataTypesForColumns(c),
    m_conn(conn)
{

}

void ReverseEngineerer::reverseEngineer()
{
    ReverseEngineererThread* genThread = new ReverseEngineererThread(m_createDataTypesForColumns,
                                                m_engine, m_project, m_conn, m_tabsToReverse,
                                                m_viewsToReverse, m_procsToReverse,
                                                m_funcsToReverse, m_triggersToReverse);
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
