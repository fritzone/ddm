#include "QHtmlText.h"
#include "QHtmlCSSClass.h"

QHtmlText::QHtmlText(const QString &text, QHtmlCSSClass *cls) : QHtmlBodyElements(""), m_text(text), m_class(cls)
{
}

QString QHtmlText::html() const
{
    if(m_class)
    {
        return m_class->renderFor(this);
    }
    return m_text;
}
