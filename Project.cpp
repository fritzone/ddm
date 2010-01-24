#include "Project.h"
#include "AbstractDTSupplier.h"
#include "MajorVersion.h"
#include "DatabaseEngine.h"

Project::Project(const QString& _name, QTreeWidget* _tree):tree(_tree)
	, name(_name)
{
    QIcon prjIcon(":/images/actions/images/small/project_open.png");
    QList<QTreeWidgetItem *> items;
    projectItem = new QTreeWidgetItem((QTreeWidget*)0, QStringList(name)) ;
    projectItem->setIcon(0, prjIcon);
    items.append(projectItem);
    tree->insertTopLevelItems(0, items);
}

void Project::setEngine(DatabaseEngine* eng)
{
    this->engine = eng;
}

void Project::createMajorVersion()
{

    MajorVersion* mjw = new MajorVersion(tree, projectItem, 1);
    majorVersions.append(mjw);
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
