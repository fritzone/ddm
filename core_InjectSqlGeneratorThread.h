#ifndef CORE_INJECTSQLGENERATORTHREAD_H
#define CORE_INJECTSQLGENERATORTHREAD_H

#include <QObject>
#include <QStringList>

class Version;

class InjectSqlGeneratorThread : public QObject
{
    Q_OBJECT
public:
    explicit InjectSqlGeneratorThread(const QString& codePage, Version* v, QObject *parent = 0);
    QStringList getSqls() const
    {
        return m_sqls;
    }

public slots:
    void doWork();

signals:
    void done(InjectSqlGeneratorThread*);

private:
    QStringList m_sqls;
    Version* m_version;
    QString m_codePage;
};

#endif // CORE_INJECTSQLGENERATORTHREAD_H
