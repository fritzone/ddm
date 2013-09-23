#include "DeploymentInitiator.h"
#include "core_InjectSqlGenerator.h"
#include "Connection.h"
#include "MainWindow.h"
#include "core_Deployer.h"
#include "Version.h"
#include "Project.h"
#include "core_Table.h"
#include "core_TableInstance.h"
#include "core_View.h"
#include "core_Procedure.h"
#include "core_Trigger.h"
#include "core_Function.h"


DeploymentInitiator::DeploymentInitiator(QObject *parent) : QObject(parent),
    m_deployers()
{
}

void DeploymentInitiator::doDeployment(Version* v, QStringList connectionNames, bool metadataInject,
                                       const QStringList &/*uidsToDeploy*/, const QStringList& /*uidsToDrop*/)
{
    InjectSqlGenerator* injectSqlGen = new InjectSqlGenerator(v, connectionNames, 0, metadataInject);
    connect(injectSqlGen, SIGNAL(done(InjectSqlGenerator*)), this, SLOT(onSqlGenerationFinished(InjectSqlGenerator*)));
    injectSqlGen->generate();
}

void DeploymentInitiator::doDeployment(Version* v, QStringList connectionNames, bool metadataInject)
{
    // generic case, everything should be deployed, gather all the uids
    QStringList uidsToDeploy;
    if(v->getProject()->oopProject())
    {
        QVector <TableInstance*> tabs = v->getTableInstances();
        for(int i=0; i<tabs.size(); i++)
        {
            uidsToDeploy.append(tabs.at(i)->getObjectUid().toString());
        }
    }
    else
    {
        QVector <Table*> tabs = v->getTables();
        for(int i=0; i<tabs.size(); i++)
        {
            uidsToDeploy.append(tabs.at(i)->getObjectUid().toString());
        }
    }
    QVector <View*> views = v->getViews();
    for(int i=0; i<views.size(); i++)
    {
        uidsToDeploy.append(views.at(i)->getObjectUid().toString());
    }
    QVector <Procedure*> procedures = v->getProcedures();
    for(int i=0; i<procedures.size(); i++)
    {
        uidsToDeploy.append(procedures.at(i)->getObjectUid().toString());
    }
    QVector <Function*> functions = v->getFunctions();
    for(int i=0; i<functions.size(); i++)
    {
        uidsToDeploy.append(functions.at(i)->getObjectUid().toString());
    }
    QVector <Trigger*> triggers = v->getTriggers();
    for(int i=0; i<triggers.size(); i++)
    {
        uidsToDeploy.append(triggers.at(i)->getObjectUid().toString());
    }
    doDeployment(v, connectionNames, metadataInject, uidsToDeploy, QStringList());
}

void DeploymentInitiator::onSqlGenerationFinished(InjectSqlGenerator *generator)
{
    QMap<Connection*, QStringList> sqlList = generator->getSqls();
    QMap<QString, QStringList> uidList = generator->getUids();
//    qDebug() << "AAAAAAAAAA:" << uidList;
    QStringList connectionNames = generator->getConnectionNames();
    Deployer* deployer = new Deployer(connectionNames, sqlList, uidList,
                                      generator->metadataInjectRequired(),
                                      generator->getVersion(), 0);
    m_deployers.append(deployer);
    connect(deployer, SIGNAL(done(Deployer*)), MainWindow::instance(), SLOT(onDeploymentFinished(Deployer*)));
    deployer->deploy();
}
