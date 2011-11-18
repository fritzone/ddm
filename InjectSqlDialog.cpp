#include "InjectSqlDialog.h"
#include "ui_InjectSqlDialog.h"

#include "IconFactory.h"
#include "Codepage.h"
#include "db_AbstractCodepageSupplier.h"
#include "db_DatabaseEngine.h"
#include "SimpleTextInputDialog.h"
#include "dbmysql_MySQLDatabaseEngine.h"
#include "core_ConnectionManager.h"
#include "core_Connection.h"

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
    ui->chkSavePassword->hide();
    ui->chkAutoConnect->hide();
    ui->cmbDatabases->hide();
    ui->btnConnect->hide();
    ui->btnCreateDatabase->hide();
    ui->grpConnectionDetails->hide();

    if(engine)
    {
        populateCodepageCombo();
        populateConnections();
    }
    else
    {
        setupForConnectionStorage();
        m_dbEngine = new MySQLDatabaseEngine(); // TODO: For now ...
    }

    clearConnectionDetails();
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

QString InjectSqlDialog::getDatabase() const
{
    return ui->cmbDatabases->currentText();
}

QString InjectSqlDialog::getUser() const
{
    return ui->txtDatabaseUser->text();
}

QString InjectSqlDialog::getPassword() const
{
    return ui->txtDatabasePassword->text();
}

QString InjectSqlDialog::getHost() const
{
    return ui->txtDatabaseHost->text();
}

QString InjectSqlDialog::getName() const
{
    return ui->txtConnectionName->text();
}

bool InjectSqlDialog::getRollbackOnError() const
{
    return ui->chkRollbackOnError->isChecked();
}

bool InjectSqlDialog::getCreateOnlyIfNotExist() const
{
    return ui->chkOnlyIfNotExist->isChecked();
}

bool InjectSqlDialog::getAutoConnect() const
{
    return ui->chkAutoConnect->isChecked();
}

void InjectSqlDialog::setupForConnectionStorage()
{
    setupForReverseEngineering();
    ui->grpConnectionDetails->show();
    ui->cmbCharacterSets->hide();
    ui->lblCodepage->hide();
    ui->chkOnlyIfNotExist->hide();
    ui->lblConnectionName->show();
    ui->txtConnectionName->show();
    ui->btnConnect->show();
    ui->lblDatabase->setEnabled(false);
    ui->btnCreateDatabase->show();
    ui->chkSavePassword->show();
    ui->chkAutoConnect->show();
    ui->grpConnections->hide();
    ui->txtDatabaseName->hide();
    ui->cmbDatabases->show();
    ui->btnDetails->hide();

    setWindowTitle(QObject::tr("Connection Details"));
    resize(450, 400);
}

void InjectSqlDialog::setupForReverseEngineering()
{
    ui->chkRollbackOnError->hide();
}

void InjectSqlDialog::selectCodePage(int i)
{
    ui->cmbCharacterSets->setCurrentIndex(i);
}

void InjectSqlDialog::populateConnections()
{
    const QVector<Connection*>& connections = ConnectionManager::instance()->connections();
    for(int i=0; i<connections.size(); i++)
    {
        QListWidgetItem* lwi = new QListWidgetItem(ui->lstAllConnections);
        lwi->setText(connections.at(i)->getName() + " ("+ connections.at(i)->getDb()+"@"+ connections.at(i)->getHost() + ")");   // TODO: This should be done with setData but I'm lasy now
        switch(connections.at(i)->getState())
        {
        case Connection::DID_NOT_TRY:
            lwi->setIcon(IconFactory::getDatabaseIcon());
            break;
        case Connection::FAILED:
            lwi->setIcon(IconFactory::getUnConnectedDatabaseIcon());
            break;
        case Connection::CONNECTED:
            lwi->setIcon(IconFactory::getConnectedDatabaseIcon());
            break;
        }
    }
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

//QString InjectSqlDialog::getCodepage() const
//{
//    QString s = ui->cmbCharacterSets->itemData(ui->cmbCharacterSets->currentIndex()).toString();
//    s=s.left(s.indexOf('_'));
//    return s;
//}

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

void InjectSqlDialog::onSelectConnection(QListWidgetItem* item)
{
    QString connectionName =item->text();
    if(ui->lstAllConnections->selectedItems().size())
    {
        ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(false);
    }
    else
    {
        ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(true);
    }
    connectionName = connectionName.left(connectionName.indexOf("(")).trimmed();
    Connection* c = ConnectionManager::instance()->getConnection(connectionName);
    if(c)
    {
        populateConnectionDetails(c);
    }
}

void InjectSqlDialog::populateConnectionDetails(Connection* c)
{
    ui->txtConnectionName->setText(c->getName());
    ui->txtDatabaseHost->setText(c->getHost());
    ui->txtDatabaseUser->setText(c->getUser());
    ui->txtDatabasePassword->setText(c->getPassword());
    ui->txtDatabaseName->setText(c->getDb());
}

void InjectSqlDialog::clearConnectionDetails()
{
    ui->txtConnectionName->clear();
    ui->txtDatabaseHost->clear();;
    ui->txtDatabaseUser->clear();
    ui->txtDatabasePassword->clear();
    ui->txtDatabaseName->clear();
}

QStringList InjectSqlDialog::getSelectedConnections() const
{
    QStringList result;
    for(int i=0; i<ui->lstAllConnections->selectedItems().size(); i++)
    {
        result.append( ui->lstAllConnections->selectedItems().at(i)->text().left(ui->lstAllConnections->selectedItems().at(i)->text().indexOf("(")).trimmed() );
    }
    return result;
}
