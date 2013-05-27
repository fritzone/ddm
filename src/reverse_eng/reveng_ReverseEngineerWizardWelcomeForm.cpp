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

void ReverseEngineerWizardWelcomeForm::setMySqlMode()
{
    ui->toolBox->setCurrentIndex(0);
    ui->toolBox->setItemEnabled(1, false);
    m_mode = "MYSQL";
}

void ReverseEngineerWizardWelcomeForm::setSqliteMode()
{
    ui->toolBox->setCurrentIndex(1);
    ui->toolBox->setItemEnabled(0, false);
    m_mode = "SQLITE";
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
