#ifndef BROWSETABLEFORM_H
#define BROWSETABLEFORM_H

#include "TextEditWithCodeCompletion.h"
#include "core_enums.h"

#include <QWidget>
#include <QSplitter>
#include <QtCore/QVariant>
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QFrame>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QSpacerItem>
#include <QTableWidget>
#include <QTableView>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>


namespace Ui {
    class BrowseTableForm;
}

class QTableView;
class Connection;

class BrowseTableForm : public QWidget
{
    Q_OBJECT

    struct BrowsedObjectAndConnection
    {
        Connection* conn;
        QString object;
    };

public:

    static BrowseTableForm* instance(QWidget *parent, Connection* c, const QString& tab, BrowsedTableLayout layout);
    static BrowseTableForm* instance();

    ~BrowseTableForm();
    void focusOnTextEdit();
    void setTextAt(int index, const QString&);

    void newPageForProcedure(BrowsedTableLayout layout, const QString &tab, Connection *c);
    void newPageforTrigger(Connection *c, const QString &tab);
    void createTableDataTab(Connection *c, const QString &tab);
    void createTableColumnsTab(Connection *c, const QString &tab);
    void createTableScriptTab(BrowsedTableLayout layout, const QString &tab, Connection *c);
protected:
    void changeEvent(QEvent *e);
    void resizeEvent(QResizeEvent *);

public slots:
    void onRunQuery();

private slots:
    void onSaveQuery();
    void onInsertNewRow();
    void onLoadQuery();
    void onTabCloseRequested(int);

private:

    explicit BrowseTableForm(QWidget *parent, Connection* c, const QString& tab);
    QString retrieveCurrentQuery();
    void newPage(Connection*c, const QString& tab, BrowsedTableLayout layout);
    void firstTimeSetup(Connection*c);
    void retranslateUi();
    QTableView* createTable(QWidget* p);
    QTableWidget* createTableWidget(QWidget* p);
    void updateCursor();

private:
    static QVector<BrowsedObjectAndConnection*> connectionsForTabs;
    static BrowseTableForm* m_instance;

private:
    TextEditWithCodeCompletion *m_textEdit;
    FrameForLineNumbers* m_frameForLineNumbers;
    int m_firstP;
    int m_lastP;
    Connection* m_connection;
    QString m_tab;
    QSplitter* spl;
    QTabWidget* mainTab;
    QVBoxLayout *mainFormsVerticalLayout;
    QWidget *mainTabPageWidget;
    QVBoxLayout *mainTabPageWidgetsLayout;
    QTabWidget *tabWidget;
    QWidget *dataTab;
    QVBoxLayout *dataTabsLayout;
    QVBoxLayout *columnTabsLayout;
    QTableView *tableForTableData;
    QTableView *tableForTableColumns;
    QTableWidget *tableForScriptResult;
    QWidget *columnsTab;
    QFrame *queryFrame;
    QHBoxLayout *queryFramesMainHorizontalLayout;
    QHBoxLayout *dataTabVerticalLayout;
    QHBoxLayout *dataTabHorizontalLayoutForButtons;
    QVBoxLayout *queryFramesMainVerticalLayout;
    QHBoxLayout *horizontalLayoutForButtons;
    QToolButton *btnExecuteQuery;
    QToolButton *btnOpenQuery;
    QToolButton *btnSaveQuery;
    QToolButton *btnInsertElements;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayoutForLineNumbersAndTextEdit;
};

#endif // BROWSETABLEFORM_H
