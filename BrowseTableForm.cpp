#include "BrowseTableForm.h"
#include "ui_BrowseTableForm.h"
#include "core_Connection.h"
#include "FrameForLineNumbers.h"

#include <QtGui>
#include <QSqlTableModel>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QMessageBox>
#include <QFileDialog>
#include <QTabWidget>

QString BrowseTableForm::browseString = "";
int BrowseTableForm::m_firstP = 0;
int BrowseTableForm::m_lastP = 0;
QVector<Connection*> BrowseTableForm::connectionsForTabs;
BrowseTableForm* BrowseTableForm::m_instance = 0;

BrowseTableForm* BrowseTableForm::instance(QWidget *parent, Connection *c, const QString &tab)
{
    if(m_instance == 0)
    {
        m_instance = new BrowseTableForm(parent, c, tab);
        m_instance->firstTimeSetup(c);
        m_instance->newPage(c,tab);
    }
    else
    {
        m_instance->newPage(c, tab);
    }

    return m_instance;
}

BrowseTableForm::BrowseTableForm(QWidget *parent, Connection* c, const QString& tab) :
    QWidget(parent),
    m_frameForLineNumbers(0), m_connection(c), m_tab(tab)
{

    connectionsForTabs.append(c);




//    else
//    {


//    }

//    spl = new QSplitter(Qt::Vertical, this);
//    spl->addWidget(frame_2);
//    spl->addWidget(frame_q);

//    spl->resize( size() );

//

//    qDebug() << this << " " << m_connection;
}

BrowseTableForm::~BrowseTableForm()
{
    //delete ui;
}

// create the main tab
void BrowseTableForm::firstTimeSetup(Connection *c)
{
    mainTab = new QTabWidget(this);
    mainTab->setObjectName(QString::fromUtf8("mainTab"));
    mainTab->setTabShape(QTabWidget::Triangular);
    //mainTab->setDocumentMode(true);
    mainTab->setTabsClosable(true);
    mainTab->setMovable(true);

    mainFormsVerticalLayout = new QVBoxLayout(this);
    mainFormsVerticalLayout->addWidget(mainTab);
    setLayout(mainFormsVerticalLayout);
}

void BrowseTableForm::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        //retranslateUi(this);
        break;
    default:
        break;
    }
}

QTableView* BrowseTableForm::getTable()
{
    return table;
}

void BrowseTableForm::onRunQuery()
{
    m_connection = connectionsForTabs[mainTab->currentIndex()];
    if(m_connection->getState() == DROPPED || m_connection->getState() == DELETED)
    {
        QMessageBox::critical(this, tr("Error"), tr("Cannot execute query on a dropped or deleted database"), QMessageBox::Ok);
        return;
    }
    QSqlQueryModel *model = new QSqlQueryModel(table);
    model->setQuery(QSqlQuery(retrieveCurrentQuery(), m_connection->getQSqlDatabase()));
    if(model->lastError().type() != QSqlError::NoError)
    {
        QMessageBox::critical(this, tr("Error"), model->lastError().text(), QMessageBox::Ok);
        return;
    }
    table->setModel(model);
}

QString BrowseTableForm::retrieveCurrentQuery()
{
    if(m_firstP != 0 || m_lastP != 0)
    {
        // TODO: This is duplicate with below
        QTextCursor origCursor = m_textEdit->textCursor();
        QTextCursor copyCursor = origCursor;

        copyCursor.setPosition(m_firstP);
        for(int j = m_firstP; j!= m_lastP; j++) copyCursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
        QTextCharFormat fmt = copyCursor.charFormat();
        fmt.setBackground(QBrush(Qt::white));
        copyCursor.setCharFormat(fmt);
        m_textEdit->setTextCursor(origCursor);
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

    QTextCursor origCursor = m_textEdit->textCursor();
    QTextCursor copyCursor = origCursor;

    copyCursor.setPosition(m_firstP);
    for(int j = m_firstP; j!= m_lastP; j++) copyCursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
    QTextCharFormat fmt = copyCursor.charFormat();
    fmt.setBackground(QBrush(Qt::white));
    copyCursor.setCharFormat(fmt);
    m_textEdit->setTextCursor(origCursor);

    return before + after;
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
    // TODO: This is a pure duplicate from the SqlForm.cpp ... find a better way to admiister these things
    QString name = QFileDialog::getSaveFileName(this, tr("Save SQL Script"), "", tr("SQL files (*.sql)"));
    QFile file(name);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return;
    }
    QTextStream out(&file);
    out << m_textEdit->toPlainText() << "\n";
}

void BrowseTableForm::resizeEvent(QResizeEvent *e)
{
//    spl->resize(e->size());
}

void BrowseTableForm::textChanged()
{
    browseString = m_textEdit->toPlainText();
}

void BrowseTableForm::focusOnTextEdit()
{
    m_textEdit->setFocus();
}

void BrowseTableForm::newPage(Connection *c, const QString &tab)
{
    if(tab.length())
    {
        mainTabPageWidget = new QWidget();
        mainTabPageWidgetsLayout = new QVBoxLayout(mainTabPageWidget);
        mainTabPageWidgetsLayout->setContentsMargins(0, 0, 0, 0);
        tabWidget = new QTabWidget(mainTabPageWidget);

        // create the "data" tab
        dataTab = new QWidget();
        dataTabsLayout = new QVBoxLayout(dataTab);
        table = new QTableView(dataTab);
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
        dataTabsLayout->addWidget(table);
        tabWidget->addTab(dataTab, tr("Data"));
        table->raise();
        // and fill in the data for the data tab
        if(tab.length() > 0)
        {
            QSqlDatabase sqldb = c->getQSqlDatabase();
            QSqlTableModel *model = new QSqlTableModel(table, sqldb);
            model->setTable(tab);
            model->select();

            if (model->lastError().type() != QSqlError::NoError)
            {
                return;
            }

            table->setModel(model);
            table->setEditTriggers(QAbstractItemView::DoubleClicked|QAbstractItemView::EditKeyPressed);
        }

        // create the "columns" tab
        columnsTab = new QWidget();
        tabWidget->addTab(columnsTab, QString("Columns"));

        mainTabPageWidgetsLayout->addWidget(tabWidget);

        tabWidget->setCurrentIndex(0);

        mainTab->addTab(tabWidget, IconFactory::getTabinstIcon(), c->getDb() + "." + tab);
    }

    // create the frame for the query ... if this is a query
    if(tab.length() == 0)
    {
        mainTabPageWidget = new QWidget();
        mainTabPageWidgetsLayout = new QVBoxLayout(mainTabPageWidget);
        mainTabPageWidgetsLayout->setContentsMargins(0, 0, 0, 0);

        frame_q = new QFrame(this);
        frame_q->setObjectName(QString::fromUtf8("frame_q"));
        horizontalLayout_3 = new QHBoxLayout(frame_q);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        btnExecuteQuery = new QToolButton(frame_q);
        btnExecuteQuery->setObjectName(QString::fromUtf8("btnExecuteQuery"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/images/actions/images/actions/button_play_green.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnExecuteQuery->setIcon(icon1);

        horizontalLayout_2->addWidget(btnExecuteQuery);

        btnOpenQuery = new QToolButton(frame_q);
        btnOpenQuery->setObjectName(QString::fromUtf8("btnOpenQuery"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/images/actions/images/actions/fileopen.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnOpenQuery->setIcon(icon2);

        horizontalLayout_2->addWidget(btnOpenQuery);

        btnSaveQuery = new QToolButton(frame_q);
        btnSaveQuery->setObjectName(QString::fromUtf8("btnSaveQuery"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/images/actions/images/actions/filesave.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnSaveQuery->setIcon(icon3);

        horizontalLayout_2->addWidget(btnSaveQuery);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));

        verticalLayout->addLayout(horizontalLayout_5);


        horizontalLayout_3->addLayout(verticalLayout);


        mainTabPageWidgetsLayout->addWidget(frame_q);

        m_textEdit = new TextEditWithCodeCompletion(this, c);
        table->horizontalHeader()->setHighlightSections(true);
        table->horizontalHeader()->setResizeMode(QHeaderView::Interactive);

        m_frameForLineNumbers = new FrameForLineNumbers(this);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        m_frameForLineNumbers->setSizePolicy(sizePolicy);
        m_frameForLineNumbers->setMinimumSize(QSize(48, 0));
        m_frameForLineNumbers->setMaximumSize(QSize(48, 16777215));
        m_frameForLineNumbers->setFrameShape(QFrame::StyledPanel);
        m_frameForLineNumbers->setFrameShadow(QFrame::Raised);
        horizontalLayout_5->addWidget(m_frameForLineNumbers);

        m_textEdit->setObjectName(QString::fromUtf8("textEdit"));
        m_textEdit->setBrowseForm(this);
        horizontalLayout_5->addWidget(m_textEdit);
        m_textEdit->setLineNumberFrame(m_frameForLineNumbers);
        m_textEdit->updateLineNumbers();
        m_textEdit->resetToConnection(c);
        m_textEdit->setPlainText(browseString);
        connect(m_textEdit, SIGNAL(textChanged()), this, SLOT(textChanged()));
        QObject::connect(btnSaveQuery, SIGNAL(clicked()), this, SLOT(onSaveQuery()));
        QObject::connect(btnExecuteQuery, SIGNAL(clicked()), this, SLOT(onRunQuery()));
        QObject::connect(btnOpenQuery, SIGNAL(clicked()), this, SLOT(onLoadQuery()));

        mainTab->addTab(mainTabPageWidget, IconFactory::getSqlIcon(), QString("Script:") + c->getDb());


    }

    //retranslateUi(BrowseTableForm);

    QMetaObject::connectSlotsByName(this);
}
