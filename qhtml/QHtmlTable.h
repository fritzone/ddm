#ifndef QHTMLTABLE_H
#define QHTMLTABLE_H

#include "HtmlSourceGenerator.h"

class QHtmlTable : public HtmlSourceGenerator
{
public:
    QHtmlTable();
    virtual QString html() const;
};

#endif // QHTMLTABLE_H
