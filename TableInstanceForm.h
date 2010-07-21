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

    void setTableInstance(TableInstance* st)
    {
        m_tinst = st;
    }

    void createTableWithValues();

public slots:
    void onLoadValuesFromCSV();
    void onSaveValuesToCSV();
    void onAddNewRow();
    void onDeleteRow();
    void onValidateData();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::TableInstanceForm *ui;
    TableInstance* m_tinst;
};

#endif // TABLEINSTANCEFORM_H
