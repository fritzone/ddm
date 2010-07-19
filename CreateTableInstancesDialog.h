#ifndef CREATETABLEINSTANCESDIALOG_H
#define CREATETABLEINSTANCESDIALOG_H

#include <QDialog>

namespace Ui {
    class CreateTableInstancesDialog;
}

class CreateTableInstancesDialog : public QDialog {
    Q_OBJECT
public:
    CreateTableInstancesDialog(QWidget *parent = 0);
    ~CreateTableInstancesDialog();

    void addTable(const QString& tabName);

    QStringList getSelectedTables();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::CreateTableInstancesDialog *ui;
};

#endif // CREATETABLEINSTANCESDIALOG_H
