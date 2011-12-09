#include "BrowseTableForm.h"
#include "ui_BrowseTableForm.h"
#include "core_Connection.h"

#include <QSqlTableModel>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>

BrowseTableForm::BrowseTableForm(QWidget *parent, Connection* c, const QString& tab) :
    QWidget(parent),
    ui(new Ui::BrowseTableForm), m_connection(c), m_tab(tab)
{
    ui->setupUi(this);

    ui->table->horizontalHeader()->setHighlightSections(true);
    ui->table->horizontalHeader()->setResizeMode(QHeaderView::Interactive);

    textEdit = new QTextEditWithCodeCompletion(this, c);
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
    model->setQuery(QSqlQuery(textEdit->toPlainText(), m_connection->getQSqlDatabase()));
    ui->table->setModel(model);
}
