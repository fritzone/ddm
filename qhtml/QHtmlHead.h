#ifndef QHTMLHEAD_H
#define QHTMLHEAD_H

#include "HtmlSourceGenerator.h"

class QHtmlTitle;

class QHtmlHead : public HtmlSourceGenerator
{
    Q_OBJECT

public:
    QHtmlHead(const QString& title = "Untitled");
    virtual QString html() const;
    QString getTitle() const;
    void setTitle(const QString&);

private:

    QHtmlTitle* m_title;
};

#endif // QHTMLHEAD_H
