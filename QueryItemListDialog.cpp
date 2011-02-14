#include "QueryItemListDialog.h"
#include "ui_QueryItemListDialog.h"

QueryItemListDialog::QueryItemListDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QueryItemListDialog)
{
    ui->setupUi(this);
}

QueryItemListDialog::~QueryItemListDialog()
{
    delete ui;
}
