#include "InjectSqlDialog.h"
#include "ui_InjectSqlDialog.h"

#include "DatabaseEngine.h"

#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>

QString InjectSqlDialog::previousHost="";
QString InjectSqlDialog::previousUser="";


InjectSqlDialog::InjectSqlDialog(DatabaseEngine* engine, QWidget *parent) : QDialog(parent), ui(new Ui::InjectSqlDialog), m_dbEngine(engine)
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

    QVector<QString> databases = m_dbEngine->getAvailableDatabases(ui->txtDatabaseHost->text(), ui->txtDatabaseUser->text(), ui->txtDatabasePassword->text());
    if(databases.size() == 0)
    {
        QMessageBox::critical(this, tr("Error"), m_dbEngine->getLastError(), QMessageBox::Ok)        ;
        return;
    }

    for(int i=0; i<databases.size(); i++)
    {
        ui->cmbDatabases->addItem(databases.at(i));
    }
    ui->cmbDatabases->setEnabled(true);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(false);
    previousHost = ui->txtDatabaseHost->text();
    previousUser = ui->txtDatabaseUser->text();
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

void InjectSqlDialog::setupForReverseEngineering()
{
    ui->chkRollbackOnError->hide();
}
