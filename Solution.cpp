#include "Solution.h"

Solution::Solution(const QString & nm) : m_name(nm)
{
}

void Solution::serialize(QDomDocument &doc, QDomElement &parent) const
{
    QDomElement solutionElement = doc.createElement("Solution");
    solutionElement.setAttribute("Name", m_name);
    QDomElement projectsElement = doc.createElement("Projects");

    for(int i=0; i<m_projects.size(); i++)
    {
        m_projects[i]->serialize(doc, projectsElement);
    }
    solutionElement.appendChild(projectsElement);
    parent.appendChild(solutionElement);

}
