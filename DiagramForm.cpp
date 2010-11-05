#include "DiagramForm.h"
#include "ui_DiagramForm.h"
#include "Version.h"
#include "IconFactory.h"
#include "Table.h"
#include "ERGraphicsScene.h"
#include "ERGraphicsView.h"
#include "DraggableGraphicsItem.h"
#include "DraggableGraphicsItemForText.h"
#include "TableListWidget.h"
#include "Diagram.h"
#include "mainwindow.h"

#include <QListWidgetItem>
#include <QGraphicsView>
#include <QGraphicsSceneDragDropEvent>
#include <QMessageBox>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QPrintDialog>
#include <QtGlobal>
#include <QSvgGenerator>
#include <qdebug.h>

DiagramForm::DiagramForm(Version* v, Diagram* dgram, QWidget *parent) : QWidget(parent),
        ui(new Ui::DiagramForm), ver(v), m_diagram(dgram), m_mw(dynamic_cast<MainWindow*>(parent)), m_tabToRemove(""), m_noteToRemove(-1)
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

    lstDiagramForms = new TableListWidget(ui->groupBox, graphicsView);
    lstDiagramForms->setObjectName(QString::fromUtf8("lstDiagramForms"));

    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(lstDiagramForms->sizePolicy().hasHeightForWidth());
    lstDiagramForms->setSizePolicy(sizePolicy);
    lstDiagramForms->setDragDropMode(QAbstractItemView::DragDrop);


    graphicsView = new ERGraphicsView(this, v, dgram, lstTables);
    graphicsView->setObjectName(QString::fromUtf8("graphicsView"));
    graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
    graphicsView->setAcceptDrops(true);

    ui->txtDiagramName->setText(m_diagram->getName());

    ui->contentLayout->addWidget(lstTables);
    ui->contentLayout->addWidget(lstDiagramForms);
    ui->horizontalLayout->addWidget(graphicsView);

    prepareLists();
    ui->cmbDgramNotation->hide();
    ui->grpHelp->hide();

}

void DiagramForm::prepareLists()
{
    lstTables->clear();
    lstDiagramForms->clear();

    for(int i=0; i<ver->getTables().size(); i++)
    {
        QListWidgetItem* qlwi = new QListWidgetItem(ver->getTables()[i]->getName(), lstTables);
        qlwi->setIcon(IconFactory::getTablesIcon());
    }

    QListWidgetItem* qlwNotes = new QListWidgetItem("Add Note", lstDiagramForms);
    qlwNotes->setIcon(QIcon(":/images/actions/images/actions/note.png"));
    lstDiagramForms->setEnabled(false);
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
    // fin the area which has diagram elemts in it
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

void DiagramForm::printDiagram()
{
    QRectF cvr = graphicsView->scene()->getCoverageRect();
    QPoint tl = graphicsView->mapFromScene(cvr.topLeft());
    QPoint br = graphicsView->mapFromScene(cvr.bottomRight());
    QRect fr(tl, br);

    QPrinter printer(QPrinter::HighResolution);

    QPrintDialog printDialog(&printer, this);

    if(printDialog.exec() == QDialog::Accepted)
    {
        printer.setPageSize(QPrinter::A4);
        QPainter painter(&printer);
        QRect t1;
        graphicsView->render(&painter, t1, fr);
    }
}

void DiagramForm::saveToImageFile()
{
    QString name = QFileDialog::getSaveFileName(this, tr("Save diagram"), "", tr("Images (*.png *.bmp *.jpg *.jpeg *.tiff);;SVG(*.svg)"));

    if(name.length() == 0)  // nothing selected
    {
        return;
    }

    const char* mode = "";
    if(name.endsWith(".png", Qt::CaseInsensitive)) mode = "png";
    if(name.endsWith(".bmp", Qt::CaseInsensitive)) mode = "bmp";
    if(name.endsWith(".jpg", Qt::CaseInsensitive)) mode = "jpg";
    if(name.endsWith(".tiff", Qt::CaseInsensitive)) mode = "tiff";
    if(name.endsWith(".svg", Qt::CaseInsensitive)) mode = "svg";

    if(QString(mode).length() == 0)
    {
        QMessageBox::critical(this, "Error", "Please specify one of the extensions: png bmp jpg tiff svg", QMessageBox::Ok);
        return;
    }

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

void DiagramForm::onButtonBoxClicked(QAbstractButton* btn)
{
    if(btn == ui->buttonBox->buttons().at(0)) // Seems very strange, but works like this... Save is the First, after Reset
    {
        onSave();
    }
    else
    {
        onReset();
    }
}

void DiagramForm::onSave()
{
    m_diagram->setName(ui->txtDiagramName->text());
    if(!m_diagram->isSaved())
    {
        ver->addDiagram(m_diagram);
    }
    m_mw->onSaveDiagram(m_diagram);
}

void DiagramForm::onReset()
{
    m_diagram->reset(true);
    graphicsView->scene()->clear();
    prepareLists();
}

void DiagramForm::removeFromDiagram()
{
    graphicsView->scene()->removeTable(m_tabToRemove);
    QListWidgetItem* qlwi = new QListWidgetItem(m_tabToRemove, lstTables);
    qlwi->setIcon(IconFactory::getTablesIcon());

    m_tabToRemove = "";
}

void DiagramForm::removeNoteFromDiagram()
{
    graphicsView->scene()->removeNote(m_noteToRemove);
    m_noteToRemove = -1;
}


void DiagramForm::setTableToRemoveFromDiagram(const QString& tabName)
{
    m_tabToRemove = tabName;
}

void DiagramForm::setCurrentWorkNoteOnDiagram(int noteIndex)
{
    m_noteToRemove = noteIndex;
}


void DiagramForm::onAddNote()
{
}

void DiagramForm::doneNote()
{
    lstDiagramForms->setEnabled(true);
}


void DiagramForm::editorLostFocus(DiagramTextItem *)
{
}

void DiagramForm::onEditNote()
{
    DraggableGraphicsViewItemForText* note = m_diagram->getNote(m_noteToRemove);
    if(note)
    {
        note->editNote();
        m_diagram->updateDescriptors();
    }
}

void DiagramForm::paintDiagram()
{
    bool enableNotes = false;
    m_diagram->reset();
    graphicsView->scene()->clear();
    // first step, put on all the notes
    for(int i=0; i<m_diagram->getNoteDescriptors().size(); i++)
    {
        DiagramNoteDescriptor* noteItem = m_diagram->getNoteDescriptors()[i];
        qDebug() << noteItem->isFramed();
        DraggableGraphicsViewItemForText* toAdd = m_diagram->clone(noteItem);
        graphicsView->scene()->addItem(toAdd);
        toAdd->place();
        m_diagram->addNoteItem(toAdd);
    }
    // then the tables
    for(int i=0; i<m_diagram->getTableDescriptors().size(); i++)
    {
        DiagramObjectDescriptor* tabDescriptor = m_diagram->getTableDescriptors()[i];
        DraggableGraphicsViewItem* toAdd = m_diagram->clone(tabDescriptor);
        graphicsView->scene()->addItem(toAdd);
        lstTables->removeItem(toAdd->getTable()->getName());
        toAdd->place();
        m_diagram->addTableItem(toAdd);
        enableNotes = true;
    }

    // and finally the foreign keys
    m_diagram->recreateFks(graphicsView->scene());

    lstDiagramForms->setEnabled(enableNotes);

    graphicsView->centerOn(graphicsView->scene()->getCoverageRect().center());
}

void DiagramForm::onHelp()
{
    ui->grpHelp->show();
    ui->btnHelp->setHidden(true);
    ui->webView->setUrl(QApplication::applicationDirPath() + QString("/doc/dgram.html"));
}

void DiagramForm::onNameChange(QString a)
{
    if(m_diagram && m_diagram->getLocation())
    {
        m_diagram->setName(a);
        m_diagram->getLocation()->setText(0, a);
        QVariant var(a);
        m_diagram->getLocation()->setData(0, Qt::UserRole, var);
    }
}

void DiagramForm::onZoomOut()
{
    graphicsView->scale(1/1.2, 1/1.2);
}

void DiagramForm::onZoomIn()
{
    graphicsView->scale(1.2, 1.2);
}
