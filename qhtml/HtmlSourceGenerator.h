#ifndef HTMLSOURCEGENERATOR_H
#define HTMLSOURCEGENERATOR_H

#include <QString>
#include <QObject>

#include "QHtmlAttributes.h"

class HtmlSourceGenerator : public QObject
{
    Q_OBJECT

public:
    HtmlSourceGenerator(const QString& tag);

    virtual QString html() const = 0;
    virtual QString serialize_attributes() const
    {
        return "";
    }

    QString openTag() const
    {
        return "<" + m_tag + serialize_attributes() + ">";
    }

    QString closeTag() const
    {
        return "</" + m_tag + ">";
    }

private:
    QString m_tag;
};

#endif // HTMLSOURCEGENERATOR_H
