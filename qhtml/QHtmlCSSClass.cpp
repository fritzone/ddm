#include "QHtmlCSSClass.h"
#include "QHtmlText.h"

QHtmlCSSClass::QHtmlCSSClass()
{}

QString QHtmlCSSClass::renderFor(const QHtmlText* txt) const
{
    QString result = "<span";
    for(int i=0; i<m_attributes.keys().size(); i++)
    {
        if(i == 0) result += " style=\"";
        result += m_attributes.keys().at(i) + ":" + m_attributes[m_attributes.keys().at(i)];
        if(i < m_attributes.keys().size() - 1) result += ";";
    }
    result += "\">";
    result += txt->text();
    result += "</span>";
    return result;
}

QHtmlCSSClass* QHtmlCSSClass::classH1()
{
    QHtmlCSSClass* ch1 = new QHtmlCSSClass();
    ch1->addAttribute("color", "blue");
    ch1->addAttribute("font-size", "14pt");
    return 0;
}
