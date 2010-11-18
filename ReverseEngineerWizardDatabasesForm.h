#ifndef REVERSEENGINEERWIZARDDATABASESFORM_H
#define REVERSEENGINEERWIZARDDATABASESFORM_H

#include <QWizardPage>

namespace Ui {
    class ReverseEngineerWizardDatabasesForm;
}

class ReverseEngineerWizardDatabasesForm : public QWizardPage
{
    Q_OBJECT

public:
    explicit ReverseEngineerWizardDatabasesForm(QWidget *parent = 0);
    ~ReverseEngineerWizardDatabasesForm();

    void clearList();
    void addDatabase(const QString&);
    QString getSelectedDatabase() const;

private:
    Ui::ReverseEngineerWizardDatabasesForm *ui;
};

#endif // REVERSEENGINEERWIZARDDATABASESFORM_H
