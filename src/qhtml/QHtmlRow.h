#ifndef QHTMLROW_H
#define QHTMLROW_H

#include <QVector>
#include "HtmlSourceGenerator.h"

class QHtmlBodyElements;

class QHtmlRow : public HtmlSourceGenerator
{
public:
    QHtmlRow();

    virtual QString html() const;
    void addData(QHtmlBodyElements* data)
    {
        m_tds.append(data);
    }

private:
    QVector<QHtmlBodyElements*> m_tds;
};

#endif // QHTMLROW_H
