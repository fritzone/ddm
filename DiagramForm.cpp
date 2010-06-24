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
#include <QSvgGenerator>
#include <qdebug.h>

DiagramForm::DiagramForm(Version* v, QWidget *parent) : QWidget(parent), ui(new Ui::DiagramForm), ver(v)
{

    ui->setupUi(this);

    lstTables = new TableListWidget(ui->groupBox, graphicsView);
    lstTables->setObjectName(QString::fromUtf8("lstTables"));
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(lstTables->sizePolicy().hasHeightForWidth());
    lstTables->setSizePolicy(sizePolicy);
    lstTables->setDragDropMode(QAbstractItemView::DragDrop);

    graphicsView = new ERGraphicsView(this, v, lstTables);
    graphicsView->setObjectName(QString::fromUtf8("graphicsView"));
    graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
    graphicsView->setAcceptDrops(true);


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

bool DiagramForm::saveToFile(const QString& fileName, bool transparent, const char* mode)
{
    // fidn the area which has diagram elemts in it
    QRectF cvr = graphicsView->scene()->getCoverageRect();
    QPoint tl = graphicsView->mapFromScene(cvr.topLeft());
    QPoint br = graphicsView->mapFromScene(cvr.bottomRight());
    QRect fr(tl, br);
    
    QGraphicsRectItem* r1 = 0;
    if(!transparent)
    {
        QBrush b1 (Qt::white);
        r1= new QGraphicsRectItem (ERGraphicsScene::LEFT, ERGraphicsScene::TOP, ERGraphicsScene::WIDTH, ERGraphicsScene::HEIGHT);
        r1->setBrush(b1);
        r1->setZValue(-100);
        graphicsView->scene()->addItem(r1);
    }



    QSize sz =fr.size();
    QImage newImage(sz, QImage::Format_ARGB32);
    QRect t1;

    QPainter painter(&newImage);
    if(transparent)
    {
        //QBrush trBrush(Qt::transparent);
        painter.fillRect(fr, Qt::transparent);
        newImage.fill(Qt::transparent);;
    }

    if(QString(mode) == "svg")
    {
        QSvgGenerator generator;
        generator.setSize(QSize(fr.width(), fr.height()));
        generator.setFileName(fileName);
        QPainter p2;
        p2.begin(&generator);
        graphicsView->render(&p2, t1, fr);
        p2.end();
    }
    else
    {
        graphicsView->render(&painter, t1, fr);
        if (!newImage.save(fileName, mode ,80))
        {
            QMessageBox::critical (this, tr("Error"), tr("Cannot save file"), QMessageBox::Ok);
            return false;
        }
    }

    if(r1)
    {
        graphicsView->scene()->removeItem(r1);
        delete r1;
        r1 = 0;
    }

    return true;
}


void DiagramForm::saveToImageFile()
{
    QString name = QFileDialog::getSaveFileName(this, tr("Save diagram"), "", tr("Images (*.png *.bmp *.jpg *.jpeg *.tiff);;SVG(*.svg)"));

    const char* mode = "png";
    if(name.endsWith(".png", Qt::CaseInsensitive)) mode = "png";
    if(name.endsWith(".bmp", Qt::CaseInsensitive)) mode = "bmp";
    if(name.endsWith(".jpg", Qt::CaseInsensitive)) mode = "jpg";
    if(name.endsWith(".tiff", Qt::CaseInsensitive)) mode = "tiff";
    if(name.endsWith(".svg", Qt::CaseInsensitive)) mode = "svg";

    bool transparent = false;

    if(QString(mode) == "png" || QString(mode) == "tiff")
    {
        QMessageBox msgBox;
        msgBox.setText("Do you want to save a transparent image?");
        msgBox.setInformativeText("PNG/TIFF supports transparency.");
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int ret = msgBox.exec();

        if(ret == QMessageBox::Yes)
        {
            transparent = true;
        }
    }

    saveToFile(name, transparent, mode);
}
