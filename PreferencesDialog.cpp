#include "PreferencesDialog.h"
#include "Configuration.h"

#include "ui_PreferencesDialog.h"

#include<QMessageBox>

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);

    ui->chkUpcase->setCheckState(Configuration::instance().sqlGenerationOptions()["Case"] == "Upper"?Qt::Checked:Qt::Unchecked);
    ui->chkBackticks->setCheckState(Configuration::instance().sqlGenerationOptions()["Backticks"] == "Yes"?Qt::Checked:Qt::Unchecked);
    ui->chkComments->setCheckState(Configuration::instance().sqlGenerationOptions()["GenerateComments"] == "Yes"?Qt::Checked:Qt::Unchecked);
    ui->chkDrawTableTypes->setCheckState(Configuration::instance().drawTableTypes()?Qt::Checked:Qt::Unchecked);
    ui->chkAllowFkPropagation->setCheckState(Configuration::instance().allowForeignKeyPropagation()?Qt::Checked:Qt::Unchecked);

    ui->cmbPkPos->setCurrentIndex( ui->cmbPkPos->findText(Configuration::instance().sqlGenerationOptions()["PKSposition"]) );
    ui->cmbFkPos->setCurrentIndex( ui->cmbFkPos->findText(Configuration::instance().sqlGenerationOptions()["FKSposition"]) );
    ui->chkContinuousValidation->setCheckState(Configuration::instance().continuousValidation()?Qt::Checked:Qt::Unchecked);

    ui->tabWidget->setCurrentIndex(0);
    ui->grpHelp->setVisible(false);
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

void PreferencesDialog::changeEvent(QEvent *e)
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

void PreferencesDialog::accept()
{
    Configuration::instance().setDrawTabletypes(ui->chkDrawTableTypes->checkState() == Qt::Checked);
    Configuration::instance().setAllowForeignKeyPropagation(ui->chkAllowFkPropagation->checkState() == Qt::Checked);
    Configuration::instance().sqlOpts()["PKSposition"] = ui->cmbPkPos->currentText();
    Configuration::instance().sqlOpts()["FKSposition"] = ui->cmbFkPos->currentText();
    Configuration::instance().sqlOpts()["Case"] = ui->chkUpcase->checkState() == Qt::Checked?"Upper":"Lower";
    Configuration::instance().sqlOpts()["Backticks"] = ui->chkBackticks->checkState() == Qt::Checked?"Yes":"No";
    Configuration::instance().sqlOpts()["GenerateComments"] = ui->chkComments->checkState() == Qt::Checked?"Yes":"No";
    Configuration::instance().setContinuousValidation(ui->chkContinuousValidation->checkState() == Qt::Checked);
    close();
}

void PreferencesDialog::onHelp()
{
    ui->grpHelp->setVisible(true);
}
