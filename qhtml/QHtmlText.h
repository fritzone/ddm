#ifndef QHTMLTEXT_H
#define QHTMLTEXT_H

#include "QHtmlBodyElements.h"

class QHtmlText : public QHtmlBodyElements
{
public:
    QHtmlText(const QString& text);
    virtual QString html() const
    {
        return m_text;
    }

private:
    QString m_text;
};

#endif // QHTMLTEXT_H
