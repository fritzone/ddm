#ifndef QUERYITEMLISTDIALOG_H
#define QUERYITEMLISTDIALOG_H

#include <QDialog>
#include <QListWidgetItem>

#include "qbr_QueryGraphicsHelper.h"

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
    explicit QueryItemListDialog(QueryGraphicsHelper* helper, QueryGraphicsHelper::ListType t, QWidget *parent = 0);
    explicit QueryItemListDialog(QueryGraphicsHelper* helper, QStringList lst, QList<QIcon> icons, bool checks, QWidget *parent = 0);

    ~QueryItemListDialog();
    QString getSelection() const
    {
        return m_selected;
    }

    void setText(const QString& a);
    void showSymbolPanel();

public slots:

    void onDblClickItem(QListWidgetItem*);
    void onTxtInputKeyPress();
    void actionTriggered(QAction*);
    void pageRequested(int);

private:
    Ui::QueryItemListDialog *ui;
    QString m_selected;
    QMap<QString, QString> m_beforeAfter;
    QMenu* m_mathMenu;
    QMenu* m_bitMenu;
    QMenu* m_functionsMenu;
    QMenu* m_comparisonMenu;
    QMenu* m_tablesMenu;

    QueryGraphicsHelper* m_helper;
};

#endif // QUERYITEMLISTDIALOG_H
