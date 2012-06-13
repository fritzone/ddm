#ifndef HTMLSOURCEGENERATOR_H
#define HTMLSOURCEGENERATOR_H

#include <QString>

class HtmlSourceGenerator
{
public:
    HtmlSourceGenerator();

    virtual QString html() const = 0;
};

#endif // HTMLSOURCEGENERATOR_H
