#include "InjectSqlDialog.h"
#include "ui_InjectSqlDialog.h"

#include "IconFactory.h"
#include "Codepage.h"
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


InjectSqlDialog::InjectSqlDialog(DatabaseEngine* engine, QWidget *parent, Version *v) :
    QDialog(parent), ui(new Ui::InjectSqlDialog), m_dbEngine(engine),
    m_nameWasChanged(false), m_injectMetadata(false)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(true);

    ui->txtDatabaseHost->setText(previousHost.length()?previousHost:"localhost");
    ui->txtDatabaseUser->setText(previousUser);
    ui->chkSavePassword->hide();
    ui->chkAutoConnect->hide();
    ui->cmbDatabases->hide();
    ui->btnConnect->hide();
    ui->btnCreateDatabase->hide();
    ui->grpConnectionDetails->hide();

    if(engine)
    {
        populateConnections();
    }
    else
    {
        setupForConnectionStorage();
        m_dbEngine = MySQLDatabaseEngine::instance(); // TODO: For now ...
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

    QStringList databases = m_dbEngine->getAvailableDatabases(ui->txtDatabaseHost->text(), ui->txtDatabaseUser->text(), ui->txtDatabasePassword->text());
    if(databases.size() == 0)
    {
        QMessageBox::critical(this, tr("Error"), m_dbEngine->getLastError(), QMessageBox::Ok)        ;
        return;
    }

    ui->cmbDatabases->clear();
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

bool InjectSqlDialog::injectMetadataRequired() const
{
    return ui->chkInjectMetadata->isChecked();
}

bool InjectSqlDialog::getAutoConnect() const
{
    return ui->chkAutoConnect->isChecked();
}

void InjectSqlDialog::setupForConnectionStorage()
{
    setupForReverseEngineering();
    ui->grpConnectionDetails->show();

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
    ui->frame->hide();
    ui->verticalLayout_4->setMargin(0);
    ui->verticalLayout_4->setSpacing(0);

    setWindowTitle(QObject::tr("Connection Details"));
    ui->tabWidget->setTabText(0, tr("Connection"));
    ui->tabWidget->setTabIcon(0, IconFactory::getDatabaseIcon());
    ui->tabWidget->removeTab(1);
    resize(450, 400);
}

void InjectSqlDialog::setupForReverseEngineering()
{
    ui->chkRollbackOnError->hide();
    ui->chkInjectMetadata->hide();
}

void InjectSqlDialog::setupForBindToDatabase()
{
    setupForReverseEngineering();
    ui->tabWidget->removeTab(1);
    ui->chkAutoConnect->hide();
    ui->chkSavePassword->hide();
    ui->txtConnectionName->hide();
    ui->lblConnectionName->hide();
    ui->btnCreateDatabase->hide();
    ui->line->hide();
    resize(450, 300);
    setWindowTitle(QObject::tr("Bind to Deployment"));
    ui->grpConnectionDetails->setTitle("Database Details");
    ui->tabWidget->setTabText(0, tr("Source"));
    ui->tabWidget->setTabIcon(0, IconFactory::getBindToDbIcon());

}

void InjectSqlDialog::populateConnections()
{
    const QVector<Connection*>& connections = ConnectionManager::instance()->connections();
    for(int i=0; i<connections.size(); i++)
    {
        QListWidgetItem* lwi = new QListWidgetItem(ui->lstAllConnections);
        lwi->setText(connections.at(i)->getName() + " ("+ connections.at(i)->getDb()+"@"+ connections.at(i)->getHost() + ")");   // TODO: This should be done with setData but I'm lasy now
        lwi->setIcon(IconFactory::getConnectionStateIcon(connections.at(i)->getState()));
    }
}

void InjectSqlDialog::onCreateDatabase()
{
    SimpleTextInputDialog* dlg = new SimpleTextInputDialog(this, tr("Enter the name of the new database"));
    dlg->setModal(true);
    dlg->setText(tr("database"));
    if(dlg->exec() == QDialog::Accepted)
    {
        QString t = dlg->getText();
        Connection* c = new Connection("temp", ui->txtDatabaseHost->text(), ui->txtDatabaseUser->text(),  ui->txtDatabasePassword->text(), dlg->getText(), false, false);

        bool b = m_dbEngine->createDatabase(c);
        if(!b)
        {
            QMessageBox::critical(this, tr("Error"), tr("Could not create a new database: ") + m_dbEngine->getLastError(), QMessageBox::Ok);
        }
        else
        {
            ui->cmbDatabases->addItem(t);
            ui->cmbDatabases->setCurrentIndex(ui->cmbDatabases->findText(t));
        }

        delete c;
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

    ui->txtDatabaseHost->setReadOnly(true);
    ui->txtDatabaseUser->setReadOnly(true);
    ui->txtDatabasePassword->setReadOnly(true);
    ui->txtConnectionName->setReadOnly(true);
    ui->txtDatabaseName->setReadOnly(true);

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
    ui->txtConnectionName->setText("localhost");
    ui->txtDatabaseHost->setText("localhost");
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

void InjectSqlDialog::onHostChange(QString newText)
{
    if(!m_nameWasChanged)
    {
        QString user = ui->txtDatabaseUser->text();
        QString finalName = user.length()?user + "@" +
                                          ((newText.length()>0)?newText:"localhost")
                                          :((newText.length()>0)?newText:"localhost");
        ui->txtConnectionName->setText(finalName);
    }
}

void InjectSqlDialog::onUserChange(QString newText)
{
    if(!m_nameWasChanged)
    {
        QString db = ui->txtDatabaseHost->text();
        QString finalName = newText.length()?newText + "@" + (db.length()?db:"localhost"):(db.length()?db:"localhost");
        ui->txtConnectionName->setText(finalName);
    }
}
