#include "SqlForm.h"
#include "ui_SqlForm.h"
#include "SqlHighlighter.h"
#include "InjectSqlDialog.h"
#include "DatabaseEngine.h"
#include "Version.h"
#include "Configuration.h"
#include "TableInstance.h"
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
        if(!m_engine->injectSql(injectDialog->getHost(), injectDialog->getUser(), injectDialog->getPassword(), injectDialog->getDatabase(), sqlList, tSql))
        {
            QMessageBox::critical (this, tr("Error"), tr("Cannot execute a query: ") + m_engine->getLastError() + tr(". Query:") + tSql, QMessageBox::Ok);
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
    QStringList finalSql("-- Full SQL listing for project " + p->getName() + "\n");
    if(v->oop())   // list the table instances' SQL
    {
        QHash<QString, QString> opts = Configuration::instance().sqlGenerationOptions();
        bool upcase = opts.contains("Case") && opts["Case"] == "Upper";
        opts["FKSposition"] = "OnlyInternal";
        for(int i=0; i<v->getTableInstances().size(); i++)
        {
            QStringList sql = v->getTableInstances().at(i)->generateSqlSource(p->getEngine()->getSqlGenerator(), opts);
            finalSql << sql;
        }

        for(int i=0; i<v->getTableInstances().size(); i++)
        {
            for(int j=0; j<v->getTableInstances().at(i)->table()->getForeignKeyCommands().size(); j++)
            {
                QString f = upcase?"ALTER TABLE ":"alter table ";
                f += v->getTableInstances().at(i)->getName();
                f += upcase?" ADD ":" add ";
                f += v->getTableInstances().at(i)->table()->getForeignKeyCommands().at(j);
                f += ";\n";

                finalSql << f;
            }
        }
    }
    else    // list table's SQL
    {
        QHash<QString, QString> opts = Configuration::instance().sqlGenerationOptions();
        bool upcase = opts.contains("Case") && opts["Case"] == "Upper";
        opts["FKSposition"] = "OnlyInternal";
        for(int i=0; i<v->getTables().size(); i++)
        {
            QStringList sql = v->getTables().at(i)->generateSqlSource(p->getEngine()->getSqlGenerator(), opts);
            finalSql << sql;
        }

        for(int i=0; i<v->getTables().size(); i++)
        {
            for(int j=0; j<v->getTables().at(i)->getForeignKeyCommands().size(); j++)
            {
                QString f = upcase?"ALTER TABLE ":"alter table ";
                f += v->getTables().at(i)->getName();
                f += upcase?" ADD ":" add ";
                f += v->getTables().at(i)->getForeignKeyCommands().at(j);
                f += ";\n";

                finalSql << f;
            }
        }
    }

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

