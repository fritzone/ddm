#ifndef CORE_DEPLOYER_H
#define CORE_DEPLOYER_H

#include <QObject>
#include <QVector>
#include <QStringList>
#include <QMap>

class DeployerThread;
class Connection;
class Version;

class Deployer : public QObject
{
    Q_OBJECT
public:
    explicit Deployer(const QStringList& connections, const QMap<Connection*,
                      QStringList>& sqls, bool injectMetadata, const Version* v,
                      QObject *parent);
    void deploy();
    bool hadErrors();
    QMap<QString, QString> getErrors();

signals:
    void done(Deployer*);
    void startWork();

public slots:
     void onDeployerThreadIsDone(int);

private:
    QVector<DeployerThread*> m_deployerThreads;
    QStringList m_connections;
    QMap<Connection*, QStringList> m_sqls;
    QVector<int> m_finishedThreads;
    bool m_injectMetadata;
    const Version* m_version;
};

#endif // CORE_DEPLOYER_H
