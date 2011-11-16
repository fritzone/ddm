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
    explicit DeployerThread(DatabaseEngine* e, Connection* c, const QStringList& sqls, QObject *parent = 0);

    void run();

signals:

public slots:

private:

    Connection* m_connection;
    DatabaseEngine* m_engine;
    QStringList m_sqls;

};

#endif // CORE_DEPLOYERTHREAD_H
