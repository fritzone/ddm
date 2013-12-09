#include "InsertNewRecordDialog.h"
#include "ui_InsertNewRecordDialog.h"
#include "Connection.h"
#include "db_DatabaseEngine.h"

#include <QLabel>
#include <QLineEdit>

InsertNewRecordDialog::InsertNewRecordDialog(QWidget *parent, const QString &tab, Connection *c) :
    QDialog(parent),
    ui(new Ui::InsertNewRecordDialog)
{
    ui->setupUi(this);

    QStringList tabCols = c->getEngine()->getColumnsOfTable(c, tab);
    for(int i=0; i<tabCols.size(); i++)
    {
        QString name = tabCols[i].left(tabCols[i].indexOf("@"));
        ui->tableWidget->insertRow(ui->tableWidget->rowCount() );
        QLabel* t = new QLabel(name, this);
        t->setStyleSheet("QLabel{background:transparent;}");
        ui->tableWidget->setCellWidget(i, 0, t);
        QLineEdit* qle = new QLineEdit(this);
        qle->setStyleSheet("QLabel{background:white;}");
        ui->tableWidget->setCellWidget(i, 1, qle);
    }

    ui->tableWidget->resizeColumnsToContents();
    //ui->tableWidget->setStyleSheet("background:transparent;");
}

InsertNewRecordDialog::~InsertNewRecordDialog()
{
    delete ui;
}
