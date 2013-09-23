#ifndef SOLUTION_H
#define SOLUTION_H

#include "TreeItem.h"
#include "Project.h"
#include "core_SerializableElement.h"
#include "core_NamedItem.h"

#include <QVector>

class Solution : public NamedItem, public TreeItem, public SerializableElement
{
public:
    Solution(const QString&);

    void addProject(Project* prj)
    {
        m_projects.append(prj);
    }

    Project* currentProject() const
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

    void setFile(const QString& f)
    {
        savedToFile = f;
    }

    QString savedFile() const
    {
        return savedToFile;
    }

private:
    // the vector holding the projects
    QVector<Project*> m_projects;

    QString savedToFile;

};

#endif // SOLUTION_H
