#include "Project.h"
#include "AbstractDTSupplier.h"
#include "MajorVersion.h"
#include "DatabaseEngine.h"
#include "IconFactory.h"

Project::Project(const QString& _name, QTreeWidget* _tree):m_tree(_tree), m_name(_name), m_engine(0), m_majorVersions(), m_prjDetailsForm(0)
{
    createTreeItem(m_tree);
}

Project::Project(const QString &_name) : m_tree(0), m_name(_name), m_engine(0), m_majorVersions(), m_prjDetailsForm(0)
{
}

void Project::createTreeItem(QTreeWidget* _tree)
{
    m_tree = _tree;

    QTreeWidgetItem* projectItem = new QTreeWidgetItem((QTreeWidget*)0, QStringList(m_name)) ;
    projectItem->setIcon(0, IconFactory::getProjectOpenIcon());

    QList<QTreeWidgetItem *> items;
    items.append(projectItem);
    m_tree->insertTopLevelItems(0, items);

    setLocation(projectItem);
}

void Project::populateTreeItem()
{
    for(int i=0; i<m_majorVersions.size(); i++)
    {
        m_majorVersions[i]->createTreeItems(m_tree, getLocation());
        m_majorVersions[i]->populateTreeItems();
    }
}

void Project::setEngine(DatabaseEngine* eng)
{
    m_engine = eng;
}

void Project::createMajorVersion()
{
    MajorVersion* mjw = new MajorVersion(m_tree, getLocation(), 1);
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
        return m_majorVersions[0];
    }

    return 0;
}

void Project::serialize(QDomDocument& doc, QDomElement& parent) const
{
    QDomElement projectElement = doc.createElement("Project");      // will hold the data in this element

    projectElement.setAttribute("Name", m_name);
    projectElement.setAttribute("DB", m_engine->getDatabase());

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
