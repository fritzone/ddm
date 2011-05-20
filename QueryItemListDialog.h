#ifndef QUERYITEMLISTDIALOG_H
#define QUERYITEMLISTDIALOG_H

#include <QDialog>
#include <QListWidgetItem>

#include "QueryGraphicsHelper.h"

namespace Ui {
    class QueryItemListDialog;
}

/**
 * This is a messy class which shows the user either a list of options, either a list of
 * tables, either a single line entry...
 */
class QueryItemListDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QueryItemListDialog(QueryGraphicsHelper::ListType t, QWidget *parent = 0);
    explicit QueryItemListDialog(QStringList lst, QList<QIcon> icons, bool checks, QWidget *parent = 0);

    ~QueryItemListDialog();
    QString getSelection() const
    {
        return m_selected;
    }

    void setText(const QString& a);

public slots:

    void onDblClickItem(QListWidgetItem*);
    void onTxtInputKeyPress();

private:
    Ui::QueryItemListDialog *ui;
    QString m_selected;
};

#endif // QUERYITEMLISTDIALOG_H
