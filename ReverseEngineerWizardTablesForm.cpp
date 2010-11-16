#include "ReverseEngineerWizardTablesForm.h"
#include "ui_ReverseEngineerWizardTablesForm.h"

ReverseEngineerWizardTablesForm::ReverseEngineerWizardTablesForm(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::ReverseEngineerWizardTablesForm)
{
    ui->setupUi(this);
}

ReverseEngineerWizardTablesForm::~ReverseEngineerWizardTablesForm()
{
    delete ui;
}
