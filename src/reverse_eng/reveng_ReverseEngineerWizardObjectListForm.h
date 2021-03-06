#ifndef REVERSEENGINEERWIZARDTABLESFORM_H
#define REVERSEENGINEERWIZARDTABLESFORM_H

#include <QWizardPage>

namespace Ui {
    class ReverseEngineerWizardObjectListForm;
}

class ReverseEngineerWizardObjectListForm :public QWizardPage
{
    Q_OBJECT

public:

    enum Mode
    {
        REVERSE_ENGINEER_TABLES = 0,
        REVERSE_ENGINEER_VIEWS = 1,
        REVERSE_ENGINEER_PROCS = 2,
        REVERSE_ENGINEER_FUNCS = 3,
        REVERSE_ENGINEER_TRIGGERS = 4
    } ;

    explicit ReverseEngineerWizardObjectListForm(QWidget *parent = 0, Mode t = REVERSE_ENGINEER_TABLES);
    ~ReverseEngineerWizardObjectListForm();

    void addObject(const QString& tab);
    void clearList();
    QStringList getSelectedItems();

public slots:

    void checkStateChanged(int);

private:
    Ui::ReverseEngineerWizardObjectListForm *ui;
    Mode m_mode;
};

#endif // REVERSEENGINEERWIZARDTABLESFORM_H
