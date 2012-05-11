#ifndef CORE_INJECTSQLGENERATORTHREAD_H
#define CORE_INJECTSQLGENERATORTHREAD_H

#include <QObject>
#include <QStringList>

class Version;
class Connection;

class InjectSqlGeneratorThread : public QObject
{
    Q_OBJECT
public:
    explicit InjectSqlGeneratorThread(Version* v, QObject *parent, Connection*);
    QStringList getSqls() const
    {
        return m_sqls;
    }
    Connection* getConnection() const
    {
        return m_connection;
    }

public slots:
    void doWork();

signals:
    void done(InjectSqlGeneratorThread*);

private:
    QStringList m_sqls;
    Version* m_version;
    Connection* m_connection;
};

#endif // CORE_INJECTSQLGENERATORTHREAD_H
