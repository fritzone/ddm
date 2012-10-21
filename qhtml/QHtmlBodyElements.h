#ifndef QHTMLBODYELEMENTS_H
#define QHTMLBODYELEMENTS_H

#include "HtmlSourceGenerator.h"

class QHtmlBodyElements : public HtmlSourceGenerator
{
public:
    QHtmlBodyElements(const QString& x) : HtmlSourceGenerator(x){}
    virtual QString html() const = 0;
};

#endif // QHTMLBODYELEMENTS_H
