#include "ReverseEngineerWizardWelcomeForm.h"
#include "ui_ReverseEngineerWizardWelcomeForm.h"
#include "strings.h"

ReverseEngineerWizardWelcomeForm::ReverseEngineerWizardWelcomeForm(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::ReverseEngineerWizardWelcomeForm)
{
    ui->setupUi(this);
    setTitle(tr("Connection"));
    setSubTitle(tr("Please specify the connection settings for the database you want to reverse engineer"));
    QIcon*p = new QIcon(strFroggieIcon);
    QIcon*p1 = new QIcon(strIcon);
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
