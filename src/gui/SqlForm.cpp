#include "SqlForm.h"
#include "ui_SqlForm.h"
#include "SqlHighlighter.h"
#include "InjectSqlDialog.h"
#include "db_DatabaseEngine.h"
#include "Version.h"
#include "Configuration.h"
#include "core_TableInstance.h"
#include "Workspace.h"
#include "Project.h"
#include "Version.h"
#include "VersionGuiElements.h"
#include "MainWindow.h"
#include "gui_HelpWindow.h"
#include "ConnectionManager.h"
#include "db_AbstractDTSupplier.h"
#include "core_Procedure.h"
#include "core_Function.h"
#include "core_Trigger.h"
#include "core_View.h"
#include "strings.h"
#include "helper_utils.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QString>
#include <QTextStream>

SqlForm::SqlForm(Version *v, DatabaseEngine* engine, QWidget *parent) : SourceCodePresenterWidget(v, parent), ui(new Ui::SqlForm), m_engine(engine), m_sourceEntity(0)
{
    ui->setupUi(this);
    highlighter = new SqlHighlighter(ui->txtSql->document(),Workspace::getInstance()->currentProjectsEngine()->getKeywords(),
                                     Workspace::getInstance()->currentProjectsEngine()->getDTSupplier()->numericTypes(),
                                     Workspace::getInstance()->currentProjectsEngine()->getDTSupplier()->booleanTypes(),
                                     Workspace::getInstance()->currentProjectsEngine()->getDTSupplier()->textTypes(),
                                     Workspace::getInstance()->currentProjectsEngine()->getDTSupplier()->blobTypes(),
                                     Workspace::getInstance()->currentProjectsEngine()->getDTSupplier()->dateTimeTypes(),
                                     Workspace::getInstance()->currentProjectsEngine()->getDTSupplier()->miscTypes(),
                                     Workspace::getInstance()->workingVersion()->getTableNames());
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
    InjectSqlDialog* injectDialog = new InjectSqlDialog(Workspace::getInstance()->currentProjectsEngine(), this, 0, "");
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
                QStringList tempSqlList;
                for(int j=0; j<sqlList.size(); j++)
                {
                    QString s = sqlList[j].trimmed();
                    if(s.endsWith(Configuration::instance().sqlOpts()[strSqlDelimiterText].trimmed()))
                    {
                        s = s.left(s.length() - 2);
                        s += ";";
                    }

                    // because of some funny reason "delimiter" is not accepted here, but if we copy/paste, it's ok
                    if(!s.startsWith("delimiter", Qt::CaseInsensitive))
                    {
                        tempSqlList.append(s);
                    }
                }

                if(!m_engine->executeSql(c, tempSqlList, QStringList(), tSql, injectDialog->getRollbackOnError()))
                {
                    QMessageBox::critical (this, tr("Error"), m_engine->getLastError() + tr("<P></PRE>") +
                                           (injectDialog->getRollbackOnError()?tr("Transaction was rolled back."):tr("Transaction was <font color=red><B>NOT</B></font> rolled back, you might have partial data in your database.")), QMessageBox::Ok);
                    error = true;
                }
            }
        }
        MainWindow::instance()->setStatus(QString("SQL injection ") + (error?" failed ":" succeeded "), error);
    }
}

void SqlForm::onSave()
{
    DDM::saveTextFileWithType(this, ui->txtSql->toPlainText(), tr("Save SQL Script"), tr("SQL files (*.sql)"));
}

void SqlForm::presentSql(Project* /*p*/, Version *v)
{
    // here create the final SQL:
    // firstly only the tables and then the foreign keys. We'll see the other elements (triggers, functions) later

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    QStringList uids;
    QStringList finalSql = v->getSqlScript(true, 0, uids);
    QApplication::restoreOverrideCursor();

    QString fs = "";
    for(int i=0; i< finalSql.size(); i++)
    {
        fs += finalSql[i];
    }
    setSource(fs);
    setSqlList(finalSql);
}

void SqlForm::presentSql(Project* p, SqlSource *ent, MainWindow::showSomething)
{
    QString fs = "";
    QHash<QString, QString> opts = Configuration::instance().sqlGenerationOptions();
    opts["FKSposition"] = "OnlyInternal";
    QStringList finalSql = ent->generateSqlSource(p->getEngine()->getSqlGenerator(), opts, 0);
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
        Table* t = dynamic_cast<Table*>(m_sourceEntity);
        if(t) MainWindow::instance()->showTableWithGuid(Workspace::getInstance()->workingVersion(), t->getObjectUid());

        TableInstance* tinst = dynamic_cast<TableInstance*>(m_sourceEntity);
        if(tinst) MainWindow::instance()->showTableInstanceWithGuid(Workspace::getInstance()->workingVersion(), tinst->getObjectUid());

        Procedure* p = dynamic_cast<Procedure*>(m_sourceEntity);
        if(p) MainWindow::instance()->showProcedureWithGuid(Workspace::getInstance()->workingVersion(), p->getObjectUid());

        Function* f = dynamic_cast<Function*>(m_sourceEntity);
        if(f) MainWindow::instance()->showFunctionWithGuid(Workspace::getInstance()->workingVersion(), f->getObjectUid());

        Trigger* tr = dynamic_cast<Trigger*>(m_sourceEntity);
        if(tr) MainWindow::instance()->showTriggerWithGuid(Workspace::getInstance()->workingVersion(), tr->getObjectUid());

        View* v = dynamic_cast<View*>(m_sourceEntity);
        if(v) MainWindow::instance()->showViewWithGuid(Workspace::getInstance()->workingVersion(), v->getObjectUid());
    }
}
