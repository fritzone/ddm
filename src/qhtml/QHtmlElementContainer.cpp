#include "QHtmlElementContainer.h"
#include <QDebug>

QString QHtmlElementContainer::html() const
{
    QString result;
    for(int i=0; i<m_elements.size(); i++)
    {
        result += m_elements.at(i)->html();
    }
    return result;
}

QHtmlElementContainer::QHtmlElementContainer() : HtmlSourceGenerator(""), m_elements()
{}

void QHtmlElementContainer::addElement(HtmlSourceGenerator* elem)
{
    m_elements.append(elem);
}
