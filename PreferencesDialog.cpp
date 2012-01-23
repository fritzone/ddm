#include "PreferencesDialog.h"
#include "Configuration.h"
#include "gui_HelpWindow.h"
#include "ui_PreferencesDialog.h"
#include "strings.h"

#include<QMessageBox>

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);

    ui->chkUpcase->setCheckState(Configuration::instance().sqlGenerationOptions()[strCase] == strUpper?Qt::Checked:Qt::Unchecked);
    ui->chkBackticks->setCheckState(Configuration::instance().sqlGenerationOptions()[strBackticks] == strYes?Qt::Checked:Qt::Unchecked);
    ui->chkComments->setCheckState(Configuration::instance().sqlGenerationOptions()[strGenerateComments] == strYes?Qt::Checked:Qt::Unchecked);
    ui->chkDrawTableTypes->setCheckState(Configuration::instance().drawTableTypes()?Qt::Checked:Qt::Unchecked);
    ui->chkAllowFkPropagation->setCheckState(Configuration::instance().allowForeignKeyPropagation()?Qt::Checked:Qt::Unchecked);

    ui->cmbPkPos->setCurrentIndex( ui->cmbPkPos->findText(Configuration::instance().sqlGenerationOptions()[strPKSposition]) );
    ui->cmbFkPos->setCurrentIndex( ui->cmbFkPos->findText(Configuration::instance().sqlGenerationOptions()[strFKSposition]) );
    ui->chkContinuousValidation->setCheckState(Configuration::instance().continuousValidation()?Qt::Checked:Qt::Unchecked);

    ui->tabWidget->setCurrentIndex(0);
    ui->tabWidget->removeTab(1);
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
    Configuration::instance().sqlOpts()[strPKSposition] = ui->cmbPkPos->currentText();
    Configuration::instance().sqlOpts()[strFKSposition] = ui->cmbFkPos->currentText();
    Configuration::instance().sqlOpts()[strCase] = ui->chkUpcase->checkState() == Qt::Checked?strUpper:strLower;
    Configuration::instance().sqlOpts()[strBackticks] = ui->chkBackticks->checkState() == Qt::Checked?strYes:strNo;
    Configuration::instance().sqlOpts()[strGenerateComments] = ui->chkComments->checkState() == Qt::Checked?strYes:strNo;
    Configuration::instance().setContinuousValidation(ui->chkContinuousValidation->checkState() == Qt::Checked);
    close();
}

void PreferencesDialog::onHelp()
{
    HelpWindow* hw = HelpWindow::instance();
    hw->showHelp(QString("/doc/config.html"));
    hw->show();
}
