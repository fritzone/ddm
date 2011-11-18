#ifndef CORE_INJECTSQLGENERATOR_H
#define CORE_INJECTSQLGENERATOR_H

#include <QObject>
#include <QStringList>

class Version;
class InjectSqlGeneratorThread;

class InjectSqlGenerator : public QObject
{
    Q_OBJECT
public:
    explicit InjectSqlGenerator(Version* v, QStringList connectionNames, QObject *parent = 0);
    QStringList getSqls()
    {
        return m_sqls;
    }

    QStringList getConnectionNames()
    {
        return m_connectionNames;
    }
    void generate();
signals:
    void done(InjectSqlGenerator*);
    void startWork();

public slots:
    void onSqlGeneratorThreadIsDone(InjectSqlGeneratorThread*);
private:
    Version *m_version;
    QStringList m_connectionNames;
    QStringList m_sqls;

};

#endif // CORE_INJECTSQLGENERATOR_H
