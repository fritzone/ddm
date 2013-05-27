#ifndef CORE_INJECTSQLGENERATOR_H
#define CORE_INJECTSQLGENERATOR_H

#include <QObject>
#include <QStringList>
#include <QMap>

class Version;
class InjectSqlGeneratorThread;
class Connection;

class InjectSqlGenerator : public QObject
{
    Q_OBJECT
public:
    explicit InjectSqlGenerator(Version* v, QStringList connectionNames,
                                QObject *parent, bool injectRequired);
    QStringList getSqls(Connection* c)
    {
        return m_sqls[c];
    }
    QMap<Connection*, QStringList> getSqls()
    {
        return m_sqls;
    }
    QMap<QString, QStringList> getUids()
    {
        return m_uids;
    }
    QStringList getConnectionNames()
    {
        return m_connectionNames;
    }
    void generate();
    Version* getVersion() const
    {
        return m_version;
    }
    bool metadataInjectRequired() const
    {
        return m_metadataInjectRequired;
    }
signals:
    void done(InjectSqlGenerator*);
    void startWork();

public slots:
    void onSqlGeneratorThreadIsDone(InjectSqlGeneratorThread*);
private:
    Version *m_version;
    QStringList m_connectionNames;
    QMap<Connection*, QStringList> m_sqls;
    QMap<QString, QStringList> m_uids;
    bool m_metadataInjectRequired;
};

#endif // CORE_INJECTSQLGENERATOR_H
