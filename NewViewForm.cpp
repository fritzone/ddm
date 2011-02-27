#include "NewViewForm.h"
#include "ui_NewViewForm.h"

#include "QueryGraphicsView.h"
#include "QueryGraphicsScene.h"
#include "QueryGraphicsItem.h"

#include <QVBoxLayout>

NewViewForm::NewViewForm(QueryGraphicsHelper* c, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NewViewForm),
    m_comps(c)
{
    ui->setupUi(this);

    m_qgs = new QueryGraphicsScene(c, this);

    m_qgv = new QueryGraphicsView(this);
    m_qgv->setObjectName(QString::fromUtf8("queryGraphicsView"));
    m_qgv->setDragMode(QGraphicsView::RubberBandDrag);
    m_qgv->setAcceptDrops(true);
    m_qgv->setScene(m_qgs);

    m_qgv->setSceneRect(0,0, 10000, 10000);
    m_qgv->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_qgv->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

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
