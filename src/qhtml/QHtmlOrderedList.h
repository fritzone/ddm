#ifndef QHTMLORDEREDLIST_H
#define QHTMLORDEREDLIST_H

#include "HtmlSourceGenerator.h"

class QHtmlOrderedList : public HtmlSourceGenerator
{
public:
    QHtmlOrderedList();
    virtual QString html() const
    {
        return "";
    }
};

#endif // QHTMLORDEREDLIST_H
