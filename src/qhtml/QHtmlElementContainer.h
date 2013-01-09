#ifndef QHTMLELEMENTCONTAINER_H
#define QHTMLELEMENTCONTAINER_H

#include "HtmlSourceGenerator.h"
#include <QVector>

class QHtmlElementContainer : public HtmlSourceGenerator
{
    Q_OBJECT

public:
    QHtmlElementContainer();
    virtual QString html() const;
    void addElement(HtmlSourceGenerator* elem);

private:
    QVector<HtmlSourceGenerator*> m_elements;

};

#endif // QHTMLELEMENTCONTAINER_H
