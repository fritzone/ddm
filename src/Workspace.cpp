#include "Workspace.h"
#include "Solution.h"
#include "Project.h"
#include "DeserializationFactory.h"
#include "ContextMenuCollection.h"
#include "db_DatabaseEngine.h"
#include "Version.h"
#include "core_UserDataType.h"
#include "ClipboardFactory.h"
#include "NameGenerator.h"
#include "VersionGuiElements.h"
#include "core_Table.h"
#include "InjectSqlDialog.h"
#include "MainWindow.h"
#include "core_ConnectionManager.h"
#include "ConnectionGuiElements.h"
#include "core_Column.h"
#include "InjectSqlDialog.h"
#include "DeploymentInitiator.h"
#include "core_Patch.h"
#include "MajorVersion.h"
#include "GuiElements.h"
#include "UidWarehouse.h"
#include "TriggerForm.h"
#include "core_Trigger.h"
#include "core_TableInstance.h"

#include <QFile>
#include <QApplication>
#include <QTreeWidget>
#include <QDockWidget>

Workspace* Workspace::m_instance = 0;

Workspace::Workspace() : m_solutions(), m_currentSolution(0), m_saved(true)
{}

Workspace* Workspace::getInstance()
{
    if(m_instance == 0)
    {
        m_instance = new Workspace();
    }

    return m_instance;
}

bool Workspace::createCurrentSolution(const QString& name)
{
    m_currentSolution = new Solution(name);
    m_solutions.append(m_currentSolution);
    return true;
}

bool Workspace::createSolution(const QString & name)
{
    m_solutions.clear();
    m_currentSolution = new Solution(name);
    m_solutions.append(m_currentSolution);
    return true;
}

bool Workspace::addProjectToSolution(Solution *s, Project *p)
{
    s->addProject(p);
    return true;
}

Version* Workspace::workingVersion() const
{
    Project* p = currentProject();
    if(!p) return 0;
    return currentProject()->getWorkingVersion();
}

bool Workspace::saveCurrentSolution(const QString &fileName)
{
    QDomDocument doc("DBM");
    QDomElement root = doc.createElement("Solutions");
    currentSolution()->serialize(doc, root);

    doc.appendChild(root);
    QString xml = doc.toString();

    if(fileName.length() > 0)
    {
        currentSolution()->setFile(fileName);
    }

    QFile f1(currentSolution()->savedFile());
    if (!f1.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return false;
    }
    f1.write(xml.toAscii());
    f1.close();

    save();

    return true;
}

bool Workspace::currentSolutionWasSavedAlready() const
{
    return currentSolution()->savedFile().length() != 0;
}

bool Workspace::onCloseSolution()
{
    m_solutions.clear();
    m_currentSolution = 0;
    save();

    return true;
}

bool Workspace::loadSolution(const QString& fileName)
{
    QDomDocument doc ("DBM");
    QFile file (fileName);
    if (!file.open(QIODevice::ReadOnly)) return false;
    if (!doc.setContent(&file))
    {
        file.close();
        return false;
    }
    file.close();

    QDomElement docElem = doc.documentElement();
    m_currentSolution = new Solution("");
    DeserializationFactory::createSolution(m_currentSolution, doc, docElem.firstChild().toElement());
    m_currentSolution->setFile(fileName);
    if(m_solutions.size() > 0)  // TODO: This is here only till we release a version which can support multiple solutions
    {
        m_solutions.clear();
    }
    m_solutions.append(m_currentSolution);

    return true;
}

Project* Workspace::currentProject() const
{
    Solution* s = currentSolution();
    if(!s) return 0;
    return currentSolution()->currentProject();
}

bool Workspace::currentProjectIsOop() const
{
    return currentProject()->oopProject();
}

Version* Workspace::getVersion(const QString &uid)
{
    return currentProject()->getVersion(uid);
}

DatabaseEngine* Workspace::currentProjectsEngine() const
{
    return currentProject()->getEngine();
}

QVector<UserDataType*> Workspace::loadDefaultDatatypesIntoCurrentSolution(Solution* s)
{
    QDomDocument doc ("DBM");
    QFile file (QApplication::applicationDirPath() + "/rsrc/" + Workspace::getInstance()->currentProjectsEngine()->getDefaultDatatypesLocation()); // TODO: This will not work for other databases :)
    if (file.open(QIODevice::ReadOnly))
    {
        if (!doc.setContent(&file))
        {
            file.close();
            return QVector<UserDataType*>();
        }
        file.close();

        QDomElement docElem = doc.documentElement();
        Solution* tempSolution = new Solution("");
        DeserializationFactory::createSolution(tempSolution, doc, docElem.firstChild().toElement());
        QVector<UserDataType*> dts = tempSolution->currentProject()->getWorkingVersion()->getDataTypes();

        for(int i=0; i<dts.size(); i++)        // add to the project itself
        {
            s->currentProject()->getWorkingVersion()->addNewDataType(dts.at(i), true);
            // make the working version of the "s" to be the version for dts[i] in UidWarehouse
            UidWarehouse::instance().setForcedVersionForUid(dts[i]->getObjectUid(), s->currentProject()->getWorkingVersion());
            dts[i]->setForcedVersion(s->currentProject()->getWorkingVersion());
        }

        return dts;
    }
    else
    {
        return QVector<UserDataType*>();
    }
}

Table* Workspace::pasteTable(Version* v)
{
    Table* tab = ClipboardFactory::pasteTable(v);
    if(tab != 0)
    {
        tab->setName(NameGenerator::getUniqueName(v, (itemGetter)&Version::getTable, tab->getName()+"_copy"));
        v->addTable(tab, false);
        return tab;
    }

    return 0;
}

bool Workspace::onUpdateTable(Table* tbl)
{
    QTreeWidgetItem* tblsItem = tbl->getLocation();
    QVariant var(tbl->getObjectUid());
    tblsItem->setData(0, Qt::UserRole, var);
    tblsItem->setText(0, tbl->getName());
    change();
    return true;
}

bool Workspace::onSaveNewTable(Table* tbl)
{
    tbl->getVersion()->getGui()->createTableTreeEntry(tbl);
    tbl->getVersion()->addTable(tbl, false);
    change();
    return true;
}

void Workspace::createNewConnection()
{
    InjectSqlDialog* injectDialog = new InjectSqlDialog(0, 0, 0, "");
    injectDialog->setModal(true);
    if(injectDialog->exec() == QDialog::Accepted)
    {
        if(MainWindow::instance()->getConnectionGuiElements())
        {
            if(!MainWindow::instance()->getConnectionGuiElements()->getConnectionsTreeDock()->isVisible())
            {
                MainWindow::instance()->showConnections();
            }
        }
        else
        {
            MainWindow::instance()->showConnections();
        }
        QString host = injectDialog->getHost();
        QString user = injectDialog->getUser();
        QString password = injectDialog->getPassword();
        QString db = injectDialog->getDatabase();
        QString name = injectDialog->getName();
        int port = injectDialog->getPort();
        Connection* c = new Connection(name, host, user, password, db, true, injectDialog->getAutoConnect(), port);
        ConnectionManager::instance()->addConnection(c);
        MainWindow::instance()->getConnectionGuiElements()->createConnectionTreeEntry(c);
    }
}

QStringList Workspace::getAllVersions()
{
    return m_currentSolution->currentProject()->getVersionNames();
}

bool Workspace::deleteDataType(UserDataType *udt)
{
    // now check that this stuff is not used in any of the tables and the delete it from the current version
    const QVector<Table*>& allTables = udt->version()->getTables();
    QString usage = "";
    for(int i=0; i<allTables.size(); i++)
    {
        const QVector<Column*> & tablesColumns = allTables.at(i)->getColumns();
        for(int j=0; j<tablesColumns.size(); j++)
        {
            QString cn = tablesColumns.at(j)->getDataType()->getName();
            if(cn == udt->getName())
            {
                usage += QObject::tr("Table: <b>") + allTables.at(i)->getName() + QObject::tr("</b> Column:<b>") + tablesColumns.at(j)->getName() + "</b><br><br>";
            }
        }
    }

    if(usage.length() > 0)
    {
        QMessageBox::critical(MainWindow::instance(),
                              QObject::tr("Error"),
                              QObject::tr("Cannot delete this data type since it's used in the following locations:<p>") + usage,
                              QMessageBox::Ok
                              );
        return false;
    }

    udt->version()->deleteDataType(udt->getName());
    return true;
}

bool Workspace::deployVersion(Version *v)
{
    InjectSqlDialog* injectDialog = new InjectSqlDialog(currentProjectsEngine(), MainWindow::instance(), v, "");
    injectDialog->setModal(true);
    if(injectDialog->exec() == QDialog::Accepted)
    {
        MainWindow::instance()->createStatusLabel();
        QStringList connectionNames = injectDialog->getSelectedConnections();
        DeploymentInitiator* dinit = new DeploymentInitiator();
        dinit->doDeployment(v, connectionNames,
                            injectDialog->injectMetadataRequired(),
                            injectDialog->getUidsToDeploy(),
                            injectDialog->getUidsToDrop());
    }
    return true;
}

bool Workspace::finalizePatch(Patch *p)
{
    Version* v = p->version();
    Project* prj = v->getProject();
    MajorVersion* newVersion = prj->createMajorVersion(v->getMajor(), v->getMinor() + 1);
    v->cloneInto(newVersion, LockableElement::FINAL_LOCK);
    newVersion->setSourceUid(v->getObjectUid());

    newVersion->createTreeItems(
                MainWindow::instance()->getGuiElements(),
                p->getLocation(),
                MainWindow::instance()->getGuiElements()->getProjectTree()->indexOfTopLevelItem(v->getLocation()) + 1
                );
    newVersion->getGui()->populateTreeItems();

    v->getGui()->getVersionItem()->setExpanded(false);
    newVersion->getGui()->getVersionItem()->setExpanded(true);
    newVersion->getGui()->getVersionItem()->setPopupMenu(ContextMenuCollection::getInstance()->getReleasedVersionPopupMenu());

    v->updateGui();

    v->getGui()->getVersionItem()->setPopupMenu(ContextMenuCollection::getInstance()->getFinalisedVersionPopupMenu());

    p->suspendPatch();

    v->lockVersion(LockableElement::FINAL_LOCK);
    newVersion->lockVersion(LockableElement::LOCKED);

    return true;
}

Trigger* Workspace::createTrigger(Version *v)
{
    QStringList names;
    if(currentProjectIsOop())
    {
        const QVector<TableInstance*>& allTables = v->getTableInstances();
        if(allTables.size() == 0)
        {
            QMessageBox::critical(MainWindow::instance(), QObject::tr("No tables defined"), QObject::tr("Cannot create a trigger when there are no table instances in an OOP project"), QMessageBox::Ok);
            return 0;
        }
        for(int i=0; i<allTables.size(); i++)
        {
            names.push_back(allTables[i]->getName());
        }
    }
    else
    {
        const QVector<Table*>& allTables = v->getTables();
        if(allTables.size() == 0)
        {
            QMessageBox::critical(MainWindow::instance(), QObject::tr("No tables defined"), QObject::tr("Cannot create a trigger when there are no tables"), QMessageBox::Ok);
            return 0;
        }
        for(int i=0; i<allTables.size(); i++)
        {
            names.push_back(allTables[i]->getName());
        }
    }
    TriggerForm* frm = v->getGui()->getTriggerForm();
    Trigger* trigger = new Trigger(NameGenerator::getUniqueName(v, (itemGetter)&Version::getTrigger, QString("trig")),
                                   QUuid::createUuid().toString(), v);
    frm->setTrigger(trigger);
    frm->initSql();
    frm->feedInTables(names);
    trigger->setTable(names[0]);
    frm->feedInTriggerEvents(Workspace::getInstance()->currentProjectsEngine()->getTriggerEvents());
    frm->feedInTriggerTimes(Workspace::getInstance()->currentProjectsEngine()->getTriggerTimings());
    v->addTrigger(trigger, false);
    v->getGui()->createTriggerTreeEntry(trigger);
    MainWindow::instance()->setCentralWidget(frm);
    return trigger;
}