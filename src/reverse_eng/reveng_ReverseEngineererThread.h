#ifndef CORE_REVERSEENGINEERERTHREAD_H
#define CORE_REVERSEENGINEERERTHREAD_H

#include "Connection.h"

#include <QObject>
#include <QStringList>
#include <QVector>
#include <QSharedPointer>

class Project;
class DatabaseEngine;


class ReverseEngineererThread : public QObject
{
    Q_OBJECT
public:

    explicit ReverseEngineererThread(bool createDataTypesForColumns,
                                     DatabaseEngine* engine, Project* p,
                                     QSharedPointer<Connection> conn,
                                     const QStringList& tabsToReverse,
                                     const QStringList& viewsToReverse,
                                     const QStringList& procsToReverse,
                                     const QStringList& funcsToReverse,
                                     const QStringList& triggersToReverse,
                                     QObject *parent = 0);

public slots:
    void doWork();

signals:
    void done(ReverseEngineererThread*);

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

#endif // CORE_REVERSEENGINEERERTHREAD_H
