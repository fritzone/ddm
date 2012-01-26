#ifndef TABLESLISTFORM_H
#define TABLESLISTFORM_H

#include <QWidget>
#include <QTreeWidgetItem>

class Table;

namespace Ui
{
    class TablesListForm;
}

class TablesListForm : public QWidget
{
    Q_OBJECT
public:
    TablesListForm(QWidget *parent = 0);
    ~TablesListForm();

    void populateTables(QVector<Table*>const & tables);
    void setOop(bool a);

public slots:

    void doubleClickTree(QTreeWidgetItem*, int);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::TablesListForm *ui;
};

#endif // TABLESLISTFORM_H
