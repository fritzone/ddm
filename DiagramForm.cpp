#include "DiagramForm.h"
#include "ui_DiagramForm.h"
#include "Version.h"
#include "IconFactory.h"
#include "Table.h"

#include <QListWidgetItem>
#include <QGraphicsView>
#include <QGraphicsSceneDragDropEvent>
#include <QMessageBox>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QGraphicsScene>
#include <QMimeData>
#include <QtGlobal>

#include <qdebug.h>

class ERGraphicsScene : public QGraphicsScene
{
public:
    ERGraphicsScene(QWidget* parent, Version* v) : QGraphicsScene(parent), itm(0), m_version(v), justDropped(false)
    {
        // to mark the centre of the view
        addLine(0, -10, 0, 10);
        addLine(-10, 0, 10, 0);
    }


    void finalizeItem(int x, int y)
    {
        itm->setX(x);
        itm->setY(y);

        justDropped = false;
    }

    bool justDropped;

protected:
    virtual void dropEvent ( QGraphicsSceneDragDropEvent * event )
    {
        qDebug() << "GraphicsScene::dropEvent : X=" << event->pos().x() << " Y=" << event->pos().y();
        QString tabName = event->mimeData()->text();
        event->acceptProposedAction();

        itm = m_version->getTable(tabName)->prepareDiagramEntity(event->pos().x(), event->pos().y());
        justDropped = true;
        addItem(itm);

    }

    void dragEnterEvent ( QGraphicsSceneDragDropEvent * event )
    {
        qDebug() << "GraphicsScene::dragEnter : X=" << event->pos().x() << " Y=" << event->pos().y();
        event->acceptProposedAction();
    }

    void dragMoveEvent ( QGraphicsSceneDragDropEvent * event )
    {
        qDebug() << "GraphicsScene::dragMove : X=" << event->pos().x() << " Y=" << event->pos().y();
        event->acceptProposedAction();
    }

    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event)
    {
        qDebug() << "ERGraphicsScene::mouseMove : X=" << event->pos().x() << " Y=" << event->pos().y();
    }

    virtual void mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
    {
        qDebug() << "GraphicsScene::mouseRelease: X=" << event->pos().x() << " Y=" << event->pos().y();
    }

private:
    QGraphicsItemGroup* itm;
    Version* m_version;


};

class ERGraphicsView : public QGraphicsView
{
public:

    ERGraphicsView(QWidget* parent, Version* v) : QGraphicsView(parent)
    {
        setAcceptDrops(true);
        scene = new ERGraphicsScene(this, v);
        setScene(scene);
    }

protected:

    virtual void mouseMoveEvent(QMouseEvent *event)
    {
        qDebug() << "GraphicsView::mouseMove : X=" << event->pos().x() << " Y=" << event->pos().y();
        if(scene->justDropped)
        {
            QPointF scpos = mapToScene(event->pos().x(), event->pos().y());
            scene->finalizeItem(scpos.x(), scpos.y());
        }
    }

    void dragEnterEvent ( QGraphicsSceneDragDropEvent * event )
    {
        qDebug() << "GraphicsView::dragEnter : X=" << event->pos().x() << " Y=" << event->pos().y();
        event->acceptProposedAction();
    }
private:

    ERGraphicsScene* scene;

};

class TableListWidget : public QListWidget
{
public:

    TableListWidget(QWidget* parent, ERGraphicsView* associatedGrView) : QListWidget(parent), grv(associatedGrView)
    {
        setDragEnabled(true);
    }

protected:
    virtual void dragEnterEvent(QDragEnterEvent *e) 
    { 
        e->accept(); 
    }

    virtual void dragLeaveEvent(QDragLeaveEvent *event)
    {
        qDebug() << "TabListWidget::dragLeave " ;
        event->accept();
    }


    virtual void dragMoveEvent(QDragMoveEvent *event)
    {
        qDebug() << "TabListWidget::dragMove : X=" << event->pos().x() << " Y=" << event->pos().y();
        event->accept();
    }

    virtual void dropEvent(QDropEvent *event)
    {
        event->accept();
    }

    virtual void mouseMoveEvent(QMouseEvent *event)
    {
        if (!(event->buttons() & Qt::LeftButton)) return;
        if (currentItem() == NULL) return;

        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;

        // mime stuff
        mimeData->setText(currentItem()->text());
        drag->setMimeData(mimeData);
        drag->setPixmap(IconFactory::getTablesIcon().pixmap(16,16));

        // start drag
        drag->start(Qt::CopyAction | Qt::MoveAction);
    }
private:
    ERGraphicsView* grv;
};

DiagramForm::DiagramForm(Version* v, QWidget *parent) : QWidget(parent), ui(new Ui::DiagramForm), ver(v)
{

    ui->setupUi(this);

    ERGraphicsView *graphicsView;
    graphicsView = new ERGraphicsView(this, v);
    graphicsView->setObjectName(QString::fromUtf8("graphicsView"));
    graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
    graphicsView->setAcceptDrops(true);

    TableListWidget *lstTables;
    lstTables = new TableListWidget(ui->groupBox, graphicsView);
    lstTables->setObjectName(QString::fromUtf8("lstTables"));
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(lstTables->sizePolicy().hasHeightForWidth());
    lstTables->setSizePolicy(sizePolicy);
    lstTables->setDragDropMode(QAbstractItemView::DragDrop);

    ui->verticalLayout_2->addWidget(lstTables);

    ui->horizontalLayout->addWidget(graphicsView);

    for(int i=0; i<v->getTables().size(); i++)
    {
        QListWidgetItem* qlwi = new QListWidgetItem(v->getTables()[i]->getName(), lstTables);
        qlwi->setIcon(IconFactory::getTablesIcon());
    }
}

DiagramForm::~DiagramForm()
{
    delete ui;
}

void DiagramForm::changeEvent(QEvent *e)
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
