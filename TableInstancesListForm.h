#ifndef TABLEINSTANCESLISTFORM_H
#define TABLEINSTANCESLISTFORM_H

#include <QWidget>
#include <QTreeWidgetItem>

class TableInstance;
class MainWindow;

namespace Ui
{
    class TableInstancesListForm;
}

class TableInstancesListForm : public QWidget
{
    Q_OBJECT
public:
    TableInstancesListForm(QWidget *parent = 0);
    ~TableInstancesListForm();
    void populateTableInstances(QVector<TableInstance*>const & tables);

protected:
    void changeEvent(QEvent *e);

public slots:
    void doubleClickTree(QTreeWidgetItem*, int);

private:
    Ui::TableInstancesListForm *ui;
    MainWindow* mw;
};

#endif // TABLEINSTANCESLISTFORM_H
