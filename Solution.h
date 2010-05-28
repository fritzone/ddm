#ifndef SOLUTION_H
#define SOLUTION_H

#include "TreeItem.h"
#include "Project.h"
#include "SerializableElement.h"

#include <QVector>

class Solution : public TreeItem, public SerializableElement
{
public:
    Solution(const QString&);

    void addProject(Project* prj)
    {
        m_projects.append(prj);
    }

    Project* currentProject()
    {
        if(m_projects.size() > 0)
        {
            return m_projects[0];
        }

        return 0;
    }

    const QVector<Project*> & projects() const
    {
        return m_projects;
    }

    virtual void serialize(QDomDocument &doc, QDomElement &parent) const;

private:
    // the vector holding the projects
    QVector<Project*> m_projects;

    QString m_name;

};

#endif // SOLUTION_H
