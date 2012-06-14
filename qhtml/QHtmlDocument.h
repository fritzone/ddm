#ifndef QHTMLDOCUMENT_H
#define QHTMLDOCUMENT_H

#include "HtmlSourceGenerator.h"
#include <QObject>

class QHtmlHead;

class QHtmlDocument : public HtmlSourceGenerator
{
    Q_OBJECT
public:
    enum DocumentType
    {
        HTML_5                      = 1,
        HTML_401_STRICT             = 2,
        HTML_401_TRANSITIONAL       = 3,
        HTML_401_FRAMESET           = 4,
        XHTML_10_STRICT             = 5,
        XHTML_10_TRANSTIONAL        = 6,
        XHTML_10_FRAMESET           = 7,
        XHTML_11                    = 8
    };

    QHtmlDocument(const QString& title, DocumentType type = HTML_5);
    virtual ~QHtmlDocument() {}
    virtual QString html() const;

    QHtmlHead* getHead() const
    {
        return m_head;
    }

private:

    DocumentType m_documentType;
    QHtmlHead* m_head;
};

#endif // QHTMLDOCUMENT_H
