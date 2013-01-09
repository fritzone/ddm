#ifndef QHTMLTABLE_H
#define QHTMLTABLE_H

#include "HtmlSourceGenerator.h"
#include <QVector>

class QHtmlRow;

class QHtmlTable : public HtmlSourceGenerator
{
public:
    QHtmlTable(int border);
    virtual QString html() const;
    void addRow(QHtmlRow* row)
    {
        m_rows.append(row);
    }

private:
    QVector<QHtmlRow*> m_rows;
    int m_border;
};

#endif // QHTMLTABLE_H
