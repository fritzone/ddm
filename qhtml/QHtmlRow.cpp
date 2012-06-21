#include "QHtmlRow.h"

QHtmlRow::QHtmlRow() : HtmlSourceGenerator("tr"), m_tds()
{
}

QString QHtmlRow::html() const
{
    QString result = openTag();
    for(int i=0; i<m_tds.size(); i++)
    {
        result += "<td>" + m_tds.at(i) + "</td>";
    }
    result += closeTag();
    return result;
}

