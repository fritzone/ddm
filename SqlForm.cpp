#include "SqlForm.h"
#include "ui_SqlForm.h"
#include "SqlHighlighter.h"
#include "InjectSqlDialog.h"
#include "DatabaseEngine.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QString>
#include <QTextStream>

SqlForm::SqlForm(DatabaseEngine* engine, QWidget *parent) : QWidget(parent), ui(new Ui::SqlForm), m_engine(engine)
{
    ui->setupUi(this);
    highlighter = new SqlHighlighter(ui->txtSql->document());
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
        if(!m_engine->injectSql(injectDialog->getHost(), injectDialog->getUser(), injectDialog->getPassword(), injectDialog->getDatabase(), sqlList))
        {
            QMessageBox::critical (this, tr("Error"), tr("Cannot execute a query: ") + m_engine->getLastError(), QMessageBox::Ok);
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
