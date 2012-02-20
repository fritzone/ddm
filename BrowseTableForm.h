#ifndef BROWSETABLEFORM_H
#define BROWSETABLEFORM_H

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

#include "TextEditWithCodeCompletion.h"

namespace Ui {
    class BrowseTableForm;
}

class QTableView;
class Connection;

class BrowseTableForm : public QWidget
{
    Q_OBJECT

public:

    static BrowseTableForm* instance(QWidget *parent, Connection* c, const QString& tab);
    ~BrowseTableForm();
    QTableView* getTable();
    void focusOnTextEdit();

protected:
    void changeEvent(QEvent *e);
    void resizeEvent(QResizeEvent *);

public slots:
    void onRunQuery();
    void onSaveQuery();
    void onLoadQuery();
    void textChanged();

private:

    explicit BrowseTableForm(QWidget *parent, Connection* c, const QString& tab);
    QString retrieveCurrentQuery();
    void newPage(Connection*c, const QString& tab);
    void firstTimeSetup(Connection*c);

private:
    TextEditWithCodeCompletion *m_textEdit;
    FrameForLineNumbers* m_frameForLineNumbers;
    static int m_firstP;
    static int m_lastP;
    Connection* m_connection;
    QString m_tab;
    QSplitter* spl;
    static QString browseString ;
    QTabWidget* mainTab;
    static QVector<Connection*> connectionsForTabs;
    static BrowseTableForm* m_instance;

    QVBoxLayout *mainFormsVerticalLayout;

    QWidget *mainTabPageWidget;
    QVBoxLayout *mainTabPageWidgetsLayout;
    QTabWidget *tabWidget;
    QWidget *dataTab;
    QVBoxLayout *dataTabsLayout;
    QTableView *table;
    QWidget *columnsTab;
    QFrame *frame_q;
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_2;
    QToolButton *btnExecuteQuery;
    QToolButton *btnOpenQuery;
    QToolButton *btnSaveQuery;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout_5;
};

#endif // BROWSETABLEFORM_H
