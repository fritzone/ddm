#include "BrowseTableForm.h"
#include "ui_BrowseTableForm.h"
#include "Connection.h"
#include "FrameForLineNumbers.h"
#include "db_DatabaseEngine.h"
#include "core_Procedure.h"
#include "core_Function.h"
#include "core_Trigger.h"
#include "TriggerForm.h"
#include "helper_utils.h"
#include "strings.h"
#include "NewTableForm.h"

#include <QtGui>
#include <QSqlTableModel>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QMessageBox>
#include <QFileDialog>
#include <QTabWidget>

QVector<BrowseTableForm::BrowsedObjectAndConnection*> BrowseTableForm::connectionsForTabs;
BrowseTableForm* BrowseTableForm::m_instance = 0;

BrowseTableForm* BrowseTableForm::instance(QWidget *parent, Connection *c, const QString &tab, BrowsedTableLayout layout)
{
    if(m_instance == 0)
    {
        m_instance = new BrowseTableForm(parent, c, tab);
        m_instance->firstTimeSetup(c);
        m_instance->newPage(c,tab, layout);
    }
    else
    {
        m_instance->newPage(c, tab, layout);
        m_instance->mainTab->setCurrentIndex(m_instance->mainTab->count() - 1);
    }

    return m_instance;
}

BrowseTableForm* BrowseTableForm::instance()
{
    return m_instance;
}

BrowseTableForm::BrowseTableForm(QWidget *parent, Connection* c, const QString& tab) :
    QWidget(parent), m_textEdit(0), m_frameForLineNumbers(0), m_firstP(0), m_lastP(0), m_connection(c), m_tab(tab), spl(0), mainTab(0), mainFormsVerticalLayout(0),
    mainTabPageWidget(0), mainTabPageWidgetsLayout(0), tabWidget(0), dataTab(0), dataTabsLayout(0), columnTabsLayout(0),
    tableForTableData(0), tableForTableColumns(0), tableForScriptResult(0),
    columnsTab(0), queryFrame(0), queryFramesMainHorizontalLayout(0), queryFramesMainVerticalLayout(0), horizontalLayoutForButtons(0), btnExecuteQuery(0),
    btnOpenQuery(0), btnSaveQuery(0), horizontalSpacer(0), horizontalLayoutForLineNumbersAndTextEdit(0)
{
}

BrowseTableForm::~BrowseTableForm()
{
    m_instance = 0;
}

// create the main tab
void BrowseTableForm::firstTimeSetup(Connection*)
{
    mainTab = new QTabWidget(this);
    mainTab->setObjectName(QString::fromUtf8("mainTab"));
    mainTab->setTabShape(QTabWidget::Triangular);
    mainTab->setDocumentMode(true);
    mainTab->setTabsClosable(true);

    mainFormsVerticalLayout = new QVBoxLayout(this);
    mainFormsVerticalLayout->addWidget(mainTab);
    setLayout(mainFormsVerticalLayout);

    connect(mainTab, SIGNAL(tabCloseRequested(int)), this, SLOT(onTabCloseRequested(int)));
}

void BrowseTableForm::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi();
        break;
    default:
        break;
    }
}

void BrowseTableForm::onRunQuery()
{
    if(!tableForScriptResult) return;
    m_connection = connectionsForTabs[mainTab->currentIndex()]->conn;
    if(m_connection->getState() == DROPPED || m_connection->getState() == DELETED)
    {
        QMessageBox::critical(this, tr("Error"), tr("Cannot execute query on a dropped or deleted database"), QMessageBox::Ok);
        return;
    }
    QSqlQueryModel *model = new QSqlQueryModel(tableForScriptResult);
    QSqlDatabase db = m_connection->getQSqlDatabase();
    model->setQuery(QSqlQuery(retrieveCurrentQuery(), db));
    if(model->lastError().type() != QSqlError::NoError)
    {
        QMessageBox::critical(this, tr("Error"), model->lastError().text(), QMessageBox::Ok);
        return;
    }
    tableForScriptResult->setModel(model);
    db.close();
}

QString BrowseTableForm::retrieveCurrentQuery()
{
    if(m_firstP != 0 || m_lastP != 0)
    {
        updateCursor();
    }

    QString t = m_textEdit->toPlainText();
    int cursorP = m_textEdit->textCursor().position() - 1;
    QString before = "";
    QString after = "";
    int i = cursorP;

    // now go backward till the last ; or doc. beginning
    while(i > -1 && t.at(i) != ';')
    {
        before.prepend(t.at(i));
        i--;
        if(i>-1)
        {
            if(t.at(i) == '\n' && t.at(i+1) == '\n') break;
        }
    }
    m_firstP = i;
    if(m_firstP == -1) m_firstP = 0;
    // and then forward till the next ; or doc. end
    i = cursorP+1;
    while(i < t.length() && t.at(i) != ';')
    {
        after.append(t.at(i));
        i++;
        if(i< t.length())
        {
            if(t.at(i) == '\n' && t.at(i-1) == '\n') break;
        }
    }
    m_lastP = i;

    updateCursor();
    return before + after;
}

void BrowseTableForm::updateCursor()
{
    QTextCursor origCursor = m_textEdit->textCursor();
    QTextCursor copyCursor = origCursor;

    copyCursor.setPosition(m_firstP);
    for(int j = m_firstP; j!= m_lastP; j++) copyCursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
    QTextCharFormat fmt = copyCursor.charFormat();
    fmt.setBackground(QBrush(Qt::white));
    copyCursor.setCharFormat(fmt);
    m_textEdit->setTextCursor(origCursor);

}

void BrowseTableForm::onLoadQuery()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open SQL Script"), ".", tr("SQL Script (*.sql)"));
    QFile f(fileName);
    if(!f.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    QByteArray ba = f.readAll();
    QString g(ba);
    m_textEdit->appendPlainText("-- Loaded " + fileName.right(-1 + fileName.length() - fileName.lastIndexOf(QDir::separator())) + " on " + QDate::currentDate().toString() + " - " + QTime::currentTime().toString());
    m_textEdit->appendPlainText(g);
    m_textEdit->updateLineNumbers();
}


void BrowseTableForm::onSaveQuery()
{
    DDM::saveTextFileWithType(this, m_textEdit->toPlainText(), tr("Save SQL Script"), tr("SQL files (*.sql)"));
}

void BrowseTableForm::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
    if(spl)
    {
        if(mainTabPageWidget)
        {
            spl->resize(e->size());
        }
    }
}

void BrowseTableForm::onTabCloseRequested(int idx)
{
    mainTab->removeTab(idx);
    connectionsForTabs.remove(idx);
}

void BrowseTableForm::focusOnTextEdit()
{
    m_textEdit->setFocus();
}

void BrowseTableForm::setTextAt(int index, const QString & txt)
{
    if(index > -1)
    {
        mainTab->setTabText(index, txt);
    }
}

QTableView* BrowseTableForm::createTable(QWidget *p)
{
    QTableView* table = new QTableView(p);
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(2);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(table->sizePolicy().hasHeightForWidth());
    table->setSizePolicy(sizePolicy);
    table->setContextMenuPolicy(Qt::ActionsContextMenu);
    table->setAlternatingRowColors(true);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setSelectionBehavior(QAbstractItemView::SelectItems);
    table->setSortingEnabled(true);
    table->setWordWrap(false);
    table->horizontalHeader()->setDefaultSectionSize(200);
    return table;
}

void BrowseTableForm::newPage(Connection *c, const QString &tab, BrowsedTableLayout layout)
{
    BrowseTableForm::BrowsedObjectAndConnection *x = new BrowseTableForm::BrowsedObjectAndConnection();
    x->conn= c;
    x->object = tab;
    connectionsForTabs.append(x);

    mainTabPageWidget = new QWidget();
    mainTabPageWidgetsLayout = new QVBoxLayout(mainTabPageWidget);
    mainTabPageWidgetsLayout->setContentsMargins(0, 0, 0, 0);

    if(layout == BROWSE_PROCEDURE || layout == BROWSE_FUNCTION)
    {
        queryFrame = new QFrame(this);
        queryFrame->setObjectName(QString::fromUtf8("frame_q"));
        queryFramesMainHorizontalLayout = new QHBoxLayout(queryFrame);
        queryFramesMainVerticalLayout = new QVBoxLayout();
        horizontalLayoutForButtons = new QHBoxLayout();
        btnSaveQuery = new QToolButton(queryFrame);
        btnSaveQuery->setIcon(IconFactory::getSaveIcon());
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        horizontalLayoutForButtons->addWidget(btnSaveQuery);
        horizontalLayoutForButtons->addItem(horizontalSpacer);
        queryFramesMainVerticalLayout->addLayout(horizontalLayoutForButtons);
        horizontalLayoutForLineNumbersAndTextEdit = new QHBoxLayout();
        queryFramesMainVerticalLayout->addLayout(horizontalLayoutForLineNumbersAndTextEdit);
        queryFramesMainHorizontalLayout->addLayout(queryFramesMainVerticalLayout);

        mainTabPageWidgetsLayout->addWidget(queryFrame);

        m_textEdit = new TextEditWithCodeCompletion(this, c);

        m_frameForLineNumbers = new FrameForLineNumbers(this);
        horizontalLayoutForLineNumbersAndTextEdit->addWidget(m_frameForLineNumbers);

        m_textEdit->setBrowseForm(this);
        horizontalLayoutForLineNumbersAndTextEdit->addWidget(m_textEdit);
        m_textEdit->setLineNumberFrame(m_frameForLineNumbers);
        m_textEdit->updateLineNumbers();
        m_textEdit->resetToConnection(c);

        QObject::connect(btnSaveQuery, SIGNAL(clicked()), this, SLOT(onSaveQuery()));

        if(layout == BROWSE_PROCEDURE)
        {
            Procedure* p = c->getEngine()->reverseEngineerProc(c, tab, 0);
            if(p)
            {
                m_textEdit->setPlainText(p->getSql());
            }

            mainTab->addTab(queryFrame, IconFactory::getProcedureIcon(), tab + ":" + c->getFullLocation());
        }
        else
        {
            Function* p = c->getEngine()->reverseEngineerFunc(c, tab, 0);
            if(p)
            {
                m_textEdit->setPlainText(p->getSql());
            }

            mainTab->addTab(queryFrame, IconFactory::getFunctionTreeIcon(), tab + ":" + c->getFullLocation());

        }
    }

    if(layout == BROWSE_TRIGGER)
    {
        queryFrame = new QFrame(this);
        queryFrame->setObjectName(QString::fromUtf8("frame_q"));
        queryFramesMainHorizontalLayout = new QHBoxLayout(queryFrame);
        queryFramesMainVerticalLayout = new QVBoxLayout();
        horizontalLayoutForButtons = new QHBoxLayout();
//        btnSaveQuery = new QToolButton(queryFrame);
//        btnSaveQuery->setIcon(IconFactory::getSaveIcon());
//        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
//        horizontalLayoutForButtons->addWidget(btnSaveQuery);
//        horizontalLayoutForButtons->addItem(horizontalSpacer);
//        queryFramesMainVerticalLayout->addLayout(horizontalLayoutForButtons);
        horizontalLayoutForLineNumbersAndTextEdit = new QHBoxLayout();
        queryFramesMainVerticalLayout->addLayout(horizontalLayoutForLineNumbersAndTextEdit);
        queryFramesMainHorizontalLayout->addLayout(queryFramesMainVerticalLayout);


        Trigger* t = c->getEngine()->reverseEngineerTrigger(c, tab, 0);
        if(t)
        {
            TriggerForm* pf = new TriggerForm(0, true, true, this);
            pf->feedInTables(QStringList(t->getTable()));
            pf->setTrigger(t);
            pf->showSql();

            horizontalLayoutForLineNumbersAndTextEdit->addWidget(pf);
            mainTab->addTab(queryFrame, IconFactory::getTriggerIcon(), tab + ":" + c->getFullLocation());
        }
        return;
    }

    if(layout == BROWSE_TABLE || layout == BROWSE_VIEW)
    {
        tabWidget = new QTabWidget(mainTabPageWidget);

        {
        // create the "Data" tab
        dataTab = new QWidget();
        dataTabsLayout = new QVBoxLayout(dataTab);
        tableForTableData = createTable(dataTab);
        dataTabsLayout->addWidget(tableForTableData);
        tabWidget->addTab(dataTab, tr("Data"));
        tableForTableData->raise();

        // and fill in the data for the "data" tab
        QSqlDatabase sqldb = c->getQSqlDatabase();
        QSqlTableModel *model = new QSqlTableModel(tableForTableData, sqldb);
        model->setTable(tab);
        model->select();

        if (model->lastError().type() == QSqlError::NoError)
        {
            tableForTableData->setModel(model);
            tableForTableData->setEditTriggers(QAbstractItemView::DoubleClicked|QAbstractItemView::EditKeyPressed);
        }
        sqldb.close();
        }

        {
        // create the "columns" tab
        columnsTab = new QWidget();
        columnTabsLayout = new QVBoxLayout(columnsTab);
        tableForTableColumns = createTable(columnsTab);
        columnTabsLayout->addWidget(tableForTableColumns);
        tabWidget->addTab(columnsTab, QString("Columns"));

        QSqlQueryModel *model = new QSqlQueryModel(tableForTableColumns);
        QSqlDatabase db2 = c->getQSqlDatabase();
        model->setQuery(QSqlQuery(c->getEngine()->getTableDescriptionScript(tab), db2));
        if(model->lastError().type() != QSqlError::NoError)
        {
            QMessageBox::critical(this, tr("Error"), model->lastError().text(), QMessageBox::Ok);
            if(db2.isOpen()) db2.close();
            return;
        }
        tableForTableColumns->setModel(model);
        db2.close();
        }

        // create the Script tab
        {
            queryFrame = new QFrame(this);
            queryFramesMainHorizontalLayout = new QHBoxLayout(queryFrame);
            queryFramesMainVerticalLayout = new QVBoxLayout();
            horizontalLayoutForButtons = new QHBoxLayout();
            btnSaveQuery = new QToolButton(queryFrame);
            btnSaveQuery->setIcon(IconFactory::getSaveIcon());
            horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
            horizontalLayoutForButtons->addWidget(btnSaveQuery);
            horizontalLayoutForButtons->addItem(horizontalSpacer);
            queryFramesMainVerticalLayout->addLayout(horizontalLayoutForButtons);
            horizontalLayoutForLineNumbersAndTextEdit = new QHBoxLayout();
            queryFramesMainVerticalLayout->addLayout(horizontalLayoutForLineNumbersAndTextEdit);
            queryFramesMainHorizontalLayout->addLayout(queryFramesMainVerticalLayout);

            mainTabPageWidgetsLayout->addWidget(queryFrame);

            m_textEdit = new TextEditWithCodeCompletion(this, c);

            m_frameForLineNumbers = new FrameForLineNumbers(this);
            horizontalLayoutForLineNumbersAndTextEdit->addWidget(m_frameForLineNumbers);

            m_textEdit->setBrowseForm(this);
            horizontalLayoutForLineNumbersAndTextEdit->addWidget(m_textEdit);
            m_textEdit->setLineNumberFrame(m_frameForLineNumbers);
            QString s;
            if(layout == BROWSE_TABLE)
            {
                s = c->getEngine()->getTableCreationScript(c, tab);
            }
            else
            {
                s = c->getEngine()->getViewCreationScript(c, tab);
            }
            m_textEdit->setPlainText(s);
            m_textEdit->updateLineNumbers();
            QKeyEvent* keyEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
            m_textEdit->updateLineNumbers();
            m_textEdit->keyPressEvent(keyEvent);
            QObject::connect(btnSaveQuery, SIGNAL(clicked()), this, SLOT(onSaveQuery()));
            tabWidget->addTab(queryFrame, tr("Script"));
        }

        mainTabPageWidgetsLayout->addWidget(tabWidget);

        tabWidget->setCurrentIndex(0);

        mainTab->addTab(tabWidget, layout == BROWSE_TABLE?IconFactory::getTabinstIcon():IconFactory::getViewIcon(), tab + ":" + c->getFullLocation());

    }

    // create the frame for the query ... if this is a query
    if(layout == CREATE_SCRIPT)
    {
        queryFrame = new QFrame(this);
        queryFrame->setObjectName(QString::fromUtf8("frame_q"));
        queryFramesMainHorizontalLayout = new QHBoxLayout(queryFrame);
        queryFramesMainVerticalLayout = new QVBoxLayout();
        horizontalLayoutForButtons = new QHBoxLayout();
        horizontalLayoutForButtons->setSpacing(1);
        btnExecuteQuery = new QToolButton(queryFrame);
        btnExecuteQuery->setIcon(IconFactory::getRunQueryIcon());
        btnOpenQuery = new QToolButton(queryFrame);
        btnOpenQuery->setIcon(IconFactory::getOpenFileIcon());
        btnSaveQuery = new QToolButton(queryFrame);
        btnSaveQuery->setIcon(IconFactory::getSaveIcon());
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        horizontalLayoutForButtons->addWidget(btnExecuteQuery);
        horizontalLayoutForButtons->addWidget(btnOpenQuery);
        horizontalLayoutForButtons->addWidget(btnSaveQuery);
        horizontalLayoutForButtons->addItem(horizontalSpacer);
        queryFramesMainVerticalLayout->addLayout(horizontalLayoutForButtons);

        horizontalLayoutForLineNumbersAndTextEdit = new QHBoxLayout();
        queryFramesMainVerticalLayout->addLayout(horizontalLayoutForLineNumbersAndTextEdit);
        queryFramesMainHorizontalLayout->addLayout(queryFramesMainVerticalLayout);

        mainTabPageWidgetsLayout->addWidget(queryFrame);

        m_textEdit = new TextEditWithCodeCompletion(this, c);

        m_frameForLineNumbers = new FrameForLineNumbers(this);
        horizontalLayoutForLineNumbersAndTextEdit->addWidget(m_frameForLineNumbers);

        m_textEdit->setBrowseForm(this);
        horizontalLayoutForLineNumbersAndTextEdit->addWidget(m_textEdit);
        m_textEdit->setLineNumberFrame(m_frameForLineNumbers);
        m_textEdit->updateLineNumbers();
        m_textEdit->resetToConnection(c);

        QObject::connect(btnSaveQuery, SIGNAL(clicked()), this, SLOT(onSaveQuery()));
        QObject::connect(btnExecuteQuery, SIGNAL(clicked()), this, SLOT(onRunQuery()));
        QObject::connect(btnOpenQuery, SIGNAL(clicked()), this, SLOT(onLoadQuery()));

        //
        tableForScriptResult = createTable(queryFrame);
        mainTabPageWidgetsLayout->addWidget(tableForScriptResult);
        tableForScriptResult->raise();

        spl = new QSplitter(Qt::Vertical, mainTabPageWidget);
        spl->addWidget(queryFrame);
        spl->addWidget(tableForScriptResult);

        spl->resize( mainTabPageWidget->size() );
        mainTab->addTab(spl, IconFactory::getSqlIcon(), "Script:" + c->getFullLocation());

    }

    if(layout == NEW_TABLE_IN_DB)
    {
        queryFrame = new QFrame(this);
        queryFrame->setObjectName(QString::fromUtf8("frame_q"));
        queryFramesMainHorizontalLayout = new QHBoxLayout(queryFrame);

        NewTableForm* ntf = new NewTableForm(c->getEngine(), 0, 0, c, this, true);

        queryFramesMainHorizontalLayout->addWidget(ntf);
        int idx = mainTab->addTab(queryFrame, IconFactory::getIconForDatabase(c->getDbType()), tr("New Table: ") + c->getName());

        ntf->focusOnName();
        ntf->setMainTabIndex(idx);
    }

    retranslateUi();

    QMetaObject::connectSlotsByName(this);
}


void BrowseTableForm::retranslateUi()
{
    setWindowTitle(QApplication::translate("BrowseTableForm", "Form", 0, QApplication::UnicodeUTF8));
    if(tabWidget) tabWidget->setTabText(tabWidget->indexOf(dataTab), QApplication::translate("BrowseTableForm", "Data", 0, QApplication::UnicodeUTF8));
    if(tabWidget) tabWidget->setTabText(tabWidget->indexOf(columnsTab), QApplication::translate("BrowseTableForm", "Columns", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
    if(btnExecuteQuery) btnExecuteQuery->setToolTip(QApplication::translate("BrowseTableForm", "<b>Run</b> the query", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
    if(btnExecuteQuery) btnExecuteQuery->setText(QApplication::translate("BrowseTableForm", "...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
    if(btnOpenQuery) btnOpenQuery->setToolTip(QApplication::translate("BrowseTableForm", "<b>Open</b> a SQL file", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
    if(btnOpenQuery) btnOpenQuery->setText(QApplication::translate("BrowseTableForm", "...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
    if(btnSaveQuery) btnSaveQuery->setToolTip(QApplication::translate("BrowseTableForm", "<b>Save</b> the query", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
    if(btnSaveQuery) btnSaveQuery->setText(QApplication::translate("BrowseTableForm", "...", 0, QApplication::UnicodeUTF8));
} // retranslateUi
