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

#include <QScrollBar>
#include <QVBoxLayout>

#include <QDebug>

NewViewForm::NewViewForm(bool queryBuilder, QueryGraphicsHelper* c, QWidget *parent) :
    SourceCodePresenterWidget(parent),
    ui(new Ui::NewViewForm),
    m_comps(c),
    m_queryBuilder(queryBuilder)
{
    ui->setupUi(this);
    m_highlighter = new SqlHighlighter(ui->txtSql->document());

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

    ui->grpHelp->hide();
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
    QString sql = getSqlSourceEntity()->generateSqlSource(p->getEngine()->getSqlGenerator(), opts, codepage).at(0); // the VIEW generateso only 1 line for now... TODO: Fix
    ui->txtSql->setText(sql);
}

void NewViewForm::presentSql(Project*, SqlSourceEntity*, const QString& codepage)
{
}

void NewViewForm::setView(View *v)
{
    m_view = v;
    ui->txtViewName->setText(v->getName());
    SelectQuery* sq = dynamic_cast<SelectQuery*>(m_comps->getQuery());
    if(sq)
    {
        QVector<const QueryComponent*> sel = sq->getSelectedComponents();
        for(int i=0; i<sel.size(); i++)
        {
            QStringList a; a.append("COL"); a.append(sel.at(i)->get());
            QTreeWidgetItem* w = new QTreeWidgetItem(a);
            ui->lstColumnsForView->addTopLevelItem(w);
        }
    }

}
