#include "SqlForm.h"
#include "ui_SqlForm.h"
#include "SqlHighlighter.h"
SqlForm::SqlForm(QWidget *parent) : QWidget(parent), ui(new Ui::SqlForm)
{
    ui->setupUi(this);
    highlighter = new SqlHighlighter(ui->txtSql->document());
    ui->grpSqlOptions->hide();
}

SqlForm::~SqlForm()
{
    delete ui;
}

void SqlForm::changeEvent(QEvent *e)
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

void SqlForm::setSource(const QString &src)
{
    ui->txtSql->setText(src);
}
