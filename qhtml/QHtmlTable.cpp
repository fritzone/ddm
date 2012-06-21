#include "QHtmlTable.h"
#include "QHtmlRow.h"
QHtmlTable::QHtmlTable() : HtmlSourceGenerator("table")
{
}

QString QHtmlTable::html() const
{
    QString result = openTag() ;
    for(int i=0; i<m_rows.size(); i++)
    {
        result += m_rows.at(i)->html();
    }
    result += closeTag();
    return result;
}
