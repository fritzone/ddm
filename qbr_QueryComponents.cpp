#include "qbr_QueryComponents.h"

void QueryComponent::removeChild(QueryComponent *c)
{
    m_children.removeOne(c);
}

void QueryComponent::serialize(QDomDocument &doc, QDomElement &parent) const
{
    QDomElement component= doc.createElement("Children");
    parent.setAttribute("level", m_level);
    parent.setAttribute("class", getClass());
    parent.setAttribute("children", m_children.size());
    bool childCreated = false;
    for(int i=0; i<m_children.size(); i++)
    {
        QDomElement childAtI = doc.createElement("Child");
        childAtI.setAttribute("idx", i);
        m_children.at(i)->serialize(doc, childAtI);
        component.appendChild(childAtI);
        childCreated = true;
    }
    if(childCreated) parent.appendChild(component);
}
