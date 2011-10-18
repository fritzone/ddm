#include "InjectSqlDialog.h"
#include "ui_InjectSqlDialog.h"

#include "Codepage.h"
#include "db_AbstractCodepageSupplier.h"
#include "DatabaseEngine.h"
#include "SimpleTextInputDialog.h"

#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QListWidget>
#include <QListWidgetItem>

QString InjectSqlDialog::previousHost="";
QString InjectSqlDialog::previousUser="";


InjectSqlDialog::InjectSqlDialog(DatabaseEngine* engine, QWidget *parent) : QDialog(parent), ui(new Ui::InjectSqlDialog), m_dbEngine(engine)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(true);
    ui->chkOnlyIfNotExist->hide();
    ui->txtDatabaseHost->setText(previousHost);
    ui->txtDatabaseUser->setText(previousUser);

    populateCodepageCombo();
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
    ui->btnCreateDatabase->setEnabled(true);
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

void InjectSqlDialog::selectCodePage(int i)
{
    ui->cmbCharacterSets->setCurrentIndex(i);
}

void InjectSqlDialog::populateCodepageCombo()
{
    QVector<Codepage*> cps = m_dbEngine->getCodepageSupplier()->getCodepages();
    QListWidget* lw = new QListWidget(this);
    for(int i=0; i<cps.size(); i++)
    {
        QString name = cps[i]->getName();
        bool header = false;
        if(cps[i]->getName().startsWith(QString("--")))
        {
            header = true;
            name = name.right(name.length() - 2);
        }
        QString iconName = "";

        if(!header)
        {
        // dig out the second string
            QStringList ls = name.split("_");
            if(ls.size() > 1)
            {

                if(ls[1] != "bin" && ls[1] != "unicode" && ls[1] != "general")
                {
                    iconName = ":/images/actions/images/small/flag_" + ls[1] + ".png";
                }
                else
                {
                    if(ls[0] == "greek")
                    {
                        iconName = ":/images/actions/images/small/flag_greek.png";
                    }
                    else
                    if(ls[0] == "armscii8")
                    {
                        iconName = ":/images/actions/images/small/flag_armenia.png";
                    }
                    else
                    if(ls[0] == "hebrew")
                    {
                        iconName = ":/images/actions/images/small/flag_israel.png";
                    }
                    else
                    {
                        iconName = ":/images/actions/images/small/flag_" + ls[1] + ".pmg";
                    }
                }

                ls[1][0] = ls[1][0].toUpper();

                name = ls[1] + " (" + ls[0];
                if(ls.size() > 2)
                {
                    name += ", " + ls[2];
                }
                name += ")";
            }
        }

        // create the lw object
        QListWidgetItem* lwi = new QListWidgetItem(name);
        QFont font = lwi->font();
        if(iconName.length() > 0)
        {
            lwi->setIcon(QIcon(iconName));
        }

        if(header)
        {
            font.setBold(true);
            font.setItalic(true);
            font.setPointSize(font.pointSize() + 1);
        }

        lwi->setFont(font);

        lw->addItem(lwi);
    }

    ui->cmbCharacterSets->setModel(lw->model());
    ui->cmbCharacterSets->setView(lw);
    ui->cmbCharacterSets->setCurrentIndex(-1);

}

QString InjectSqlDialog::getCodepage()
{
    QString s = ui->cmbCharacterSets->itemData(ui->cmbCharacterSets->currentIndex()).toString();
    s=s.left(s.indexOf('_'));
    return s;
}

void InjectSqlDialog::onCreateDatabase()
{
    SimpleTextInputDialog* dlg = new SimpleTextInputDialog(this, tr("Enter the name of the new database"));
    dlg->setModal(true);
    dlg->setText(tr("database"));
    if(dlg->exec() == QDialog::Accepted)
    {
        QString t = dlg->getText();
        bool b = m_dbEngine->createDatabase(ui->txtDatabaseHost->text(), ui->txtDatabaseUser->text(), ui->txtDatabasePassword->text(), t);
        if(!b)
        {
            QMessageBox::critical(this, tr("Error"), tr("Could not create a new database: ") + m_dbEngine->getLastError(), QMessageBox::Ok);
        }
        else
        {
            ui->cmbDatabases->addItem(t);
            ui->cmbDatabases->setCurrentIndex(ui->cmbDatabases->findText(t));
        }
    }
}
