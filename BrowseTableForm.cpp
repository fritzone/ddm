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

QTextEditWithCodeCompletion *BrowseTableForm::m_textEdit = 0;
FrameForLineNumbers* BrowseTableForm::m_frameForLineNumbers = 0;
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

    static FrameForLineNumbers* frameForLineNumbers = new FrameForLineNumbers(this);
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(frameForLineNumbers->sizePolicy().hasHeightForWidth());
    frameForLineNumbers->setSizePolicy(sizePolicy);
    frameForLineNumbers->setMinimumSize(QSize(48, 0));
    frameForLineNumbers->setMaximumSize(QSize(48, 16777215));
    frameForLineNumbers->setFrameShape(QFrame::StyledPanel);
    frameForLineNumbers->setFrameShadow(QFrame::Raised);
    ui->horizontalLayout_5->addWidget(frameForLineNumbers);


    textEdit->setObjectName(QString::fromUtf8("textEdit"));
    textEdit->setBrowseForm(this);
    ui->horizontalLayout_5->addWidget(textEdit);
    textEdit->setLineNumberFrame(frameForLineNumbers);
    textEdit->updateLineNumbers();

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
    if(m_frameForLineNumbers == 0) m_frameForLineNumbers = frameForLineNumbers;

    m_textEdit->resetToConnection(c);

    ui->txtConnection->setText(c->getFullLocation());
    ui->frame->hide();
    spl = new QSplitter(Qt::Vertical, this);
    spl->addWidget(ui->table);
    spl->addWidget(ui->groupBox);

    spl->resize( size() );

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
    spl->resize(e->size());
}
