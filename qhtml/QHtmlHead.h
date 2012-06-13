#ifndef QHTMLHEAD_H
#define QHTMLHEAD_H

#include "HtmlSourceGenerator.h"

class QHtmlHead : public HtmlSourceGenerator
{
public:
    QHtmlHead(const QString& title);

    virtual QString html() const;

    QString getTitle() const;
    void setTitle(const QString&);

private:

    QString m_title;
};

#endif // QHTMLHEAD_H
