#ifndef CORE_REVERSEENGINEERER_H
#define CORE_REVERSEENGINEERER_H

#include <QObject>
#include <QStringList>
#include <QVector>
#include <QSharedPointer>

#include "Connection.h"

class ReverseEngineererThread;
class Project;
class DatabaseEngine;
class Connection;

class ReverseEngineerer : public QObject
{
    Q_OBJECT
public:

    explicit ReverseEngineerer(bool c, DatabaseEngine* engine, Project* p,
                               QSharedPointer<Connection> conn,
                               const QStringList& tabsToReverse, const QStringList& viewsToReverse,
                               const QStringList& procsToReverse, const QStringList& funcsToReverse,
                               const QStringList& triggersToReverse,
                               QObject *parent = 0);

    virtual ~ReverseEngineerer() {}

    void reverseEngineer();

signals:
    void done(ReverseEngineerer*);
    void startWork();

public slots:
    void onReverseEngineerThreadIsDone(ReverseEngineererThread*);
private:
    QStringList m_tabsToReverse;
    QStringList m_viewsToReverse;
    QStringList m_procsToReverse;
    QStringList m_funcsToReverse;
    QStringList m_triggersToReverse;
    DatabaseEngine* m_engine;
    Project* m_project;
    bool m_createDataTypesForColumns;
    QSharedPointer<Connection> m_conn;
};


#endif // CORE_REVERSEENGINEERER_H
