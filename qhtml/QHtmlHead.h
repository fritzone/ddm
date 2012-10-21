#ifndef QHTMLHEAD_H
#define QHTMLHEAD_H

#include "HtmlSourceGenerator.h"

class QHtmlTitle;
class QHtmlCSSStyleSet;

class QHtmlHead : public HtmlSourceGenerator
{
    Q_OBJECT

public:
    QHtmlHead(const QString& title = "Untitled", const QHtmlCSSStyleSet* styleset = 0);
    virtual QString html() const;
    QString getTitle() const;
    void setTitle(const QString&);

private:

    QHtmlTitle* m_title;
    const QHtmlCSSStyleSet* m_styleSet;
};

#endif // QHTMLHEAD_H
