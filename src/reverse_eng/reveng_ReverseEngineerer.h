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

    /**
     * @brief ReverseEngineerer creates an object for databases with authentication model (sqlite)
     * @param c
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
    explicit ReverseEngineerer(bool c, DatabaseEngine* engine, Project* p,
                               const QString& host, const QString& user, const QString& pass, const QString& db, int port,
                               const QStringList& tabsToReverse, const QStringList& viewsToReverse,
                               const QStringList& procsToReverse, const QStringList& funcsToReverse,
                               const QStringList& triggersToReverse,
                               QObject *parent = 0);

    /**
     * @brief ReverseEngineerer creates an object for databases with file based model (sqlite)
     * @param c
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
    explicit ReverseEngineerer(bool c, DatabaseEngine* engine, Project* p,
                               const QString& fileName,
                               const QStringList& tabsToReverse, const QStringList& viewsToReverse,
                               const QStringList& procsToReverse, const QStringList& funcsToReverse,
                               const QStringList& triggersToReverse,
                               QObject *parent = 0);

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
    QStringList m_procsToReverse;
    QStringList m_funcsToReverse;
    QStringList m_triggersToReverse;
    DatabaseEngine* m_engine;
    Project* m_project;
    bool m_createDataTypesForColumns;
    int m_port;
    QString m_sqliteFile;
};


#endif // CORE_REVERSEENGINEERER_H
