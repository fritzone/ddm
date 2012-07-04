#ifndef DOCUMENTATIONFORM_H
#define DOCUMENTATIONFORM_H

#include <QWidget>

namespace Ui {
class DocumentationForm;
}

class DocumentationGenerator;
class QHtmlCSSStyleSet;

class DocumentationForm : public QWidget
{
    Q_OBJECT

public:
    explicit DocumentationForm(QWidget *parent = 0);
    ~DocumentationForm();

    void setDoc(const QString& html);
    void setUid(const QString& uid)
    {
        m_uidShown = uid;
    }
    void showDocumentationforUid(const QString &guid, QHtmlCSSStyleSet *css = 0);
    void initiateStyleChange(QString);

public slots:
    void styleChanged(QString);
    void onSave();
    void onPrint();

private:
    Ui::DocumentationForm *ui;
    QString m_uidShown;

public:
    static QString s_lastStyle;

    static QStringList s_openedStyles;
};

#endif // DOCUMENTATIONFORM_H
