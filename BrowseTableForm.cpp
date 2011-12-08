#include "BrowseTableForm.h"
#include "ui_BrowseTableForm.h"
#include "QTextEditWithCodeCompletion.h"

BrowseTableForm::BrowseTableForm(QWidget *parent, Connection* c) :
    QWidget(parent),
    ui(new Ui::BrowseTableForm)
{
    ui->setupUi(this);

    ui->table->horizontalHeader()->setHighlightSections(true);
    ui->table->horizontalHeader()->setResizeMode(QHeaderView::Interactive);

    QTextEditWithCodeCompletion *textEdit;
    textEdit = new QTextEditWithCodeCompletion(ui->groupBox, c);
    textEdit->setObjectName(QString::fromUtf8("textEdit"));

    ui->verticalLayout_2->addWidget(textEdit);
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
