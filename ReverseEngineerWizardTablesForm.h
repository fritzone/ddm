#ifndef REVERSEENGINEERWIZARDTABLESFORM_H
#define REVERSEENGINEERWIZARDTABLESFORM_H

#include <QWizardPage>

namespace Ui {
    class ReverseEngineerWizardTablesForm;
}

class ReverseEngineerWizardTablesForm :public QWizardPage
{
    Q_OBJECT

public:
    explicit ReverseEngineerWizardTablesForm(QWidget *parent = 0);
    ~ReverseEngineerWizardTablesForm();

    void addTable(const QString& tab);
    void clearList();
    QVector<QString> getSelectedTables();

public slots:

    void checkStateChanged(int);

private:
    Ui::ReverseEngineerWizardTablesForm *ui;
};

#endif // REVERSEENGINEERWIZARDTABLESFORM_H
