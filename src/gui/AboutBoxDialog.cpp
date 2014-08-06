#include "AboutBoxDialog.h"
#include "ui_AboutBoxDialog.h"
#include "strings.h"

#include <QSqlDatabase>

#include <QDebug>

AboutBoxDialog::AboutBoxDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutBoxDialog)
{
    ui->setupUi(this);

    QStringList supportedDbs = QSqlDatabase::drivers();
    qDebug() << supportedDbs;
    QString supportedDatabases = "Supported databases: ";
    if(supportedDbs.contains(strQCUBRID))
    {
        supportedDatabases += strCUBRID + strSpace;
    }
    if(supportedDbs.contains(strQSqlite))
    {
        supportedDatabases += strSqlite + strSpace;
    }
    if(supportedDbs.contains(strQMySql))
    {
        supportedDatabases += strMySql + strSpace;
    }
    if(supportedDbs.contains(strQPostgres))
    {
        supportedDatabases += strPostgres + strSpace;
    }

    ui->textEdit->setText(supportedDatabases);
}

AboutBoxDialog::~AboutBoxDialog()
{
    delete ui;
}

void AboutBoxDialog::changeEvent(QEvent *e)
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
