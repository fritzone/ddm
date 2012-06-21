#ifndef QHTMLTABLE_H
#define QHTMLTABLE_H

#include "HtmlSourceGenerator.h"
#include <QVector>

class QHtmlRow;

class QHtmlTable : public HtmlSourceGenerator
{
public:
    QHtmlTable();
    virtual QString html() const;
    void addRow(QHtmlRow* row)
    {
        m_rows.append(row);
    }

private:
    QVector<QHtmlRow*> m_rows;
};

#endif // QHTMLTABLE_H
