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

QVector<UserDataType*> Workspace::loadDefaultDatatypesIntoCurrentSolution()
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
            Workspace::getInstance()->workingVersion()->addNewDataType(dts.at(i), true);
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
    InjectSqlDialog* injectDialog = new InjectSqlDialog(0, 0, 0);
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
