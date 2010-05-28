#include "Project.h"
#include "AbstractDTSupplier.h"
#include "MajorVersion.h"
#include "DatabaseEngine.h"

Project::Project(const QString& _name, QTreeWidget* _tree):tree(_tree), name(_name)
{
    createTreeItem();
}

Project::Project(const QString &_name) : tree(0), name(_name)
{

}

void Project::createTreeItem()
{
    QIcon prjIcon(":/images/actions/images/small/project_open.png");
    QList<QTreeWidgetItem *> items;
    QTreeWidgetItem* projectItem = new QTreeWidgetItem((QTreeWidget*)0, QStringList(name)) ;
    projectItem->setIcon(0, prjIcon);
    items.append(projectItem);
    tree->insertTopLevelItems(0, items);

    setLocation(projectItem);
}

void Project::setEngine(DatabaseEngine* eng)
{
    this->engine = eng;
}

void Project::createMajorVersion()
{
    MajorVersion* mjw = new MajorVersion(tree, getLocation(), 1);
    majorVersions.append(mjw);
}

void Project::addMajorVersion(MajorVersion* mv)
{
    majorVersions.append(mv);
}

DatabaseEngine* Project::getEngine() const
{
    return engine;
}

Version* Project::getWorkingVersion() const
{
    if(majorVersions.size() > 0)
    {
        return majorVersions[0];
    }

    return 0;
}

void Project::serialize(QDomDocument& doc, QDomElement& parent) const
{
    QDomElement projectElement = doc.createElement("Project");      // will hold the data in this element

    projectElement.setAttribute("Name", name);

    // now saving the major versions of the project
    QDomElement majorVersionsElement = doc.createElement("MajorVersions");
    for(int i=0; i< majorVersions.size(); i++)
    {
        majorVersions[i]->serialize(doc, majorVersionsElement);
    }
    projectElement.appendChild(majorVersionsElement);

    // now serialize the tables

    parent.appendChild(projectElement);

}
