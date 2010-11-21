#ifndef REVERSEENGINEERWIZARDOPTIONSFORM_H
#define REVERSEENGINEERWIZARDOPTIONSFORM_H

#include <QWizardPage>

namespace Ui {
    class ReverseEngineerWizardOptionsForm;
}

class ReverseEngineerWizardOptionsForm : public QWizardPage
{
    Q_OBJECT

public:
    explicit ReverseEngineerWizardOptionsForm(QWidget *parent = 0);
    ~ReverseEngineerWizardOptionsForm();
    bool createDataTypesForColumns();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::ReverseEngineerWizardOptionsForm *ui;
};

#endif // REVERSEENGINEERWIZARDOPTIONSFORM_H
