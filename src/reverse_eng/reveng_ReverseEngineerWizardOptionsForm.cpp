#include "reveng_ReverseEngineerWizardOptionsForm.h"
#include "ui_ReverseEngineerWizardOptionsForm.h"
#include "IconFactory.h"

ReverseEngineerWizardOptionsForm::ReverseEngineerWizardOptionsForm(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::ReverseEngineerWizardOptionsForm)
{
    ui->setupUi(this);

    setTitle(tr("Options"));
    setSubTitle(tr("Please specify the reverse engineering options."));
    QIcon*p = new QIcon(icons::strFroggieIcon);
    QIcon*p1 = new QIcon(icons::strIcon);
    setPixmap(QWizard::WatermarkPixmap, p->pixmap(150, 187));
    setPixmap(QWizard::LogoPixmap, p1->pixmap(32, 32));

    ui->lblWarning->setVisible(false);

}

ReverseEngineerWizardOptionsForm::~ReverseEngineerWizardOptionsForm()
{
    delete ui;
}

void ReverseEngineerWizardOptionsForm::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

bool ReverseEngineerWizardOptionsForm::createDataTypesForColumns()
{
    return ui->checkBox->isChecked();
}

void ReverseEngineerWizardOptionsForm::onCheckChangee(bool a)
{
    ui->lblWarning->setVisible(!a);
}
