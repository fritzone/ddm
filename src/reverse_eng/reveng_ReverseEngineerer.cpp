#include "reveng_ReverseEngineerer.h"
#include "reveng_ReverseEngineererThread.h"
#include <QThread>
#include <QDebug>

ReverseEngineerer::ReverseEngineerer(bool c, DatabaseEngine* engine, Project* p,
                                     const QString &host, const QString &user, const QString &pass, const QString &db, int port,
                                     const QStringList& tabsToReverse, const QStringList& viewsToReverse,
                                     const QStringList& procsToReverse, const QStringList& funcsToReverse,
                                     const QStringList& triggersToReverse,
                                     QObject *parent) :
    QObject(parent), m_host(host), m_user(user), m_pass(pass), m_db(db),
    m_tabsToReverse(tabsToReverse), m_viewsToReverse(viewsToReverse), m_procsToReverse(procsToReverse),
    m_funcsToReverse(funcsToReverse), m_triggersToReverse(triggersToReverse),
    m_engine(engine), m_project(p), m_createDataTypesForColumns(c), m_port(port), m_sqliteFile()
{

}

ReverseEngineerer::ReverseEngineerer(bool c, DatabaseEngine *engine, Project *p,
                                     const QString &fileName, int sqliteVersion,
                                     const QStringList &tabsToReverse, const QStringList &viewsToReverse,
                                     const QStringList &procsToReverse, const QStringList &funcsToReverse,
                                     const QStringList &triggersToReverse, QObject *parent) :
    m_tabsToReverse(tabsToReverse), m_viewsToReverse(viewsToReverse), m_procsToReverse(procsToReverse),
    m_funcsToReverse(funcsToReverse), m_triggersToReverse(triggersToReverse),
    m_engine(engine), m_project(p), m_createDataTypesForColumns(c),
    m_sqliteFile(fileName), m_sqliteVersion(sqliteVersion)
{
}

void ReverseEngineerer::reverseEngineer()
{

    ReverseEngineererThread* genThread = 0;
    if(m_sqliteFile.isEmpty())
    {
        genThread = new ReverseEngineererThread(m_createDataTypesForColumns, m_engine, m_project, m_host,
                                                m_user, m_pass, m_db, m_port, m_tabsToReverse, m_viewsToReverse, m_procsToReverse,
                                                m_funcsToReverse, m_triggersToReverse);
    }
    else
    {
        genThread = new ReverseEngineererThread(m_createDataTypesForColumns, m_engine, m_project,
                                                m_sqliteFile, m_sqliteVersion,
                                                m_tabsToReverse, m_viewsToReverse, m_procsToReverse,
                                                m_funcsToReverse, m_triggersToReverse);
    }
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
