#ifndef QHTMLTITLE_H
#define QHTMLTITLE_H

#include "HtmlSourceGenerator.h"

#include <QObject>
#include <QString>

class QHtmlTitle : public HtmlSourceGenerator
{
    Q_OBJECT
public:
    QHtmlTitle(const QString& title = "Untitled");
    virtual QString html() const;

    QString getTitle() const;
    void setTitle(const QString&);
private:
    QString m_title;

};

#endif // QHTMLTITLE_H
