#include "InjectSqlDialog.h"
#include "ui_InjectSqlDialog.h"

#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>

InjectSqlDialog::InjectSqlDialog(QWidget *parent) : QDialog(parent), ui(new Ui::InjectSqlDialog)
{
    ui->setupUi(this);
}

InjectSqlDialog::~InjectSqlDialog()
{
    delete ui;
}

void InjectSqlDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void InjectSqlDialog::onConnect()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName(ui->txtDatabaseHost->text());
    db.setUserName(ui->txtDatabaseUser->text());
    db.setPassword(ui->txtDatabasePassword->text());

    bool ok = db.open();

    if(!ok)
    {
        QMessageBox::critical (this, tr("Error"), tr("Cannot connect to the database: ") + db.lastError().text(), QMessageBox::Ok);
        return;
    }

    QSqlQuery query;

    query.exec("show databases");

    while(query.next())
    {
        QString db = query.value(0).toString();
        ui->cmbDatabases->addItem(db);
    }

}
