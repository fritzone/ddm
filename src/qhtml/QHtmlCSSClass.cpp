#include "QHtmlCSSClass.h"
#include "QHtmlText.h"

QHtmlCSSClass::QHtmlCSSClass(const QString &name) : m_className(name)
{}

QString QHtmlCSSClass::renderFor(const QHtmlText* txt) const
{
    QString result = "<span class = \"" + m_className + "\">";
    result += txt->text();
    result += "</span>";
    return result;
}

QHtmlCSSClass* QHtmlCSSClass::classProjectDescription()
{
    QHtmlCSSClass* ch1 = new QHtmlCSSClass("classProjectDescription");
    return ch1;
}

QHtmlCSSClass* QHtmlCSSClass::classDescription()
{
    QHtmlCSSClass* ch1 = new QHtmlCSSClass("classDescription");
    return ch1;
}

QHtmlCSSClass* QHtmlCSSClass::classTableHeader()
{
    QHtmlCSSClass* ch1 = new QHtmlCSSClass("classTableHeader");
    return ch1;
}

QHtmlCSSClass* QHtmlCSSClass::classTODO()
{
    QHtmlCSSClass* ch1 = new QHtmlCSSClass("classTODO");
    return ch1;
}

QHtmlCSSClass* QHtmlCSSClass::classTableText()
{
    QHtmlCSSClass* ch1 = new QHtmlCSSClass("classTableText");
    return ch1;
}

QHtmlCSSClass* QHtmlCSSClass::classCode()
{
    QHtmlCSSClass* ch1 = new QHtmlCSSClass("classCode");
    return ch1;
}
