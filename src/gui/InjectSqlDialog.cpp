#include "InjectSqlDialog.h"
#include "ui_InjectSqlDialog.h"

#include "IconFactory.h"
#include "db_Codepage.h"
#include "db_DatabaseEngine.h"
#include "SimpleTextInputDialog.h"
#include "dbmysql_DatabaseEngine.h"
#include "dbpostgres_DatabaseEngine.h"
#include "ConnectionManager.h"
#include "Connection.h"
#include "core_Table.h"
#include "core_TableInstance.h"
#include "Version.h"
#include "Workspace.h"
#include "core_View.h"
#include "core_Procedure.h"
#include "core_Trigger.h"
#include "core_Function.h"
#include "MySqlConnection.h"
#include "SqliteConnection.h"
#include "conn_CUBRID.h"
#include "conn_Postgres.h"
#include "gui_HelpWindow.h"

#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QListWidget>
#include <QListWidgetItem>
#include <QFileDialog>
#include <QToolTip>
#include <QHeaderView>

QString InjectSqlDialog::previousHost="";
QString InjectSqlDialog::previousUser="";


InjectSqlDialog::InjectSqlDialog(DatabaseEngine* engine, QWidget *parent, Version *v, const QString &objNameToDeploy) :
    QDialog(parent), ui(new Ui::InjectSqlDialog), m_dbEngine(engine),
    m_nameWasChanged(false), m_injectMetadata(false),
    m_signalMapper(new QSignalMapper(this)),
    m_UidsToDeploy(), m_UidsToDrop(), m_objName(objNameToDeploy),
    m_alreadyConnected(false), m_cubridTested(false),
    m_strDbEngine(strMySql.toUpper()),
    m_mode(MODE_MYSQL)
{
    ui->setupUi(this);

    ui->cmbSqliteVersion->hide();
    ui->frame_2->hide();
    disableOkButton();
    ui->txtDatabaseHost->setText(previousHost.length()?previousHost:"localhost");
    ui->txtDatabaseUser->setText(previousUser);
    ui->chkSavePassword->hide();
    ui->chkAutoConnect->hide();
    ui->cmbDatabases->hide();
    ui->btnConnect->hide();
    ui->btnCreateDatabase->hide();
    ui->grpConnectionDetails->hide();
    ui->chkRollbackOnError->hide();
    ui->tabWidget->removeTab(1);
    ui->btnTestConnection->setVisible(false);

    if(engine)
    {
        populateConnections();
    }
    else
    {
        setupForConnectionStorage();

        // initially the supported engine is MySql
        m_dbEngine = MySQLDatabaseEngine::instance();
    }

    clearConnectionDetails();

    if(v)
    {
        ui->lblVersionToBeDeployed->setText(QObject::tr("Deploying: <b>Version ") + v->getVersionText());
        // create the tables
        QTreeWidgetItem* tabItem = new QTreeWidgetItem(QStringList("Tables"));
        tabItem->setIcon(0, IconFactory::getTableIcon());
        if(Workspace::getInstance()->currentProjectIsOop())
        {
            tabItem->setIcon(0, IconFactory::getTabinstIcon());
        }
        ui->treeObjectsToDeploy->addTopLevelItem(tabItem);

        if(Workspace::getInstance()->currentProjectIsOop())
        {
            QVector <TableInstance*> tabs = v->getTableInstances();
            for(int i=0; i<tabs.size(); i++)
            {
                createTreeItem(tabItem, tabs.at(i)->getName(), tabs.at(i)->getObjectUid().toString(), IconFactory::getTabinstIcon());
            }
        }
        else
        {
            QVector <Table*> tabs = v->getTables();
            for(int i=0; i<tabs.size(); i++)
            {
                createTreeItem(tabItem, tabs.at(i)->getName(), tabs.at(i)->getObjectUid().toString(), IconFactory::getTableIcon());
            }
        }
        tabItem->setExpanded(true);

        // views
        QTreeWidgetItem* viewsItem = new QTreeWidgetItem(QStringList("Views"));
        viewsItem->setIcon(0, IconFactory::getViewsIcon());
        ui->treeObjectsToDeploy->addTopLevelItem(viewsItem);

        QVector <View*> views = v->getViews();
        for(int i=0; i<views.size(); i++)
        {
            createTreeItem(viewsItem, views.at(i)->getName(), views.at(i)->getObjectUid().toString(), IconFactory::getViewIcon());
        }
        viewsItem->setExpanded(true);

        // procedures
        QTreeWidgetItem* proceduresItem = new QTreeWidgetItem(QStringList("Procedures"));
        proceduresItem->setIcon(0, IconFactory::getProceduresIcon());
        ui->treeObjectsToDeploy->addTopLevelItem(proceduresItem);

        QVector <Procedure*> procedures = v->getProcedures();
        for(int i=0; i<procedures.size(); i++)
        {
            createTreeItem(proceduresItem, procedures.at(i)->getName(), procedures.at(i)->getObjectUid().toString(), IconFactory::getProcedureIcon());
        }
        proceduresItem->setExpanded(true);

        // functions
        QTreeWidgetItem* functionsItem = new QTreeWidgetItem(QStringList("Functions"));
        functionsItem->setIcon(0, IconFactory::getFunctionsTreeIcon());
        ui->treeObjectsToDeploy->addTopLevelItem(functionsItem);

        QVector <Function*> functions = v->getFunctions();
        for(int i=0; i<functions.size(); i++)
        {
            createTreeItem(functionsItem, functions.at(i)->getName(), functions.at(i)->getObjectUid().toString(), IconFactory::getFunctionTreeIcon());
        }
        functionsItem->setExpanded(true);

        // triggers
        QTreeWidgetItem* triggersItem = new QTreeWidgetItem(QStringList("Triggers"));
        triggersItem->setIcon(0, IconFactory::getTriggersIcon());
        ui->treeObjectsToDeploy->addTopLevelItem(triggersItem);

        QVector <Trigger*> triggers = v->getTriggers();
        for(int i=0; i<triggers.size(); i++)
        {
            createTreeItem(triggersItem, triggers.at(i)->getName(), triggers.at(i)->getObjectUid().toString(), IconFactory::getTriggerIcon());
        }
        triggersItem->setExpanded(true);
        #if QT_VERSION >= 0x050000
        ui->treeObjectsToDeploy->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        #else
        ui->treeObjectsToDeploy->header()->setResizeMode(0, QHeaderView::ResizeToContents);
        #endif



        connect(m_signalMapper, SIGNAL(mapped(QString)), this, SLOT(checkBoxToggled(QString)));
    }
    else
    {
        if(objNameToDeploy.length())
        {
            ui->lblVersionToBeDeployed->setText(QObject::tr("Deploying: <b>") + objNameToDeploy);
        }
        else
        {
            ui->lblVersionToBeDeployed->hide();
        }
    }

    // and now remove the unneeded radio buttons from the GUI. Ugly, just hardcode it.
    QStringList supportedDbs = QSqlDatabase::drivers();
    if(!supportedDbs.contains(strQCUBRID))
    {
        ui->cmbDatabaseType->removeItem(ui->cmbDatabaseType->findText(strCUBRID, Qt::MatchExactly ));
    }
    if(!supportedDbs.contains(strQSqlite))
    {
        ui->cmbDatabaseType->removeItem(ui->cmbDatabaseType->findText(strCamelSqlite, Qt::MatchExactly ));
    }
    if(!supportedDbs.contains(strQMySql))
    {
        ui->cmbDatabaseType->removeItem(ui->cmbDatabaseType->findText(strMySql, Qt::MatchExactly ));
    }
    if(!supportedDbs.contains(strQPostgres))
    {
        ui->cmbDatabaseType->removeItem(ui->cmbDatabaseType->findText(strPostgres, Qt::MatchExactly ));
    }}

void InjectSqlDialog::createTreeItem(QTreeWidgetItem* parent, const QString &text, const QString &uid, const QIcon &icon)
{
    QTreeWidgetItem* tabItemI = new QTreeWidgetItem(parent, QStringList(text));
    ui->treeObjectsToDeploy->addTopLevelItem(tabItemI);
    tabItemI->setIcon(0, icon);

    QCheckBox* checkToDeploy = new QCheckBox();
    ui->treeObjectsToDeploy->setItemWidget(tabItemI, 1, checkToDeploy);
    m_signalMapper->setMapping(checkToDeploy, QString("#") + uid);
    connect(checkToDeploy, SIGNAL(clicked()), m_signalMapper, SLOT(map()));
    checkToDeploy->setChecked(true);
    m_UidsToDeploy.append(uid);

    QCheckBox* checkToDrop = new QCheckBox();
    ui->treeObjectsToDeploy->setItemWidget(tabItemI, 2, checkToDrop);
    m_signalMapper->setMapping(checkToDrop, QString("@") + uid);
    connect(checkToDrop, SIGNAL(clicked()), m_signalMapper, SLOT(map()));
}

void InjectSqlDialog::toggleUidBelongeness(QStringList &lst, const QString &s)
{
    if(lst.contains(s))
    {
        lst.removeOne(s);
    }
    else
    {
        lst.append(s);
    }
}

void InjectSqlDialog::checkBoxToggled(QString a)
{
    if(a.at(0) == '#')
    {
        toggleUidBelongeness(m_UidsToDeploy, a.mid(1));
    }
    else
    {
        toggleUidBelongeness(m_UidsToDrop, a.mid(1));
    }
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
    {
        QString oldConnName = ui->txtConnectionName->text();
        ui->retranslateUi(this);
        ui->txtConnectionName->setText(oldConnName);
        break;
    }
    default:
        break;
    }
}

void InjectSqlDialog::onConnect()
{

    QStringList databases = m_dbEngine->getAvailableDatabases(ui->txtDatabaseHost->text(),
                                                              ui->txtDatabaseUser->text(),
                                                              ui->txtDatabasePassword->text(),
                                                              ui->txtPort->text().toInt());
    if(databases.size() == 0)
    {
        QMessageBox::critical(this, tr("Error"), m_dbEngine->getLastError(), QMessageBox::Ok);
        return;
    }

    QString t = ui->cmbDatabases->currentText();
    ui->cmbDatabases->clear();
    for(int i=0; i<databases.size(); i++)
    {
        ui->cmbDatabases->addItem(databases.at(i));
    }
    ui->cmbDatabases->setCurrentIndex(ui->cmbDatabases->findText(t));
    ui->cmbDatabases->setEnabled(true);
    ui->lblDatabase->setEnabled(true);
    m_alreadyConnected = true;
    enableOkButton();
    ui->btnCreateDatabase->setEnabled(true);
    previousHost = ui->txtDatabaseHost->text();
    previousUser = ui->txtDatabaseUser->text();
}

QString InjectSqlDialog::getDatabase() const
{
    if(m_mode == MODE_MYSQL || m_mode == MODE_POSTGRES)
    {
        return ui->cmbDatabases->currentText();
    }
    else
    {
        return ui->txtDatabaseName->text();
    }
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
    return false;
//    return ui->chkRollbackOnError->isChecked();
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

    ui->btnBrowseForFile->hide();

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

    ui->lblVersionToBeDeployed->hide();
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
        // add only the connections where the database is the same as the current project
        QString dbType = connections[i]->getDbType().toUpper() ;
        QString dbName =m_dbEngine->getDatabaseEngineName().toUpper();
        //qDebug() << dbType << dbName;
        if(m_dbEngine && dbType == dbName)
        {
            QListWidgetItem* lwi = new QListWidgetItem(ui->lstAllConnections);
            lwi->setText(connections.at(i)->getName() + " (" + connections.at(i)->getFullLocation() + ")");   // TODO: This should be done with setData but I'm lasy now
            lwi->setIcon(IconFactory::getConnectionStateIcon(connections.at(i)->getState()));
        }
    }
}

void InjectSqlDialog::onCreateDatabase()
{
    SimpleTextInputDialog* dlg = new SimpleTextInputDialog(this, tr("Enter the name of the new database"), tr("database"));
    if(dlg->exec() == QDialog::Accepted)
    {
        QString t = dlg->getText();
        MySqlConnection* c = new MySqlConnection("temp",
                                       ui->txtDatabaseHost->text(),
                                       ui->txtDatabaseUser->text(),
                                       ui->txtDatabasePassword->text(),
                                       dlg->getText(), false, false, ui->txtPort->text().toInt());

        bool b = m_dbEngine->createDatabase(c);
        if(!b)
        {
            QMessageBox::critical(this, tr("Error"), m_dbEngine->getLastError(), QMessageBox::Ok);
        }
        else
        {
            ui->cmbDatabases->addItem(t);
            ui->cmbDatabases->setCurrentIndex(ui->cmbDatabases->findText(t));
        }

        delete c;
    }
}

void InjectSqlDialog::disableOkButton()
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(true);
}

void InjectSqlDialog::enableOkButton()
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(false);
}

void InjectSqlDialog::onSelectConnection(QListWidgetItem* item)
{
    if(!item)
    {
        disableOkButton();
        return;
    }

    QString connectionName =item->text();
    connectionName = connectionName.left(connectionName.indexOf("(")).trimmed();
    Connection* c = ConnectionManager::instance()->getConnection(connectionName);
    if(c)
    {
        populateConnectionDetails(c);
    }

    if(ui->lstAllConnections->selectedItems().empty())
    {
        disableOkButton();
    }
    else
    {
        enableOkButton();
    }

    blockSignals(true);
    ui->txtDatabaseHost->setReadOnly(true);
    ui->txtDatabaseUser->setReadOnly(true);
    ui->txtPort->setReadOnly(true);
    ui->txtDatabasePassword->setReadOnly(true);
    ui->txtConnectionName->setReadOnly(true);
    ui->txtDatabaseName->setReadOnly(true);
    blockSignals(false);

}

void InjectSqlDialog::populateConnectionDetails(Connection* conn)
{
    ui->txtConnectionName->setText(conn->getName());

    if(MySqlConnection* c = dynamic_cast<MySqlConnection*>(conn))
    {
        ui->cmbDatabaseType->setCurrentIndex(0);
        setMysqlLayout();
        blockSignals(true);
        ui->txtDatabaseHost->setText(c->getHost());
        ui->txtDatabaseUser->setText(c->getUser());
        ui->txtDatabasePassword->setText(c->getPassword());
        ui->txtPort->setText(QString::number(c->getPort()));
        ui->cmbDatabases->addItem(c->getDb());
        blockSignals(false);
        return;
    }

    if(CUBRIDConnection* c2 = dynamic_cast<CUBRIDConnection*>(conn))
    {
        ui->cmbDatabaseType->setCurrentIndex(2);
        setCUBRIDLayout();
        blockSignals(true);
        ui->txtDatabaseHost->setText(c2->getHost());
        ui->txtDatabaseUser->setText(c2->getUser());
        ui->txtDatabasePassword->setText(c2->getPassword());
        ui->txtDatabaseName->setText(c2->getDb());
        ui->txtPort->setText(QString::number(c2->getPort()));
        blockSignals(false);
        return;
    }

    if(SqliteConnection* c1 = dynamic_cast<SqliteConnection*>(conn))
    {
        ui->cmbDatabaseType->setCurrentIndex(1);
        setSqliteLayout();
        ui->txtDatabaseName->setText(c1->getFileName());
        if(c1->getVersion() == 2) ui->cmbSqliteVersion->setCurrentIndex(1);
    }
}

void InjectSqlDialog::clearConnectionDetails()
{
    ui->txtConnectionName->setText("localhost");
    ui->txtDatabaseHost->setText("localhost");
    ui->txtDatabaseUser->clear();
    ui->txtPort->clear();
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
        QString dbHost = ui->txtDatabaseHost->text();
        QString finalName = newText.length()?newText + "@" + (dbHost.length()?dbHost:"localhost"):(dbHost.length()?dbHost:"localhost");
        ui->txtConnectionName->setText(finalName);
    }
}

int InjectSqlDialog::getPort() const
{
    return ui->txtPort->text().toInt();
}

void InjectSqlDialog::onDbChange(QString newDb)
{
    if(!m_nameWasChanged)
    {
        QString dbHost = ui->txtDatabaseHost->text();
        QString finalName = newDb.length()?newDb + "@" + (dbHost.length()?dbHost:"localhost"):(dbHost.length()?dbHost:"localhost");
        ui->txtConnectionName->setText(finalName);
    }
}

void InjectSqlDialog::onConnectionNameEdited(QString a)
{
    m_nameWasChanged = true;

    if(ConnectionManager::instance()->getConnection(a))
    {
        QPalette *palette = new QPalette();
        palette->setColor(QPalette::Text,Qt::red);
        ui->txtConnectionName->setPalette(*palette);
        QString nt = tr("There si already a connection called <b>") + a;
        QToolTip::showText(ui->txtConnectionName->mapToGlobal(QPoint()), nt);
        ui->txtConnectionName->setToolTip(nt);
    }
    else
    {
        QPalette *palette = new QPalette();
        palette->setColor(QPalette::Text,Qt::black);
        ui->txtConnectionName->setPalette(*palette);
        ui->txtConnectionName->setToolTip(tr("The name of the connection"));
    }
}

void InjectSqlDialog::onDbTypeChange(QString a)
{
//    qDebug() << a;
    m_strDbEngine = a.toUpper();
    if(m_strDbEngine == strSqlite.toUpper())
    {
        setSqliteLayout();
    }
    else
    if(m_strDbEngine == strMySql.toUpper())
    {
        setMysqlLayout();
    }
    else
    if(m_strDbEngine == strPostgres.toUpper())
    {
        setPostgresLayout();
    }
    else
    if(m_strDbEngine == strCUBRID.toUpper())
    {
        setCUBRIDLayout();
    }
}

void InjectSqlDialog::setSqliteLayout()
{
    ui->lblHost->hide();
    ui->lblPassword->hide();
    ui->lblUser->hide();
    ui->lblPort->hide();
    ui->txtDatabaseHost->hide();
    ui->txtDatabasePassword->hide();
    ui->txtDatabaseUser->hide();
    ui->txtPort->hide();
    ui->cmbDatabases->hide();
    ui->btnConnect->hide();
    ui->btnCreateDatabase->hide();

    ui->txtDatabaseName->show();
    ui->btnBrowseForFile->show();
    ui->lblDatabase->setText(tr("Filename"));

    ui->lblDatabase->setEnabled(true);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(ui->txtDatabaseName->text().length() == 0);
    ui->btnTestConnection->setVisible(false);

    ui->cmbSqliteVersion->show();
    m_mode = MODE_SQLITE;

}

void InjectSqlDialog::setMysqlLayout()
{
    ui->lblHost->show();
    ui->lblPassword->show();
    ui->lblUser->show();
    ui->lblPort->show();
    ui->txtDatabaseHost->show();
    ui->txtDatabasePassword->show();
    ui->txtDatabaseUser->show();
    ui->txtPort->show();
    ui->cmbDatabases->show();
    ui->btnConnect->show();
    ui->btnCreateDatabase->show();

    ui->txtDatabaseName->hide();
    ui->btnBrowseForFile->hide();
    ui->lblDatabase->setText(tr("Database"));

    ui->lblDatabase->setEnabled(m_alreadyConnected);

    // set the button state only if there are no uids/objects to deploy, ie, this is
    // a dalog setting up things, connection, etc...
    if(m_UidsToDeploy.isEmpty() && m_objName.isEmpty())
    {
        ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(!m_alreadyConnected);
    }

    ui->cmbSqliteVersion->hide();
    ui->btnTestConnection->setVisible(false);
    m_mode = MODE_MYSQL;
    m_dbEngine = MySQLDatabaseEngine::instance();
}

// TODO: This is almost the same as mysql layout
void InjectSqlDialog::setPostgresLayout()
{
    ui->lblHost->show();
    ui->lblPassword->show();
    ui->lblUser->show();
    ui->lblPort->show();
    ui->txtDatabaseHost->show();
    ui->txtDatabasePassword->show();
    ui->txtDatabaseUser->show();
    ui->txtPort->show();
    ui->cmbDatabases->show();
    ui->btnConnect->show();
    ui->btnCreateDatabase->show();

    ui->txtDatabaseName->hide();
    ui->btnBrowseForFile->hide();
    ui->lblDatabase->setText(tr("Database"));

    ui->lblDatabase->setEnabled(m_alreadyConnected);

    // set the button state only if there are no uids/objects to deploy, ie, this is
    // a dalog setting up things, connection, etc...
    if(m_UidsToDeploy.isEmpty() && m_objName.isEmpty())
    {
        ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(!m_alreadyConnected);
    }

    ui->cmbSqliteVersion->hide();
    ui->btnTestConnection->setVisible(false);
    m_mode = MODE_POSTGRES;

    m_dbEngine = PostgresDatabaseEngine::instance();

}


void InjectSqlDialog::setCUBRIDLayout()
{
    ui->lblHost->show();
    ui->lblPassword->show();
    ui->lblUser->show();
    ui->lblPort->show();
    ui->txtDatabaseHost->show();
    ui->txtDatabasePassword->show();
    ui->txtDatabaseUser->show();
    ui->txtPort->show();
    ui->cmbDatabases->hide();
    ui->btnConnect->hide();
    ui->btnCreateDatabase->hide();

    ui->txtDatabaseName->show();
    ui->btnBrowseForFile->hide();
    ui->lblDatabase->setText(tr("Database"));
    ui->btnTestConnection->setVisible(true);
    ui->lblDatabase->setEnabled(true);

    ui->cmbSqliteVersion->hide();
    m_mode = MODE_CUBRID;
}


void InjectSqlDialog::onSelectFileForSqlite()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setWindowTitle(tr("Specify the Sqlite file"));
    dialog.setNameFilter(tr("Sqlite files (*.sqlite);;All files (*.*)"));
    dialog.setViewMode(QFileDialog::Detail);

    QStringList fileNames;
    if (dialog.exec())
    {
        fileNames = dialog.selectedFiles();
    }

    QString fileName = fileNames.length()? fileNames[0]:"";

    if(fileName.length() == 0)
    {
        return;
    }

    if(!fileName.endsWith(".sqlite"))
    {
        fileName += ".sqlite";
    }

    ui->txtDatabaseName->setText(fileName);
    enableOkButton();
}

void InjectSqlDialog::onSqliteFileNameChange(QString a)
{
    if(a.length() > 0)
    {
        enableOkButton();
    }
    else
    {
        disableOkButton();
    }
}

void InjectSqlDialog::onTestConnection()
{
    if(m_mode == MODE_CUBRID)
    {
        QSqlDatabase cubridDb = QSqlDatabase::addDatabase("QCUBRID");
        cubridDb.setHostName(ui->txtDatabaseHost->text());
        cubridDb.setUserName(ui->txtDatabaseUser->text());
        cubridDb.setPassword(ui->txtDatabasePassword->text());
        cubridDb.setPort(ui->txtPort->text().toInt());
        cubridDb.setDatabaseName(ui->txtDatabaseName->text());

        if(!cubridDb.open())
        {
            QSqlError error = cubridDb.lastError();
            QMessageBox::information(0, tr("Cannot test connection"),
                                     error.databaseText() + "<p>" + error.driverText(),
                                     QMessageBox::Ok);
            m_cubridTested = false;
        }
        else
        {
            QSqlQuery q(cubridDb);
            q.prepare("select LIST_DBS()");
            if(!q.exec())
            {
                QMessageBox::information(0, tr("Cannot execute a query"), tr("Cannot execute <pre>select LIST_DBS()"), QMessageBox::Ok);
            }
            else
            {
                if(q.next())
                {
                    QString v = q.value(0).toString().split(" ").join("<br>");
                    QMessageBox::information(0, tr("Connection Succesful"),
                                             tr("<b>Success</b><p>Following databases were found:<p><pre>") + v,
                                             QMessageBox::Ok);
                }
            }
            cubridDb.close();
            m_cubridTested = true;
        }
    }
}

void InjectSqlDialog::onHelp()
{
    setModal(false);
    HelpWindow* hw = HelpWindow::instance();
    hw->showHelp(QString("/doc/conn.html"));
    hw->show();

}

QString InjectSqlDialog::getFileName() const
{
    return ui->txtDatabaseName->text();
}

int InjectSqlDialog::getSqliteVersion() const
{
    if(ui->cmbSqliteVersion->currentText().contains("2")) return 2;
    else return 3;
}

Connection *InjectSqlDialog::provideConnection()
{
    Connection* c = 0;
    if(getSDbEngine().toUpper() == strMySql.toUpper() || getSDbEngine().toUpper() == strCUBRID.toUpper() || getSDbEngine().toUpper() == strPostgres.toUpper())
    {
        QString host = getHost();
        QString user = getUser();
        QString password = getPassword();
        int port = getPort();
        QString db = getDatabase();

        if(getSDbEngine().toUpper() == strCUBRID.toUpper())
        {
            c = new CUBRIDConnection(getName(), host, user, password, db, false, getAutoConnect(), port);
        }
        else
        if(getSDbEngine().toUpper() == strMySql.toUpper())
        {
            c = new MySqlConnection(getName(), host, user, password, db, false, getAutoConnect(), port);
        }
        else
        if(getSDbEngine().toUpper() == strPostgres.toUpper())
        {
            c = new PostgresConnection(getName(), host, user, password, db, false, getAutoConnect(), port);
        }
    }
    else
    {
        QString dbFile = getFileName();
        c = new SqliteConnection(getName(), dbFile, getAutoConnect(), getSqliteVersion());
    }
    return c;
}
