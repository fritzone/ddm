#include "BrowseTableForm.h"
#include "ui_BrowseTableForm.h"
#include "core_Connection.h"

#include <QSqlTableModel>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QMessageBox>

QTextEditWithCodeCompletion *BrowseTableForm::m_textEdit = 0;
int BrowseTableForm::m_firstP = 0;
int BrowseTableForm::m_lastP = 0;

BrowseTableForm::BrowseTableForm(QWidget *parent, Connection* c, const QString& tab) :
    QWidget(parent),
    ui(new Ui::BrowseTableForm), m_connection(c), m_tab(tab)
{
    ui->setupUi(this);

    static QTextEditWithCodeCompletion *textEdit = new QTextEditWithCodeCompletion(this, c);
    ui->table->horizontalHeader()->setHighlightSections(true);
    ui->table->horizontalHeader()->setResizeMode(QHeaderView::Interactive);

    textEdit->setObjectName(QString::fromUtf8("textEdit"));
    textEdit->setBrowseForm(this);
    ui->verticalLayout_2->addWidget(textEdit);

    QSqlDatabase sqldb = c->getQSqlDatabase();
    QSqlTableModel *model = new QSqlTableModel(ui->table, sqldb);
    model->setTable(tab);
    model->select();

    if (model->lastError().type() != QSqlError::NoError)
    {
        return;
    }

    ui->table->setModel(model);
    ui->table->setEditTriggers(QAbstractItemView::DoubleClicked|QAbstractItemView::EditKeyPressed);
    if(m_textEdit == 0) m_textEdit = textEdit;

    m_textEdit->resetToConnection(c);

    ui->txtConnection->setText(c->getFullLocation());
}

BrowseTableForm::~BrowseTableForm()
{
    delete ui;
}

void BrowseTableForm::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

QTableView* BrowseTableForm::getTable()
{
    return ui->table;
}

void BrowseTableForm::onRunQuery()
{
    QSqlQueryModel *model = new QSqlQueryModel(ui->table);
    model->setQuery(QSqlQuery(retrieveCurrentQuery(), m_connection->getQSqlDatabase()));
    if(model->lastError().type() != QSqlError::NoError)
    {
        QMessageBox::critical(this, tr("Error"), model->lastError().text(), QMessageBox::Ok);
        return;
    }
    ui->table->setModel(model);
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
        qDebug() << after;
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