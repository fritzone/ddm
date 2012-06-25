#ifndef QHTMLCSSCLASS_H
#define QHTMLCSSCLASS_H

#include "QHtmlBodyElements.h"
#include <QMap>

class QHtmlText;

class QHtmlCSSClass
{
public:
    QHtmlCSSClass();

    static QHtmlCSSClass* classH1();
    static QHtmlCSSClass* classH2() {return 0;}
    static QHtmlCSSClass* classH3() {return 0;}
    static QHtmlCSSClass* classH4() {return 0;}
    static QHtmlCSSClass* classH5() {return 0;}
    static QHtmlCSSClass* classH6() {return 0;}

    static QHtmlCSSClass* classProjectDescription() {return 0;}
    static QHtmlCSSClass* classDescription() {return 0;}
    static QHtmlCSSClass* classTableHeader() {return 0;}
    static QHtmlCSSClass* classTableText() {return 0;}
    static QHtmlCSSClass* classCode() {return 0;}

    QString renderFor(const QHtmlText *) const;
    void addAttribute(const QString&n, const QString&v)
    {
        m_attributes[n] = v;
    }

private:
    QMap<QString, QString> m_attributes;
};

#endif // QHTMLCSSCLASS_H
