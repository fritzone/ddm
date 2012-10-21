#ifndef TABLEINSTANCEFORM_H
#define TABLEINSTANCEFORM_H

#include <QWidget>

class TableInstance;

namespace Ui {
    class TableInstanceForm;
}

class TableInstanceForm : public QWidget {
    Q_OBJECT
public:
    TableInstanceForm(QWidget *parent = 0);
    ~TableInstanceForm();

    void setTableInstance(TableInstance* st);

    void createTableWithValues();

public slots:
    void onLoadValuesFromCSV();
    void onSaveValuesToCSV();
    void onAddNewRow();
    void onDeleteRow();
    void onValidateData();
    void onCellChange(int,int);
    void onHelp();
    void onValuesDoubleClick();
    void onGotoTable();
    void onLockUnlock(bool);
    void onUndelete();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::TableInstanceForm *ui;
    TableInstance* m_tinst;
    bool m_populated;
};

#endif // TABLEINSTANCEFORM_H
