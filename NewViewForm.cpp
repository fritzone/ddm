#include "NewViewForm.h"
#include "ui_NewViewForm.h"

#include"SqlHighlighter.h"
#include "qbr_QueryGraphicsView.h"
#include "qbr_QueryGraphicsScene.h"
#include "qbr_QueryGraphicsItem.h"

#include <QScrollBar>
#include <QVBoxLayout>

#include <QDebug>

NewViewForm::NewViewForm(QueryGraphicsHelper* c, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NewViewForm),
    m_comps(c)
{
    ui->setupUi(this);
    m_highlighter = new SqlHighlighter(ui->txtSql->document());

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
    m_qgs->addItem(itm);
}

void NewViewForm::scrollTo(int hor, int ver)
{
    //m_qgv->horizontalScrollBar()->setSliderPosition(hor);
    //m_qgv->verticalScrollBar()->setSliderPosition(ver);
    m_qgv->centerOn(hor, ver);
}


void NewViewForm::getCenter(int &x, int &y)
{
    x = m_qgv->mapToScene(m_qgv->viewport()->rect().center()).x();
    y = m_qgv->mapToScene(m_qgv->viewport()->rect().center()).y();
}

void NewViewForm::setSql(const QString &sql)
{
    m_sql = sql;
    ui->txtSql->setText(m_sql);
}
