#include "SqlForm.h"
#include "ui_SqlForm.h"
#include "SqlHighlighter.h"
#include "InjectSqlDialog.h"
#include "db_DatabaseEngine.h"
#include "Version.h"
#include "Configuration.h"
#include "TableInstance.h"
#include "Workspace.h"
#include "Project.h"
#include "Version.h"
#include "VersionGuiElements.h"
#include "MainWindow.h"
#include "gui_HelpWindow.h"
#include "core_ConnectionManager.h"
#include "db_AbstractDTSupplier.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QString>
#include <QTextStream>

SqlForm::SqlForm(DatabaseEngine* engine, QWidget *parent) : SourceCodePresenterWidget(parent), ui(new Ui::SqlForm), m_engine(engine), m_codepage("latin1"), m_sourceEntity(0)
{
    ui->setupUi(this);
    highlighter = new SqlHighlighter(ui->txtSql->document(),Workspace::getInstance()->currentProjectsEngine()->getKeywords(),
                                     Workspace::getInstance()->currentProjectsEngine()->getDTSupplier()->numericTypes(),
                                     Workspace::getInstance()->currentProjectsEngine()->getDTSupplier()->booleanTypes(),
                                     Workspace::getInstance()->currentProjectsEngine()->getDTSupplier()->textTypes(),
                                     Workspace::getInstance()->currentProjectsEngine()->getDTSupplier()->blobTypes(),
                                     Workspace::getInstance()->currentProjectsEngine()->getDTSupplier()->dateTimeTypes(),
                                     Workspace::getInstance()->currentProjectsEngine()->getDTSupplier()->miscTypes(),
                                     Workspace::getInstance()->workingVersion()->getTables());
    ui->cmbOptions->hide();
}

SqlForm::~SqlForm()
{
    delete ui;
}

void SqlForm::changeEvent(QEvent *e)
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

void SqlForm::setSource(const QString &src)
{
    ui->txtSql->setText(src);
}

void SqlForm::onInject()
{
    ui->labelDeploymentStatus->setText("");
    InjectSqlDialog* injectDialog = new InjectSqlDialog(Workspace::getInstance()->currentProjectsEngine(), this);
    injectDialog->setModal(true);
    if(injectDialog->exec() == QDialog::Accepted)
    {
        QString tSql;
        QStringList connectionNames = injectDialog->getSelectedConnections();
        bool error = false;
        for(int i=0; i< connectionNames.size(); i++)
        {
            Connection* c = ConnectionManager::instance()->getConnection(connectionNames.at(i));
            if(c)
            {
                QStringList tempSqlList = Workspace::getInstance()->workingVersion()->getSqlScript(m_codepage, false);
                if(!m_engine->executeSql(c, tempSqlList, tSql, injectDialog->getRollbackOnError()))
                {
                    QMessageBox::critical (this, tr("Error"), tr("<B>Cannot execute a query!</B><P>Reason: ") + m_engine->getLastError() + tr(".<P>Query:<PRE>") + tSql+ "</PRE><P>" +
                                           (injectDialog->getRollbackOnError()?tr("Transaction was rolled back."):tr("Transaction was <font color=red><B>NOT</B></font> rolled back, you might have partial data in your database.")), QMessageBox::Ok);
                    error = true;
                }
            }
        }
        MainWindow::instance()->setStatus(QString("SQL injection ") + (error?" failed":" succeeded"), error);
        if(!error) ui->labelDeploymentStatus->setText("Succesful deployment");
    }
}

void SqlForm::onSave()
{
    QString name = QFileDialog::getSaveFileName(this, tr("Save sql"), "", tr("SQL files (*.sql)"));
    QFile file(name);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return;
    }
    QTextStream out(&file);
    out << ui->txtSql->toPlainText() << "\n";
}

void SqlForm::presentSql(Project* p,const QString& codepage)
{
    // here create the final SQL:
    // firstly only the tables and then the foreign keys. We'll see the other elements (triggers, functions) later

    Version *v = p->getWorkingVersion();
    QStringList finalSql = v->getSqlScript(codepage, true);
    m_codepage = codepage;

    QString fs = "";
    for(int i=0; i< finalSql.size(); i++)
    {
        fs += finalSql[i];
    }
    setSource(fs);
    setSqlList(finalSql);
}

void SqlForm::presentSql(Project* p, SqlSourceEntity* ent,const QString& codepage, MainWindow::showSomething)
{
    QString fs = "";
    QHash<QString, QString> opts = Configuration::instance().sqlGenerationOptions();
    opts["FKSposition"] = "OnlyInternal";
    QStringList finalSql = ent->generateSqlSource(p->getEngine()->getSqlGenerator(), opts, codepage);
    for(int i=0; i< finalSql.size(); i++)
    {
        fs += finalSql[i];
    }
    setSource(fs);
    setSqlList(finalSql);
    m_sourceEntity = ent;
}

void SqlForm::onHelp()
{
    HelpWindow* hw = HelpWindow::instance();
    hw->showHelp(QString("/doc/sqls.html"));
    hw->show();
}

void SqlForm::onGoToOriginator()
{
    if(m_sourceEntity)
    {

    }
}
