#ifndef DOCUMENTATIONFORM_H
#define DOCUMENTATIONFORM_H

#include <QWidget>

namespace Ui {
class DocumentationForm;
}

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

public slots:
    void styleChanged(QString);
    void onSave();
    void onPrint();

private:
    Ui::DocumentationForm *ui;
    QString m_uidShown;
};

#endif // DOCUMENTATIONFORM_H
