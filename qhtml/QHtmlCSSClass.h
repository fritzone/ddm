#ifndef QHTMLCSSCLASS_H
#define QHTMLCSSCLASS_H

#include "QHtmlBodyElements.h"
#include <QMap>

class QHtmlText;

class QHtmlCSSClass
{
public:
    QHtmlCSSClass(const QString& name);

    static QHtmlCSSClass* classProjectDescription();
    static QHtmlCSSClass* classDescription();
    static QHtmlCSSClass* classTableHeader();
    static QHtmlCSSClass* classTODO();
    static QHtmlCSSClass* classTableText();
    static QHtmlCSSClass* classCode();

    QString renderFor(const QHtmlText *) const;
    void addAttribute(const QString&n, const QString&v)
    {
        m_attributes[n] = v;
    }

private:
    QMap<QString, QString> m_attributes;
    QString m_className;
};

#endif // QHTMLCSSCLASS_H
