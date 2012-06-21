#include "QHtmlTable.h"

QHtmlTable::QHtmlTable() : HtmlSourceGenerator("table")
{
}

QString QHtmlTable::html() const
{
    QString result = openTag() ;
    result += closeTag();
    return result;
}
