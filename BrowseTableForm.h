#ifndef BROWSETABLEFORM_H
#define BROWSETABLEFORM_H

#include "TextEditWithCodeCompletion.h"
#include "enums.h"

#include <QWidget>
#include <QSplitter>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFrame>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QSpacerItem>
#include <QtGui/QTabWidget>
#include <QtGui/QTableView>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>


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
    ~BrowseTableForm();
    void focusOnTextEdit();

protected:
    void changeEvent(QEvent *e);
    void resizeEvent(QResizeEvent *);

public slots:
    void onRunQuery();

private slots:
    void onSaveQuery();
    void onLoadQuery();
    void onTabCloseRequested(int);

private:

    explicit BrowseTableForm(QWidget *parent, Connection* c, const QString& tab);
    QString retrieveCurrentQuery();
    void newPage(Connection*c, const QString& tab, BrowsedTableLayout layout);
    void firstTimeSetup(Connection*c);
    void retranslateUi();
    QTableView* createTable(QWidget* p);

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
    QTableView *tableForScriptResult;
    QWidget *columnsTab;
    QFrame *queryFrame;
    QHBoxLayout *queryFramesMainHorizontalLayout;
    QVBoxLayout *queryFramesMainVerticalLayout;
    QHBoxLayout *horizontalLayoutForButtons;
    QToolButton *btnExecuteQuery;
    QToolButton *btnOpenQuery;
    QToolButton *btnSaveQuery;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayoutForLineNumbersAndTextEdit;
};

#endif // BROWSETABLEFORM_H
