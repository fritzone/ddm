#ifndef QHTMLBREAK_H
#define QHTMLBREAK_H

#include "HtmlSourceGenerator.h"

class QHtmlBreak : public HtmlSourceGenerator
{
public:
    QHtmlBreak();
    virtual QString html() const {return "<br>";}
};

#endif // QHTMLBREAK_H
