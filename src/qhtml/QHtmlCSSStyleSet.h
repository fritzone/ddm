#ifndef QHTMLCSSSTYLESET_H
#define QHTMLCSSSTYLESET_H

#include <QString>
#include <QMap>
#include <QVector>

class QHtmlCSSStyleSet
{
public:

    struct StyleSet
    {
        QString m_name;
        QMap<QString, QString> m_values;
    };

    QHtmlCSSStyleSet(const QString& fileName);
    QString render() const;
    QString get() const
    {
        return m_styleSet;
    }

private:
    QVector<StyleSet> m_styles;
    QString m_styleSet;
};

#endif // QHTMLCSSSTYLESET_H
