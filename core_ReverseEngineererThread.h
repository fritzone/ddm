#ifndef CORE_REVERSEENGINEERERTHREAD_H
#define CORE_REVERSEENGINEERERTHREAD_H

#include <QObject>
#include <QStringList>
#include <QVector>

class Project;
class DatabaseEngine;

class ReverseEngineererThread : public QObject
{
    Q_OBJECT
public:
    explicit ReverseEngineererThread(bool c, DatabaseEngine* engine, Project* p, const QString& host, const QString& user, const QString& pass, const QString& db, QVector<QString> tabsToReverse, QVector<QString> viewsToReverse, QObject *parent = 0);

public slots:
    void doWork();

signals:
    void done(ReverseEngineererThread*);

private:
    QString m_host;
    QString m_user;
    QString m_pass;
    QString m_db;
    QVector<QString> m_tabsToReverse;
    QVector<QString> m_viewsToReverse;
    DatabaseEngine* m_engine;
    Project* m_project;
    bool m_createDataTypesForColumns;
};

#endif // CORE_REVERSEENGINEERERTHREAD_H
