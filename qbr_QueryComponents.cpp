#include "qbr_QueryComponents.h"

void QueryComponent::removeChild(QueryComponent *c)
{
    m_children.removeOne(c);
}

void QueryComponent::serialize(QDomDocument &doc, QDomElement &parent) const
{
    QDomElement component= doc.createElement("Component");
    component.setAttribute("class", getClass());
    component.setAttribute("children", m_children.size());
    for(int i=0; i<m_children.size(); i++)
    {
        QDomElement childAtI = doc.createElement("Child");
        childAtI.setAttribute("idx", i);
        m_children.at(i)->serialize(doc, childAtI);
        component.appendChild(childAtI);
    }
    parent.appendChild(component);
}
