#include "Project.h"
#include "db_AbstractDTSupplier.h"
#include "MajorVersion.h"
#include "db_DatabaseEngine.h"
#include "IconFactory.h"
#include "VersionGuiElements.h"
#include "GuiElements.h"

Project::Project(const QString &_name, bool oop) : NamedItem(_name), m_engine(0), m_majorVersions(), m_oopIsEnabled(oop), m_workingVersionIndex(0)
{
}

void Project::createTreeItem(GuiElements* gui)
{
    ContextMenuEnabledTreeWidgetItem* projectItem = new ContextMenuEnabledTreeWidgetItem((ContextMenuEnabledTreeWidgetItem*)0, QStringList(m_name)) ;
    projectItem->setIcon(0, IconFactory::getProjectOpenIcon());
    gui->getProjectTree()->insertTopLevelItem(0, projectItem);
    setLocation(projectItem);
}

void Project::populateTreeItem(GuiElements* gui)
{
    for(int i=0; i<m_majorVersions.size(); i++)
    {
        m_majorVersions[i]->createTreeItems(gui, getLocation());
        m_majorVersions[i]->getGui()->populateTreeItems();
    }
}

void Project::setEngine(DatabaseEngine* eng)
{
    m_engine = eng;
}

void Project::createMajorVersion(int major, int minor)
{
    MajorVersion* mjw = new MajorVersion(major, minor, this);
    m_majorVersions.append(mjw);
}

void Project::addMajorVersion(MajorVersion* mv)
{
    m_majorVersions.append(mv);
}

DatabaseEngine* Project::getEngine() const
{
    return m_engine;
}

Version* Project::getWorkingVersion() const
{
    if(m_majorVersions.size() > 0)
    {
        return m_majorVersions[m_workingVersionIndex];
    }
    return 0;
}

Version* Project::getVersionNamed(const QString &a) const
{
    if(m_majorVersions.size() > 0)
    {
        for(int i=0; i<m_majorVersions.size(); i++)
        {
            if(m_majorVersions.at(i)->getVersionText() == a)
            {
                return m_majorVersions[i];
            }
        }
    }

    return 0;
}

void Project::serialize(QDomDocument& doc, QDomElement& parent) const
{
    QDomElement projectElement = doc.createElement("Project");      // will hold the data in this element

    projectElement.setAttribute("Name", m_name);
    projectElement.setAttribute("OOP", m_oopIsEnabled);
    projectElement.setAttribute("DB", m_engine->getDatabaseEngineName());

    {
    QDomElement descElement = doc.createElement("Description");  // description
    QDomText descNode = doc.createTextNode(getDescription());
    descElement.appendChild(descNode);
    projectElement.appendChild(descElement);
    }

    // now saving the major versions of the project
    QDomElement majorVersionsElement = doc.createElement("MajorVersions");
    for(int i=0; i< m_majorVersions.size(); i++)
    {
        m_majorVersions[i]->serialize(doc, majorVersionsElement);
    }
    projectElement.appendChild(majorVersionsElement);

    // now serialize the tables

    parent.appendChild(projectElement);

}

void Project::releaseMajorVersion()
{
    Version* cv = getWorkingVersion();
    createMajorVersion(cv->getMajor() + 1, 0);
    m_workingVersionIndex ++;
}
