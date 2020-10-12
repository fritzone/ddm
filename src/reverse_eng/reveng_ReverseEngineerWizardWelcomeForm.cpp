#include "reveng_ReverseEngineerWizardWelcomeForm.h"
#include "ui_ReverseEngineerWizardWelcomeForm.h"
#include "IconFactory.h"

#include <QFileDialog>
#include <QDebug>

ReverseEngineerWizardWelcomeForm::ReverseEngineerWizardWelcomeForm(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::ReverseEngineerWizardWelcomeForm), m_mode()
{
    ui->setupUi(this);
    setTitle(tr("Connection"));
    setSubTitle(tr("Please select the database you want to reverse engineer"));
    QIcon*p = new QIcon(icons::strFroggieIcon);
    QIcon*p1 = new QIcon(icons::strIcon);
    setPixmap(QWizard::WatermarkPixmap, p->pixmap(150, 187));
    setPixmap(QWizard::LogoPixmap, p1->pixmap(32, 32));
}

ReverseEngineerWizardWelcomeForm::~ReverseEngineerWizardWelcomeForm()
{
    delete ui;
}

QString ReverseEngineerWizardWelcomeForm::getHost()
{
    return ui->txtDatabaseHost->text();
}

QString ReverseEngineerWizardWelcomeForm::getUser()
{
    return ui->txtDatabaseUser->text();
}

QString ReverseEngineerWizardWelcomeForm::getPassword()
{
    return ui->txtDatabasePassword->text();
}

int ReverseEngineerWizardWelcomeForm::getPort()
{
    return ui->txtPort->text().toInt();
}

QString ReverseEngineerWizardWelcomeForm::getDatabase()
{
    return ui->txtDatabaseName->text();
}

void ReverseEngineerWizardWelcomeForm::setMySqlMode()
{
    ui->tabWidget->removeTab(1);
    ui->tabWidget->setTabIcon(0, IconFactory::getMySqlIcon());
    ui->tabWidget->setTabText(0, strCamelMySql);

    ui->txtDatabaseName->hide();
    ui->lblDatabaseName->hide();

    m_mode = strMySql;
}

void ReverseEngineerWizardWelcomeForm::setSqliteMode()
{
    ui->tabWidget->removeTab(0);
    m_mode = strSqlite;
}

void ReverseEngineerWizardWelcomeForm::setCubridMode()
{
    ui->tabWidget->removeTab(1);
    ui->tabWidget->setTabIcon(0, IconFactory::getCUBRIDIcon());
    ui->tabWidget->setTabText(0, strCUBRID);

    ui->txtDatabaseName->show();
    ui->lblDatabaseName->show();

    m_mode = strCUBRID;
}

void ReverseEngineerWizardWelcomeForm::setPostgresqlMode()
{
    ui->tabWidget->removeTab(1);
    ui->tabWidget->setTabIcon(0, IconFactory::getPostgresIcon());
    ui->tabWidget->setTabText(0, strPostgres);

    ui->txtDatabaseName->hide();
    ui->lblDatabaseName->hide();

    m_mode = strPostgres;

}

QString ReverseEngineerWizardWelcomeForm::getSqliteFile()
{
    return ui->txtSqliteFilename->text();
}

int ReverseEngineerWizardWelcomeForm::getSqliteVersion()
{
    if(ui->cmbSqliteVersion->currentText().contains("3")) return 3;
    return 2;
}

void ReverseEngineerWizardWelcomeForm::onBrowseFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Select Database file"), "",
                                                    tr("Sqlite files (*.sqlite);;All files (*.*)"), 0,
                                                    QFileDialog::DontConfirmOverwrite | QFileDialog::ReadOnly);
    if(fileName.length() == 0)
    {
        return;
    }
    if(!fileName.endsWith(".sqlite")) fileName += ".sqlite";
    ui->txtSqliteFilename->setText(fileName);
}
