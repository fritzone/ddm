#include "SqlForm.h"
#include "ui_SqlForm.h"
#include "SqlHighlighter.h"
#include "InjectSqlDialog.h"
#include "DatabaseEngine.h"
#include "Version.h"
#include "Configuration.h"
#include "TableInstance.h"
#include "Workspace.h"
#include "Project.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QString>
#include <QTextStream>

SqlForm::SqlForm(DatabaseEngine* engine, QWidget *parent) : SourceCodePresenterWidget(parent), ui(new Ui::SqlForm), m_engine(engine)
{
    ui->setupUi(this);
    highlighter = new SqlHighlighter(ui->txtSql->document());
    ui->cmbOptions->hide();
    ui->grpHelp->hide();
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
    InjectSqlDialog* injectDialog = new InjectSqlDialog(this);
    injectDialog->setModal(true);
    if(injectDialog->exec() == QDialog::Accepted)
    {
        QString tSql;
        if(!m_engine->injectSql(injectDialog->getHost(), injectDialog->getUser(), injectDialog->getPassword(), injectDialog->getDatabase(), sqlList, tSql, injectDialog->getRollbackOnError(), injectDialog->getCreateOnlyIfNotExist()))
        {
            QMessageBox::critical (this, tr("Error"), tr("<B>Cannot execute a query!</B><P>Reason: ") + m_engine->getLastError() + tr(".<P>Query:<PRE>") + tSql+ "</PRE><P>" +
                                   (injectDialog->getRollbackOnError()?tr("Transaction was rolled back."):tr("Transaction was <font color=red><B>NOT</B></font> rolled back, you might have partial data in your database.")), QMessageBox::Ok);
        }
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

void SqlForm::presentSql(Project* p)
{
    // here create the final SQL:
    // firstly only the tables and then the foreign keys. We'll see the other elements (triggers, functions) later

    Version *v = p->getWorkingVersion();
    QStringList finalSql = v->getSqlScript();

    QString fs = "";
    for(int i=0; i< finalSql.size(); i++)
    {
        fs += finalSql[i];
    }
    setSource(fs);
    setSqlList(finalSql);
}

void SqlForm::presentSql(Project* p, SqlSourceEntity* ent)
{
    QString fs = "";
    QStringList finalSql = ent->generateSqlSource(p->getEngine()->getSqlGenerator(), Configuration::instance().sqlGenerationOptions());
    for(int i=0; i< finalSql.size(); i++)
    {
        fs += finalSql[i];
    }
    setSource(fs);
    setSqlList(finalSql);
}

void SqlForm::onHelp()
{
    ui->grpHelp->show();
    ui->btnHelp->hide();
    ui->webView->setUrl(QApplication::applicationDirPath() + QString("/doc/sqls.html"));
}
