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
#include "db_AbstractDTSupplier.h"

// TODO: ugly ... But till we find out what's wrong with QCubrid keep it like this
#include <dbcubrid_DatabaseEngine.h>

#include <QtGui>
#include <QSqlTableModel>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QMessageBox>
#include <QFileDialog>
#include <QTabWidget>
#include <QSqlRecord>

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
    if(!tableForScriptResult)
    {
        return;
    }

    m_connection = connectionsForTabs[mainTab->currentIndex()]->conn;
    if(m_connection->getState() == DROPPED || m_connection->getState() == DELETED)
    {
        QMessageBox::critical(this, tr("Error"), tr("Cannot execute query on a dropped or deleted database"), QMessageBox::Ok);
        return;
    }
    QSqlDatabase db = m_connection->getQSqlDatabase();

    QSqlQuery query(db);
    QString sq = retrieveCurrentQuery();

    int crow = 0;
    int cnt = -1;

    if(query.exec(sq))
    {

        if(crow == 0)
        {
            QSqlRecord rec = query.record();
            cnt = rec.count();
            tableForScriptResult->setColumnCount(cnt);
            for(int i=0; i<cnt; i++)
            {
                QString name = rec.fieldName(i);
                QTableWidgetItem *columnHeaderItem = new QTableWidgetItem(name);
                columnHeaderItem->setTextAlignment(Qt::AlignVCenter);
                tableForScriptResult->setHorizontalHeaderItem(i, columnHeaderItem);
            }

            tableForScriptResult->setRowCount(0);
        }

        while(query.next())
        {
            tableForScriptResult->setRowCount(tableForScriptResult->rowCount() + 1);
            for(int j=0; j<cnt; j++)
            {
                QString v = "";
                QVariant vj = query.value(j);

                if(!query.value(j).isNull())
                {
                    if(vj.canConvert<int>())
                    {
                        if(vj.canConvert<QString>())
                        {
                            QString tv = vj.toString();
                            bool ok;
                            int x = tv.toInt(&ok);
                            if(ok)
                            {
                                v = QString::number(x);
                            }
                            else
                            {
                                v = tv;
                            }
                        }
                        else
                        {
                            v = QString::number(vj.toInt());
                        }
                    }
                    else
                    {
                        v = vj.toString();
                    }
                }
                tableForScriptResult->setItem(crow, j,  new QTableWidgetItem(v));
            }
            crow ++;
        }
    }
    else
    {
        QMessageBox::critical(this, tr("Error"), tr("Cannot execute the query:") + db.lastError().driverText() + "/" + db.lastError().databaseText(), QMessageBox::Ok);
    }

    Workspace::getInstance()->refreshConnection(m_connection);

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

QTableWidget *BrowseTableForm::createTableWidget(QWidget *p)
{
    QTableWidget* table = new QTableWidget(p);
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

void BrowseTableForm::newPageForProcedure(BrowsedTableLayout layout, const QString &tab, Connection *c)
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

void BrowseTableForm::newPageforTrigger(Connection *c, const QString &tab)
{
    queryFrame = new QFrame(this);
    queryFrame->setObjectName(QString::fromUtf8("frame_q"));
    queryFramesMainHorizontalLayout = new QHBoxLayout(queryFrame);
    queryFramesMainVerticalLayout = new QVBoxLayout();
    horizontalLayoutForButtons = new QHBoxLayout();
    horizontalLayoutForLineNumbersAndTextEdit = new QHBoxLayout();
    queryFramesMainVerticalLayout->addLayout(horizontalLayoutForLineNumbersAndTextEdit);
    queryFramesMainHorizontalLayout->addLayout(queryFramesMainVerticalLayout);


    Trigger* t = c->getEngine()->reverseEngineerTrigger(c, tab, 0);
    if(t)
    {
        TriggerForm* pf = new TriggerForm(0, c, true, true, this);
        pf->feedInTables(QStringList(t->getTable()));
        pf->feedInTriggerEvents(c->getEngine()->getTriggerEvents(), t->getEvent());
        pf->feedInTriggerTimes(c->getEngine()->getTriggerTimings(), t->getTime());
        pf->setTrigger(t);
        pf->showSql();

        horizontalLayoutForLineNumbersAndTextEdit->addWidget(pf);
        mainTab->addTab(queryFrame, IconFactory::getTriggerIcon(),
                        tab + strSemicolon + c->getFullLocation());
    }
}

void BrowseTableForm::createTableDataTab(Connection *c, const QString &tab)
{
    dataTab = new QWidget();
    dataTabsLayout = new QVBoxLayout(dataTab);

    QTableWidget* tableForTableData = new QTableWidget(dataTab);
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(2);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(tableForTableData->sizePolicy().hasHeightForWidth());
    tableForTableData->setSizePolicy(sizePolicy);
    tableForTableData->setContextMenuPolicy(Qt::ActionsContextMenu);
    tableForTableData->setAlternatingRowColors(true);
    tableForTableData->setSelectionMode(QAbstractItemView::SingleSelection);
    tableForTableData->setSelectionBehavior(QAbstractItemView::SelectItems);
    tableForTableData->setSortingEnabled(true);
    tableForTableData->setWordWrap(false);
    tableForTableData->horizontalHeader()->setDefaultSectionSize(200);

    dataTabsLayout->addWidget(tableForTableData);
    tabWidget->addTab(dataTab, tr("Data"));
    tableForTableData->raise();


    QSqlDatabase database = c->getQSqlDatabase();
    QStringList tabCols = c->getEngine()->getColumnsOfTable(c, tab);

    if(!database.isOpen())
    {
        return;
    }
    else
    {
        QSqlQuery q(database);
        QString sq = "select";

        for(int i=0; i<tabCols.size(); i++)
        {
            QString name = tabCols[i].left(tabCols[i].indexOf("@"));
            sq += " " + name;
            if(i < tabCols.size() - 1)
            {
                sq += ",";
            }
        }
        sq += " from " + tab;

        q.prepare(sq);
        if(!q.exec())
        {
            return;
        }
        else
        {
            tableForTableData->setColumnCount(tabCols.size());

            for(int k=0; k<tabCols.size(); k++)
            {
                QString name = tabCols[k].left(tabCols[k].indexOf("@"));
                QTableWidgetItem *columnHeaderItem = new QTableWidgetItem(name);
                columnHeaderItem->setTextAlignment(Qt::AlignVCenter);
                tableForTableData->setHorizontalHeaderItem(k, columnHeaderItem);
            }

            tableForTableData->setRowCount(0);

            int i = 0;
            while(q.next())
            {
                tableForTableData->insertRow(tableForTableData->rowCount());
                for(int j=0; j<tabCols.size(); j++)
                {
                    QString v = "";
                    QString type = tabCols[j].mid(tabCols[j].indexOf("@") + 1);

                    if(!q.value(j).isNull())
                    {
                        if(type.toUpper().startsWith("BIT"))
                        {
                            int t = q.value(j).toInt();
                            v = QString::number(t);
                        }
                        else
                        {
                            v = q.value(j).toString();
                        }
                    }
                    tableForTableData->setItem(i, j,  new QTableWidgetItem(v));
                }
                i++;
            }
        }
        database.close();
    }

}

void BrowseTableForm::createTableColumnsTab(Connection *c, const QString &tab)
{
    columnsTab = new QWidget();
    columnTabsLayout = new QVBoxLayout(columnsTab);

    if(dynamic_cast<CUBRIDDatabaseEngine*>(c->getEngine()))
    {
        QTableWidget* tableForTableColumns = new QTableWidget(columnsTab);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(2);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(tableForTableColumns->sizePolicy().hasHeightForWidth());
        tableForTableColumns->setSizePolicy(sizePolicy);
        tableForTableColumns->setContextMenuPolicy(Qt::ActionsContextMenu);
        tableForTableColumns->setAlternatingRowColors(true);
        tableForTableColumns->setSelectionMode(QAbstractItemView::SingleSelection);
        tableForTableColumns->setSelectionBehavior(QAbstractItemView::SelectItems);
        tableForTableColumns->setSortingEnabled(true);
        tableForTableColumns->setWordWrap(false);
        tableForTableColumns->horizontalHeader()->setDefaultSectionSize(200);

        columnTabsLayout->addWidget(tableForTableColumns);
        tabWidget->addTab(columnsTab, QString("Columns"));

        QSqlDatabase cubridDb = c->getQSqlDatabase();

        if(!cubridDb.isOpen())
        {
            return;
        }
        else
        {
            QSqlQuery q(cubridDb);
            q.prepare("desc " + tab);
            if(!q.exec())
            {
                return;
            }
            else
            {

                // ugly header, creating
                tableForTableColumns->setColumnCount(6);

                QTableWidgetItem *columnHeaderItemField = new QTableWidgetItem("Field");
                QTableWidgetItem *columnHeaderItemType = new QTableWidgetItem("Type");
                QTableWidgetItem *columnHeaderItemNull= new QTableWidgetItem("Null");
                QTableWidgetItem *columnHeaderItemKey = new QTableWidgetItem("Key");
                QTableWidgetItem *columnHeaderItemDefault = new QTableWidgetItem("Default");
                QTableWidgetItem *columnHeaderItemExtra = new QTableWidgetItem("Extra");

                columnHeaderItemField->setTextAlignment(Qt::AlignVCenter);
                columnHeaderItemType->setTextAlignment(Qt::AlignVCenter);
                columnHeaderItemNull->setTextAlignment(Qt::AlignVCenter);
                columnHeaderItemKey->setTextAlignment(Qt::AlignVCenter);
                columnHeaderItemDefault->setTextAlignment(Qt::AlignVCenter);
                columnHeaderItemExtra->setTextAlignment(Qt::AlignVCenter);

                tableForTableColumns->setHorizontalHeaderItem(0, columnHeaderItemField);
                tableForTableColumns->setHorizontalHeaderItem(1, columnHeaderItemType);
                tableForTableColumns->setHorizontalHeaderItem(2, columnHeaderItemNull);
                tableForTableColumns->setHorizontalHeaderItem(3, columnHeaderItemKey);
                tableForTableColumns->setHorizontalHeaderItem(4, columnHeaderItemDefault);
                tableForTableColumns->setHorizontalHeaderItem(5, columnHeaderItemExtra);

                tableForTableColumns->setRowCount(0);

                int i = 0;
                while(q.next())
                {
                    tableForTableColumns->insertRow(tableForTableColumns->rowCount());
                    for(int j=0; j<6; j++)
                    {
                        QString v = q.value(j).toString();
                        tableForTableColumns->setItem(i, j,  new QTableWidgetItem(v));
                    }
                    i++;
                }
            }
            cubridDb.close();
        }

    }
    else
    {
        tableForTableColumns = createTable(columnsTab);
        columnTabsLayout->addWidget(tableForTableColumns);
        tabWidget->addTab(columnsTab, QString("Columns"));

        QSqlQueryModel *model = new QSqlQueryModel(tableForTableColumns);
        QSqlDatabase db2 = c->getQSqlDatabase();
        QSqlQuery q(c->getEngine()->getTableDescriptionScript(tab), db2);
        model->setQuery(q);
        if(model->lastError().type() != QSqlError::NoError)
        {
            QMessageBox::critical(this, tr("Error"), model->lastError().text(), QMessageBox::Ok);
            if(db2.isOpen()) db2.close();
            return;
        }
        tableForTableColumns->setModel(model);
        db2.close();
    }

}

void BrowseTableForm::createTableScriptTab(BrowsedTableLayout layout, const QString &tab, Connection *c)
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

    // placing the cursor at the end
    QTextCursor tmpCursor = m_textEdit->textCursor();
    tmpCursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor, 1);
    m_textEdit->setTextCursor(tmpCursor);

    QKeyEvent* keyEventEnter = new QKeyEvent(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
    QKeyEvent* keyEventBackspace = new QKeyEvent(QEvent::KeyPress, Qt::Key_Backspace, Qt::NoModifier);
    m_textEdit->updateLineNumbers();
    m_textEdit->keyPressEvent(keyEventEnter);
    m_textEdit->keyPressEvent(keyEventBackspace);
    QObject::connect(btnSaveQuery, SIGNAL(clicked()), this, SLOT(onSaveQuery()));
    tabWidget->addTab(queryFrame, tr("Script"));
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
        newPageForProcedure(layout, tab, c);
    }

    if(layout == BROWSE_TRIGGER)
    {
        newPageforTrigger(c, tab);
    }

    if(layout == BROWSE_TABLE || layout == BROWSE_VIEW)
    {
        tabWidget = new QTabWidget(mainTabPageWidget);

        createTableDataTab(c, tab);
        createTableColumnsTab(c, tab);
        createTableScriptTab(layout, tab, c);

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
        tableForScriptResult = createTableWidget(queryFrame);
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
    setWindowTitle(QObject::tr("Form"));
    if(tabWidget) tabWidget->setTabText(tabWidget->indexOf(dataTab), QObject::tr("Data"));
    if(tabWidget) tabWidget->setTabText(tabWidget->indexOf(columnsTab), QObject::tr("Columns"));
#ifndef QT_NO_TOOLTIP
    if(btnExecuteQuery) btnExecuteQuery->setToolTip(QObject::tr("<b>Run</b> the query"));
#endif // QT_NO_TOOLTIP
    if(btnExecuteQuery) btnExecuteQuery->setText(QObject::tr("..."));
#ifndef QT_NO_TOOLTIP
    if(btnOpenQuery) btnOpenQuery->setToolTip(QObject::tr("<b>Open</b> a SQL file"));
#endif // QT_NO_TOOLTIP
    if(btnOpenQuery) btnOpenQuery->setText(QObject::tr("..."));
#ifndef QT_NO_TOOLTIP
    if(btnSaveQuery) btnSaveQuery->setToolTip(QObject::tr("<b>Save</b> the query"));
#endif // QT_NO_TOOLTIP
    if(btnSaveQuery) btnSaveQuery->setText(QObject::tr("..."));
} // retranslateUi
