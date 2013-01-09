#ifndef QHTMLTEXT_H
#define QHTMLTEXT_H

#include "QHtmlBodyElements.h"

class QHtmlCSSClass;

class QHtmlText : public QHtmlBodyElements
{
public:
    QHtmlText(const QString& text, QHtmlCSSClass* cls = 0);
    virtual QString html() const;
    QString text() const
    {
        return m_text;
    }
private:
    QString m_text;
    QHtmlCSSClass* m_class;
};

#endif // QHTMLTEXT_H
