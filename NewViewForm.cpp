#include "NewViewForm.h"
#include "ui_NewViewForm.h"
#include "db_DatabaseEngine.h"
#include "SqlHighlighter.h"
#include "qbr_QueryGraphicsView.h"
#include "qbr_QueryGraphicsScene.h"
#include "qbr_QueryGraphicsItem.h"
#include "SqlSourceEntity.h"
#include "Project.h"
#include "Configuration.h"
#include "core_View.h"
#include "qbr_SelectQuery.h"
#include "qbr_SingleExpressionQueryComponent.h"
#include "qbr_SelectQueryAsComponent.h"
#include "Column.h"
#include "Workspace.h"
#include "Version.h"
#include "VersionGuiElements.h"
#include "gui_HelpWindow.h"
#include "MainWindow.h"
#include "InjectSqlDialog.h"
#include "core_ConnectionManager.h"

#include <QScrollBar>
#include <QVBoxLayout>
#include <QtGui>

#include <QDebug>

NewViewForm::NewViewForm(bool queryBuilder, QueryGraphicsHelper* c, QWidget *parent) :
    SourceCodePresenterWidget(parent),
    ui(new Ui::NewViewForm),
    m_comps(c),
    m_queryBuilder(queryBuilder)
{
    ui->setupUi(this);
    txtSql = new QTextEditWithCodeCompletion(ui->groupBox_3);
    txtSql->setObjectName(QString::fromUtf8("txtSql"));
    txtSql->setMinimumSize(QSize(0, 200));

    ui->verticalLayout_4->addWidget(txtSql);
    QObject::connect(txtSql, SIGNAL(textChanged()), this, SLOT(onSqlChange()));

    if(m_queryBuilder)
    {
        m_qgs = new QueryGraphicsScene(c, this);

        m_qgv = new QueryGraphicsView(this);
        m_qgv->setObjectName(QString::fromUtf8("queryGraphicsView"));
        m_qgv->setDragMode(QGraphicsView::RubberBandDrag);
        m_qgv->setAcceptDrops(true);
        m_qgv->setScene(m_qgs);

        m_qgv->setSceneRect(0,0, 10000, 10000);
        m_qgv->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        m_qgv->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

        ui->verticalLayout->insertWidget(1, m_qgv);
    }
    else
    {
        ui->tabWidget_4->hide();

        delete ui->txtViewName;
        delete ui->labelName;

        ui->txtViewName = new QLineEdit(ui->groupBox_3);
        ui->txtViewName->setObjectName(QString::fromUtf8("txtViewName"));
        ui->txtViewName->setToolTip(QApplication::translate("NewViewForm", "The name of the View", 0, QApplication::UnicodeUTF8));
        ui->groupBox->hide();
        ui->verticalLayout_4->insertWidget(0, ui->txtViewName);
    }
}

NewViewForm::~NewViewForm()
{
    delete ui;
}

void NewViewForm::changeEvent(QEvent *e)
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

void NewViewForm::setGraphicsItem(QueryGraphicsItem * itm)
{
    if(m_queryBuilder) m_qgs->addItem(itm);
}

void NewViewForm::scrollTo(int hor, int ver)
{
    if(m_queryBuilder) m_qgv->centerOn(hor, ver);
}


void NewViewForm::getCenter(int &x, int &y)
{
    if(m_queryBuilder)
    {
        x = m_qgv->mapToScene(m_qgv->viewport()->rect().center()).x();
        y = m_qgv->mapToScene(m_qgv->viewport()->rect().center()).y();
    }
}

void NewViewForm::presentSql(Project* p, const QString& codepage)
{
    // TODO: this is still not right, it's here just to work.
    QHash<QString, QString> opts = Configuration::instance().sqlGenerationOptions();
    QStringList sqls = getSqlSourceEntity()->generateSqlSource(p->getEngine()->getSqlGenerator(), opts, codepage);
    QString sql = "";
    for(int i=0; i<sqls.size(); i++)
    {
        sql += sqls.at(i);
    }
    txtSql->setText(sql);
}

void NewViewForm::presentSql(Project*, SqlSourceEntity*, const QString& codepage)
{
}

void NewViewForm::setView(View *v)
{
    m_view = v;
    ui->chkCanReplace->setChecked(v->getReplace());
    ui->txtViewName->setText(v->getName());
    if(m_comps) // do this only if w are buildingteh query with the query builder
    {
        SelectQuery* sq = dynamic_cast<SelectQuery*>(m_comps->getQuery());
        QStringList columnNamesForView;
        if(sq)
        {
            QVector<const QueryComponent*> sel = sq->getSelectedComponents();
            for(int i=0; i<sel.size(); i++)
            {
                bool added = false;
                QVector<const Column*> columns;
                SingleExpressionQueryComponent* seq = dynamic_cast<SingleExpressionQueryComponent*>(const_cast<QueryComponent*>(sel.at(i)));
                if(seq->hasStar())
                {
                    columnNamesForView.clear();
                    break;
                }
                if(seq)
                {
                    if(seq->hasAtLeastOneColumnSelected())
                    {
                        columns = seq->getColumns();
                    }

                    for(int j=0; j<columns.size(); j++)
                    {
                        QStringList a; a.append(columns.at(j)->getName()); a.append(columns.at(j)->getName());
                        columnNamesForView.append(columns.at(j)->getName());
                        QTreeWidgetItem* w = new QTreeWidgetItem(a);
                        ui->lstColumnsForView->addTopLevelItem(w);
                        added = true;
                    }

                    if(const SelectQueryAsComponent* as = seq->hasAs())             // and the last one is added as the alias
                    {
                        QStringList a; a.append(as->getAs()); a.append(sel.at(i)->get());
                        columnNamesForView.append(as->getAs());
                        QTreeWidgetItem* w = new QTreeWidgetItem(a);
                        ui->lstColumnsForView->addTopLevelItem(w);
                        added = true;
                    }
                    // check if this is a plain * and in that case reset the list, we don't want that
                }

                if(!added)
                {
                    QString n;n.setNum(i);
                    QStringList a; a.append(QString("COL")+n); a.append(sel.at(i)->get());
                    columnNamesForView.append(QString("COL")+n);
                    QTreeWidgetItem* w = new QTreeWidgetItem(a);
                    ui->lstColumnsForView->addTopLevelItem(w);
                }
            }
            v->setColumnNames(columnNamesForView);
        }
    }
}

void NewViewForm::onChkCanReplaceToggle(bool st)
{
    m_view->setReplace(st);
    presentSql(Workspace::getInstance()->currentProject(), QString("latin1"));
}

void NewViewForm::onNameChange(QString a)
{
    m_view->setName(a);
    m_view->getLocation()->setText(0, a);
    presentSql(Workspace::getInstance()->currentProject(), QString("latin1"));
}

void NewViewForm::onSqlChange()
{
    if(!m_queryBuilder)
    {
        m_view->setSql(txtSql->toPlainText());
    }
}

void NewViewForm::onHelp()
{
    HelpWindow* hw = HelpWindow::instance();
    hw->showHelp(QString("/doc/view.html"));
    hw->show();
    Workspace::getInstance()->workingVersion()->getGui()->getMainWindow()->addDockWidget(Qt::RightDockWidgetArea, hw);
}

void NewViewForm::onSaveSql()
{
    QString name = QFileDialog::getSaveFileName(this, tr("Save sql"), "", tr("SQL files (*.sql)"));
    QFile file(name);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return;
    }
    QTextStream out(&file);
    out << txtSql->toPlainText() << "\n";
}

void NewViewForm::onInject()
{
    InjectSqlDialog* injectDialog = new InjectSqlDialog(Workspace::getInstance()->currentProjectsEngine(), this);
    //injectDialog->selectCodePage(ui->cmbCharSetForSql->currentIndex());
    injectDialog->setModal(true);
    bool error = false;
    if(injectDialog->exec() == QDialog::Accepted)
    {
        QString tSql;
        QStringList connectionNames = injectDialog->getSelectedConnections();
        for(int i=0; i< connectionNames.size(); i++)
        {
            Connection* c = ConnectionManager::instance()->getConnection(connectionNames.at(i));
            if(c)
            {
                QString sql = txtSql->toPlainText();
                QStringList sqls; sqls << sql;
                if(!Workspace::getInstance()->currentProjectsEngine()->executeSql(c, sqls, tSql, injectDialog->getRollbackOnError()))
                {
                    QMessageBox::critical (this, tr("Error"), tr("<B>Cannot execute a query!</B><P>Reason: ") + Workspace::getInstance()->currentProjectsEngine()->getLastError() + tr(".<P>Query:<PRE>") + tSql+ "</PRE><P>" +
                                           (injectDialog->getRollbackOnError()?tr("Transaction was rolled back."):tr("Transaction was <font color=red><B>NOT</B></font> rolled back, you might have partial data in your database.")), QMessageBox::Ok);
                    error = true;
                }
            }
        }
        MainWindow::instance()->setStatus(QString("Creating view ") + ui->txtViewName->text() + (error?" Failed ":" Succeeded "), error);
    }
}
