#ifndef CORE_DEPLOYERTHREAD_H
#define CORE_DEPLOYERTHREAD_H

#include <QThread>
#include <QStringList>

class Connection;
class DatabaseEngine;

class DeployerThread : public QThread
{
    Q_OBJECT
public:
    explicit DeployerThread(DatabaseEngine* e, Connection* c, const QStringList& sqls, int i, QObject *parent = 0);
    void run();
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

signals:
    void done(int);

private:

    Connection* m_connection;
    DatabaseEngine* m_engine;
    QStringList m_sqls;
    QString m_lastSql;
    bool m_success;
    int m_idx;
    QString m_lastError;
};

#endif // CORE_DEPLOYERTHREAD_H
