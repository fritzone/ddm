#include "QHtmlHead.h"
#include "QHtmlTitle.h"

QHtmlHead::QHtmlHead(const QString &title) : HtmlSourceGenerator("head"),
    m_title(new QHtmlTitle(title))
{
}

QString QHtmlHead::html() const
{
    return openTag() + m_title->html() + closeTag();
}

void QHtmlHead::setTitle(const QString &title)
{
    m_title->setTitle(title);
}

QString QHtmlHead::getTitle() const
{
    return m_title->getTitle();
}

