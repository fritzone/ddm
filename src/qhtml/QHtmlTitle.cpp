#include "QHtmlTitle.h"

QHtmlTitle::QHtmlTitle(const QString &title) : HtmlSourceGenerator("title"),
    m_title(title)
{
}

QString QHtmlTitle::html() const
{
    return openTag() + m_title + closeTag();
}

void QHtmlTitle::setTitle(const QString &title)
{
    m_title = title;
}

QString QHtmlTitle::getTitle() const
{
    return m_title;
}


