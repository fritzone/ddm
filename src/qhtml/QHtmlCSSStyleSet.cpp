#include "QHtmlCSSStyleSet.h"
#include <QTextStream>
#include <QFile>

QHtmlCSSStyleSet::QHtmlCSSStyleSet(const QString& fileName)
{
    QFile f(fileName);
    if(!f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    QTextStream in(&f);
    while (!in.atEnd()) {
        QString line = in.readLine();
        m_styleSet += line;
    }
}
