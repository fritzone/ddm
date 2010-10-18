#include "Workspace.h"
#include "Solution.h"
#include "Project.h"
#include "DeserializationFactory.h"
#include "ContextMenuCollection.h"
#include "DatabaseEngine.h"
#include "Version.h"
#include "UserDataType.h"

#include <QFile>
#include <QApplication>

Workspace* Workspace::m_instance = 0;

Workspace::Workspace() : m_solutions(), m_currentSolution(0)
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
    m_currentSolution = DeserializationFactory::createSolution(doc, docElem.firstChild().toElement());
    m_currentSolution->setFile(fileName);

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
        Solution* tempSolution = DeserializationFactory::createSolution(doc, docElem.firstChild().toElement());
        QVector<UserDataType*> dts = tempSolution->currentProject()->getWorkingVersion()->getDataTypes();


        for(int i=0; i<dts.size(); i++)        // add to the project itself
        {
            Workspace::getInstance()->workingVersion()->addNewDataType(dts.at(i));
        }

        return dts;
    }
    else
    {
        return QVector<UserDataType*>();
    }
}
