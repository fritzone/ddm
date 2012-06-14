#include "QHtmlDocument.h"
#include "QHtmlHead.h"

QHtmlDocument::QHtmlDocument(const QString &title, DocumentType type) :
    HtmlSourceGenerator("html"),
    m_documentType(type),
    m_head(new QHtmlHead(title))
{
}

QString QHtmlDocument::html() const
{
    QString typeString = "";
    switch(m_documentType)
    {
    case HTML_5:
        typeString = "html";
        break;
    case HTML_401_STRICT:
        typeString = "HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\"";
        break;
    case HTML_401_TRANSITIONAL:
        typeString = "HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\"";
        break;
    case HTML_401_FRAMESET:
        typeString = "HTML PUBLIC \"-//W3C//DTD HTML 4.01 Frameset//EN\" \"http://www.w3.org/TR/html4/frameset.dtd\"";
        break;
    case XHTML_10_STRICT:
        typeString = "html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\"";
        break;
    case XHTML_10_TRANSTIONAL:
        typeString = "html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\"";
        break;
    case XHTML_10_FRAMESET:
        typeString = "html PUBLIC \"-//W3C//DTD XHTML 1.0 Frameset//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-frameset.dtd\"";
        break;
    case XHTML_11:
        typeString = "html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\"";
        break;
    }

    QString result = "<!DOCTYPE " + typeString + ">";
    result += openTag();
    result += m_head->html();
    result += closeTag();
    return result;
}

