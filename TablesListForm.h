#ifndef TABLESLISTFORM_H
#define TABLESLISTFORM_H

#include <QWidget>

namespace Ui {
    class TablesListForm;
}

class TablesListForm : public QWidget {
    Q_OBJECT
public:
    TablesListForm(QWidget *parent = 0);
    ~TablesListForm();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::TablesListForm *ui;
};

#endif // TABLESLISTFORM_H
