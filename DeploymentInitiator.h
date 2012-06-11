#ifndef DEPLOYMENTINITIATOR_H
#define DEPLOYMENTINITIATOR_H

#include <QObject>
#include <QVector>

class Deployer;
class InjectSqlGenerator;
class Version;

class DeploymentInitiator : public QObject
{
    Q_OBJECT
public:
    explicit DeploymentInitiator(QObject *parent = 0);
    void doDeployment(Version *v, QStringList connectionNames, bool metadataInject, const QStringList& uidsToDeploy, const QStringList& uidsToDrop);
    void doDeployment(Version *v, QStringList connectionNames, bool metadataInject);

public slots:
    void onSqlGenerationFinished(InjectSqlGenerator*);

private:
    QVector<Deployer*> m_deployers;

};

#endif // DEPLOYMENTINITIATOR_H
