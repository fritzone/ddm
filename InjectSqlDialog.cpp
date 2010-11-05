#include "InjectSqlDialog.h"
#include "ui_InjectSqlDialog.h"

#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>

QString InjectSqlDialog::previousHost="";
QString InjectSqlDialog::previousUser="";


InjectSqlDialog::InjectSqlDialog(QWidget *parent) : QDialog(parent), ui(new Ui::InjectSqlDialog)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(true);
    ui->chkOnlyIfNotExist->hide();
    ui->txtDatabaseHost->setText(previousHost);
    ui->txtDatabaseUser->setText(previousUser);
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
        QMessageBox::critical (this, tr("Error"), tr("Cannot connect to the database: ") + db.lastError().databaseText() + "/"
                               + db.lastError().driverText(), QMessageBox::Ok);
        return;
    }

    QSqlQuery query;

    query.exec("show databases");

    while(query.next())
    {
        QString db = query.value(0).toString();
        ui->cmbDatabases->addItem(db);
    }

    ui->cmbDatabases->setEnabled(true);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(false);
    ui->txtDatabaseHost->setText(previousHost);
    ui->txtDatabaseUser->setText(previousUser);

}

QString InjectSqlDialog::getDatabase()
{
    return ui->cmbDatabases->currentText();
}

QString InjectSqlDialog::getUser()
{
    return ui->txtDatabaseUser->text();
}

QString InjectSqlDialog::getPassword()
{
    return ui->txtDatabasePassword->text();
}

QString InjectSqlDialog::getHost()
{
    return ui->txtDatabaseHost->text();
}

bool InjectSqlDialog::getRollbackOnError()
{
    return ui->chkRollbackOnError->isChecked();
}

bool InjectSqlDialog::getCreateOnlyIfNotExist()
{
    return ui->chkOnlyIfNotExist->isChecked();
}
