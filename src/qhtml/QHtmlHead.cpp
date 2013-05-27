#include "QHtmlHead.h"
#include "QHtmlTitle.h"
#include "QHtmlCSSStyleSet.h"

QHtmlHead::QHtmlHead(const QString &title, const QHtmlCSSStyleSet *styleset) : HtmlSourceGenerator("head"),
    m_title(new QHtmlTitle(title)), m_styleSet(styleset)
{
}

QString QHtmlHead::html() const
{
    return openTag() + m_title->html() +
                (m_styleSet?("<style type=\"text/css\">"+ m_styleSet->get() +"</style>"):"")
                + closeTag();
}

void QHtmlHead::setTitle(const QString &title)
{
    m_title->setTitle(title);
}

QString QHtmlHead::getTitle() const
{
    return m_title->getTitle();
}

