#ifndef QHTMLROW_H
#define QHTMLROW_H

#include <QStringList>
#include "HtmlSourceGenerator.h"

class QHtmlRow : public HtmlSourceGenerator
{
public:
    QHtmlRow();

    virtual QString html() const;
    void addRowData(const QString& a)
    {
        m_tds.append(a);
    }

private:
    QStringList m_tds;
};

#endif // QHTMLROW_H
