#include "TablesListForm.h"
#include "ui_TablesListForm.h"

TablesListForm::TablesListForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TablesListForm)
{
    ui->setupUi(this);
}

TablesListForm::~TablesListForm()
{
    delete ui;
}

void TablesListForm::changeEvent(QEvent *e)
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
