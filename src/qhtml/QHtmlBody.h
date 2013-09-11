#ifndef QHTMLBODY_H
#define QHTMLBODY_H

#include "QHtmlElementContainer.h"
#include "QHtmlHeading.h"

class QHtmlHeading;
class QHtmlBodyElements;

class QHtmlBody : public QHtmlElementContainer
{
public:
    QHtmlBody();
    virtual QString html() const
    {
        QString result = "<body>";
        result += QHtmlElementContainer::html();
        result += "</body>";
        return result;
    }
    void addHeading(QHtmlHeading* h)
    {
        addElement(h);
    }

};

#endif // QHTMLBODY_H
