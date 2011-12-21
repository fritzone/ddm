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

    enum Mode
    {
        REVERSE_ENGINEER_TABLES = 0,
        REVERSE_ENGINEER_VIEWS = 1
    } ;

    explicit ReverseEngineerWizardTablesForm(QWidget *parent = 0, Mode t = REVERSE_ENGINEER_TABLES);
    ~ReverseEngineerWizardTablesForm();

    void addTable(const QString& tab);
    void clearList();
    QStringList getSelectedItems();

public slots:

    void checkStateChanged(int);

private:
    Ui::ReverseEngineerWizardTablesForm *ui;
    Mode m_mode;
};

#endif // REVERSEENGINEERWIZARDTABLESFORM_H
