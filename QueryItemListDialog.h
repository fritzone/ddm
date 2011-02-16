#ifndef QUERYITEMLISTDIALOG_H
#define QUERYITEMLISTDIALOG_H

#include <QDialog>
#include <QListWidgetItem>

#include "QueryComponents.h"

namespace Ui {
    class QueryItemListDialog;
}

class QueryItemListDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QueryItemListDialog(QueryComponents::ListType t, QWidget *parent = 0);
    ~QueryItemListDialog();
    QString getSelection() const
    {
        return m_selected;
    }

public slots:

    void onDblClickItem(QListWidgetItem*);

private:
    Ui::QueryItemListDialog *ui;
    QString m_selected;
};

#endif // QUERYITEMLISTDIALOG_H
