#ifndef REVERSEENGINEERWIZARDWELCOMEFORM_H
#define REVERSEENGINEERWIZARDWELCOMEFORM_H

#include <QWizardPage>

namespace Ui {
    class ReverseEngineerWizardWelcomeForm;
}

class ReverseEngineerWizardWelcomeForm : public QWizardPage
{
    Q_OBJECT

public:
    explicit ReverseEngineerWizardWelcomeForm(QWidget *parent = 0);
    ~ReverseEngineerWizardWelcomeForm();

    QString getHost();
    QString getUser();
    QString getPassword();
    int getPort();
    QString getDatabase();

    QString getSqliteFile();
    int getSqliteVersion();

    void setMySqlMode();
    void setSqliteMode();
    void setCubridMode();
    void setPostgresqlMode();

private slots:

    void onBrowseFile();

private:
    Ui::ReverseEngineerWizardWelcomeForm *ui;
    QString m_mode;
};

#endif // REVERSEENGINEERWIZARDWELCOMEFORM_H
