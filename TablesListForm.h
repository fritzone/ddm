#ifndef TABLESLISTFORM_H
#define TABLESLISTFORM_H

#include <QWidget>

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

protected:
    void changeEvent(QEvent *e);

private:
    Ui::TablesListForm *ui;
};

#endif // TABLESLISTFORM_H
