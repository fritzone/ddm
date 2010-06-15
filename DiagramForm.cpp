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

class ERGraphicsScene : public QGraphicsScene
{
public:
    ERGraphicsScene(QWidget* parent, Version* v) : QGraphicsScene(parent), m_version(v), lastX(-1), lastY(-1)
    {}

    void setLastCoords(int x, int y)
    {
        if(x != 0) lastX = x;
        if(y != 0) lastY = y;
    }

protected:
    virtual void dropEvent ( QGraphicsSceneDragDropEvent * event )
    {
        QString tabName = event->mimeData()->text();
        event->acceptProposedAction();

        QGraphicsItemGroup* itm = m_version->getTable(tabName)->prepareDiagramEntity(event->pos().x(), event->pos().y());
        itm->setX(lastX);
        itm->setY(lastY);
        addItem(itm);
    }

    void dragEnterEvent ( QGraphicsSceneDragDropEvent * event )
    {
        event->acceptProposedAction();
    }

    void dragMoveEvent ( QGraphicsSceneDragDropEvent * event )
    {
        event->acceptProposedAction();
        int x = event->pos().x();
        int y = event->pos().y();
        setLastCoords(x,y);
    }

    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event)
    {
        setLastCoords(event->pos().x(), event->pos().y());
    }

private:

    Version* m_version;
    int lastX;
    int lastY;

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


    void setLastCoords(int x, int y)
    {
        scene->setLastCoords(x, y);
    }

protected:

    virtual void mouseMoveEvent(QMouseEvent *event)
    {
        scene->setLastCoords(event->pos().x(), event->pos().y());
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

    virtual void dragMoveEvent(QDragMoveEvent *e)
    {
        e->accept();
    }

    virtual void dropEvent(QDropEvent *event)
    {
        event->accept();
    }

    virtual void mouseMoveEvent(QMouseEvent *event)
    {
        // if not left button - return
        if (!(event->buttons() & Qt::LeftButton)) return;

        // if no item selected, return (else it would crash)
        if (currentItem() == NULL) return;

        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;

        // mime stuff
        mimeData->setText(currentItem()->text());
        drag->setMimeData(mimeData);
        drag->setPixmap(IconFactory::getTablesIcon().pixmap(16,16));

        // start drag
        drag->start(Qt::CopyAction | Qt::MoveAction);

        grv->setLastCoords(event->pos().x(), event->pos().y());
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
