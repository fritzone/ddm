#ifndef CORE_DEPLOYERTHREAD_H
#define CORE_DEPLOYERTHREAD_H

#include <QThread>
#include <QStringList>

class Connection;
class DatabaseEngine;
class Version;

class DeployerThread : public QObject
{
    Q_OBJECT
public:
    explicit DeployerThread(DatabaseEngine* e, Connection* c, const QStringList& sqls, const QStringList &uids,
                            int i, bool injectMetadata,
                            const Version* v,
                            QObject *parent);
    bool wasFaulted() const
    {
        return !m_success;
    }
    QString getLastSql() const
    {
        return m_lastSql;
    }
    QString getLastError() const
    {
        return m_lastError;
    }
    QStringList getUids() const
    {
        return m_uids;
    }
public slots:
    void doWork();

signals:
    void done(int);

private:

    Connection* m_connection;
    DatabaseEngine* m_engine;
    QStringList m_sqls;
    QStringList m_uids;
    QString m_lastSql;
    bool m_success;
    int m_idx;
    QString m_lastError;
    bool m_injectMetadata;
    const Version* m_version;
};

#endif // CORE_DEPLOYERTHREAD_H
