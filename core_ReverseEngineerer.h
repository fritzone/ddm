#ifndef CORE_REVERSEENGINEERER_H
#define CORE_REVERSEENGINEERER_H

#include <QObject>
#include <QStringList>
#include <QVector>

class ReverseEngineererThread;
class Project;
class DatabaseEngine;

class ReverseEngineerer : public QObject
{
    Q_OBJECT
public:
    explicit ReverseEngineerer(bool c, DatabaseEngine* engine, Project* p, const QString& host, const QString& user, const QString& pass, const QString& db,  const QStringList& tabsToReverse,  const QStringList& viewsToReverse, QObject *parent = 0);
    void reverseEngineer();
signals:
    void done(ReverseEngineerer*);
    void startWork();

public slots:
    void onReverseEngineerThreadIsDone(ReverseEngineererThread*);
private:
    QString m_host;
    QString m_user;
    QString m_pass;
    QString m_db;
    QStringList m_tabsToReverse;
    QStringList m_viewsToReverse;
    DatabaseEngine* m_engine;
    Project* m_project;
    bool m_createDataTypesForColumns;
};


#endif // CORE_REVERSEENGINEERER_H
