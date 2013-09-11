#include "QHtmlTable.h"
#include "QHtmlRow.h"
QHtmlTable::QHtmlTable(int border) : HtmlSourceGenerator("table"), m_border(border)
{
}

QString QHtmlTable::html() const
{
    QString result = "<table border=" + QString::number(m_border) + ">";
    for(int i=0; i<m_rows.size(); i++)
    {
        result += m_rows.at(i)->html();
    }
    result += closeTag();
    return result;
}
