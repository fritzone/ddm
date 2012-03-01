#include "NamedObjectListingForm.h"
#include "NamedItem.h"

#include <QDebug>

NamedObjectListingForm::NamedObjectListingForm(QWidget *parent, MainWindow::showSomething shower, const QIcon& icon, const QString& title) :
    QWidget(parent),
    ui(new Ui::NamedObjectListingForm), m_objectShower(shower), m_icon(icon), m_title(title)
{
    ui->setupUi(this);
    ui->btnUp->hide();
    ui->btnDown->hide();
    ui->btnRemove->hide();
    QObject::connect(ui->treeItems, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(doubleClickTree(QTreeWidgetItem*,int)));

    // hm... :)
    ui->lblHeader->setText("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\np, li { white-space: pre-wrap; }\n</style></head><body style=\" font-family:'Sans'; font-size:10pt; font-weight:400; font-style:normal;\">\n<table border=\"0\" style=\"-qt-table-type: root; margin-top:4px; margin-bottom:4px; margin-left:4px; margin-right:4px;\">\n<tr>\n<td style=\"border: none;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; font-weight:600;\">"+title+"</span></p></td></tr></table></body></html>");

}

NamedObjectListingForm::~NamedObjectListingForm()
{
    delete ui;
}

void NamedObjectListingForm::changeEvent(QEvent *e)
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

void NamedObjectListingForm::doubleClickTree(QTreeWidgetItem* item, int)
{
    QString guid = item->data(0, Qt::UserRole).toString();
    qDebug() << guid;
    (MainWindow::instance()->*m_objectShower)(guid, true);
}
