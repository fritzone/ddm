#ifndef QUERYITEMLISTDIALOG_H
#define QUERYITEMLISTDIALOG_H

#include <QDialog>
#include <QListWidgetItem>

#include "qbr_QueryGraphicsHelper.h"
class Table;

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
    void setColumnMode();
    void setColumns(QVector<const Column*> c)
    {
        m_columnsToShow = c;
    }
    void setOrderBy(QStringList oby)
    {
        m_orderBy = oby;
    }
    void setJoin(const SelectQueryJoinComponent* j)
    {
        m_join = j;
    }
    void setOrderByMode();
    void populateTablesAndColumns(QVector<const Table*>);

public slots:

    void onDblClickItem(QListWidgetItem*);
    void onTxtInputKeyPress();
    void actionTriggered(QAction*);
    void pageRequested(int);
    void btnLikeClicked();
    void btnDistinctClicked();
    void btnStarClicked();
    void btnIsClicked();
    void btnInClicked();
    void btnBetweenClicked();
    void btnExistsClicked();
    void btnNotClicked();
    void btnAcceptText();
    void btnRollupClicked();
    void btnNullClicked();
    void btnOpenParanthesisClicked();
    void btnCloseParanthesisClicked();

private:
    Ui::QueryItemListDialog *ui;
    QString m_selected;
    QMap<QString, QString> m_beforeAfter;
    QMenu* m_mathMenu;
    QMenu* m_bitMenu;
    QMenu* m_functionsMenu;
    QMenu* m_comparisonMenu;
    QMenu* m_tablesMenu;
    QMenu* m_functionsMathMenu;
    QMenu* m_functionsAggregateMenu;
    QMenu* m_functionsStringMenu;
    QMenu* m_functionsDateTimeMenu;
    QMenu* m_functionsCryptMenu;
    QMenu* m_functionsMiscMenu;
    QMenu* m_functionsCastMenu;
    QMenu* m_functionsBitMenu;
    QMenu* m_functionsControlFlowMenu;
    QMenu* m_functionsInfoMenu;

    QueryGraphicsHelper* m_helper;
    QVector<const Column*> m_columnsToShow;
    const SelectQueryJoinComponent* m_join;
    QStringList m_orderBy;
};

#endif // QUERYITEMLISTDIALOG_H
