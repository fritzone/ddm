#include "DiagramForm.h"
#include "ui_DiagramForm.h"
#include "Version.h"
#include "IconFactory.h"
#include "Table.h"
#include "ERGraphicsScene.h"
#include "ERGraphicsView.h"
#include "DraggableGraphicsItem.h"
#include "TableListWidget.h"

#include <QListWidgetItem>
#include <QGraphicsView>
#include <QGraphicsSceneDragDropEvent>
#include <QMessageBox>
#include <QDragEnterEvent>
#include <QDropEvent>

#include <QtGlobal>

#include <qdebug.h>

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
