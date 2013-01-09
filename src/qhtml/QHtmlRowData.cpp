#include "QHtmlRowData.h"
#include "QHtmlBodyElements.h"
QHtmlRowData::QHtmlRowData(QHtmlBodyElements *data) : QHtmlBodyElements("td"), m_data(data)
{
}

QString QHtmlRowData::html() const
{
    QString result = openTag();
    result += m_data->html();
    result += closeTag();
    return result;
}
