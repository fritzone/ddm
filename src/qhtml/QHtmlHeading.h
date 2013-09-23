#ifndef QHTMLHEADING_H
#define QHTMLHEADING_H

#include "QHtmlElementContainer.h"

class QHtmlHeading : public QHtmlElementContainer
{
public:
    QHtmlHeading(int level) : m_level(level) {}
    virtual QString html() const
    {
        QString result = "<h" + QString::number(m_level) + ">";
        result += QHtmlElementContainer::html();
        result += "</h" + QString::number(m_level) + ">";
        return result;
    }

private:
    int m_level;

};

#endif // QHTMLHEADING_H
