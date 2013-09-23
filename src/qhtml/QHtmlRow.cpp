#include "QHtmlRow.h"
#include "QHtmlRowData.h"

QHtmlRow::QHtmlRow() : HtmlSourceGenerator("tr"), m_tds()
{
}

QString QHtmlRow::html() const
{
    QString result = openTag();
    for(int i=0; i<m_tds.size(); i++)
    {
        result += m_tds.at(i)->html();
    }
    result += closeTag();
    return result;
}

