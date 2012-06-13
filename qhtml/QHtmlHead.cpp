#include "QHtmlHead.h"

QHtmlHead::QHtmlHead(const QString &title) : m_title(title)
{
}

QString QHtmlHead::html() const
{
    QString result = "<head>";
    if(!m_title.isEmpty())
    {
        result += "<title>";
        result += m_title;
        result += "</title>";
    }
    result += "</head>";
    return result;
}

void QHtmlHead::setTitle(const QString &title)
{
    m_title = title;
}

QString QHtmlHead::getTitle() const
{
    return m_title;
}

