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

private:
    Ui::ReverseEngineerWizardWelcomeForm *ui;
};

#endif // REVERSEENGINEERWIZARDWELCOMEFORM_H
