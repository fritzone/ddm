#include "DiagramsListForm.h"
#include "ui_DiagramsListForm.h"
#include "Diagram.h"
#include "IconFactory.h"
#include "MainWindow.h"

DiagramsListForm::DiagramsListForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DiagramsListForm)
{
    ui->setupUi(this);
    ui->btnUp->hide();
    ui->btnDown->hide();
    ui->btnRemove->hide();
}

DiagramsListForm::~DiagramsListForm()
{
    delete ui;
}

void DiagramsListForm::changeEvent(QEvent *e)
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

void DiagramsListForm::populateDiagrams(const QVector<Diagram *> dgrs)
{
    ui->treeDiagrams->clear();
    for(int i=0; i<dgrs.size(); i++)
    {
        QStringList a(dgrs.at(i)->getName());

        QTreeWidgetItem* item = new QTreeWidgetItem((QTreeWidget*)0, a);
        item->setIcon(0, IconFactory::getDiagramIcon());
        ui->treeDiagrams->addTopLevelItem(item);

    }
}

void DiagramsListForm::doubleClickTree(QTreeWidgetItem* item, int)
{
    QString name = item->text(0);
    MainWindow::instance()->showDiagram(name);
}
