#include "QHtmlTableHeader.h"

QHtmlTableHeader::QHtmlTableHeader(QHtmlBodyElements* data) : QHtmlBodyElements("th"), m_data(data)
{
}

QString QHtmlTableHeader::html() const
{
    QString result = openTag();
    result += m_data->html();
    result += closeTag();
    return result;
}
