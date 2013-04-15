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
    /**
     * @brief ReverseEngineererThread reverse engineer a database which has an authentication model (such as mysql)
     * @param createDataTypesForColumns
     * @param engine
     * @param p
     * @param host
     * @param user
     * @param pass
     * @param db
     * @param port
     * @param tabsToReverse
     * @param viewsToReverse
     * @param procsToReverse
     * @param funcsToReverse
     * @param triggersToReverse
     * @param parent
     */
    explicit ReverseEngineererThread(bool createDataTypesForColumns, DatabaseEngine* engine, Project* p,
                                     const QString& host, const QString& user, const QString& pass, const QString& db, int port,
                                     const QStringList& tabsToReverse,  const QStringList& viewsToReverse,
                                     const QStringList& procsToReverse, const QStringList& funcsToReverse,
                                     const QStringList& triggersToReverse, QObject *parent = 0);


    /**
     * @brief ReverseEngineererThread reverse engineer a database which stores its data in files (sqlite)
     * @param createDataTypesForColumns
     * @param engine
     * @param p
     * @param fileName
     * @param tabsToReverse
     * @param viewsToReverse
     * @param procsToReverse
     * @param funcsToReverse
     * @param triggersToReverse
     * @param parent
     */
    explicit ReverseEngineererThread(bool createDataTypesForColumns, DatabaseEngine* engine, Project* p,
                                     const QString& fileName,
                                     const QStringList& tabsToReverse,  const QStringList& viewsToReverse,
                                     const QStringList& procsToReverse, const QStringList& funcsToReverse,
                                     const QStringList& triggersToReverse, QObject *parent = 0);

public slots:
    void doWork();

signals:
    void done(ReverseEngineererThread*);

private:
    QString m_host;
    QString m_user;
    QString m_pass;
    QString m_db;
    QStringList m_tabsToReverse;
    QStringList m_viewsToReverse;
    QStringList m_procsToReverse;
    QStringList m_funcsToReverse;
    QStringList m_triggersToReverse;
    DatabaseEngine* m_engine;
    Project* m_project;
    bool m_createDataTypesForColumns;
    int m_port;
    QString m_fileName;
};

#endif // CORE_REVERSEENGINEERERTHREAD_H
