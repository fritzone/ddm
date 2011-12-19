#include "ReverseEngineerWizardDatabasesForm.h"
#include "ui_ReverseEngineerWizardDatabasesForm.h"
#include "IconFactory.h"

ReverseEngineerWizardDatabasesForm::ReverseEngineerWizardDatabasesForm(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::ReverseEngineerWizardDatabasesForm)
{
    ui->setupUi(this);

    setTitle(tr("Select Database"));
    setSubTitle(tr("Please select the database you want to reverse engineer."));
    QIcon*p = new QIcon(strFroggieIcon);
    QIcon*p1 = new QIcon(strIcon);
    setPixmap(QWizard::WatermarkPixmap, p->pixmap(150, 187));
    setPixmap(QWizard::LogoPixmap, p1->pixmap(32, 32));
}

ReverseEngineerWizardDatabasesForm::~ReverseEngineerWizardDatabasesForm()
{
    delete ui;
}

void ReverseEngineerWizardDatabasesForm::addDatabase(const QString & name)
{
    QListWidgetItem* lwi = new QListWidgetItem(name, ui->listWidget);
    QIcon c = IconFactory::getDatabaseIcon();
    QIcon b = QIcon(c.pixmap(16,16));
    lwi->setIcon(b);
}

QString ReverseEngineerWizardDatabasesForm::getSelectedDatabase() const
{
    if(ui->listWidget->selectedItems().size() > 0) return ui->listWidget->selectedItems().at(0)->text();
    return "";
}

void ReverseEngineerWizardDatabasesForm::clearList()
{
    ui->listWidget->clear();
}
