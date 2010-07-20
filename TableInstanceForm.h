#ifndef TABLEINSTANCEFORM_H
#define TABLEINSTANCEFORM_H

#include <QWidget>

namespace Ui {
    class TableInstanceForm;
}

class TableInstanceForm : public QWidget {
    Q_OBJECT
public:
    TableInstanceForm(QWidget *parent = 0);
    ~TableInstanceForm();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::TableInstanceForm *ui;
};

#endif // TABLEINSTANCEFORM_H
