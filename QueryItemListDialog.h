#ifndef QUERYITEMLISTDIALOG_H
#define QUERYITEMLISTDIALOG_H

#include <QDialog>

namespace Ui {
    class QueryItemListDialog;
}

class QueryItemListDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QueryItemListDialog(QWidget *parent = 0);
    ~QueryItemListDialog();

private:
    Ui::QueryItemListDialog *ui;
};

#endif // QUERYITEMLISTDIALOG_H
