#ifndef TABLEINSTANCESLISTFORM_H
#define TABLEINSTANCESLISTFORM_H

#include <QWidget>

class TableInstance;

namespace Ui {
    class TableInstancesListForm;
}

class TableInstancesListForm : public QWidget {
    Q_OBJECT
public:
    TableInstancesListForm(QWidget *parent = 0);
    ~TableInstancesListForm();
    void populateTableInstances(QVector<TableInstance*>const & tables);
protected:
    void changeEvent(QEvent *e);

private:
    Ui::TableInstancesListForm *ui;
};

#endif // TABLEINSTANCESLISTFORM_H
