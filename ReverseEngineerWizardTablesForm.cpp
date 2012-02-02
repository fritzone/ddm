#include "ReverseEngineerWizardObjectListForm.h"
#include "ui_ReverseEngineerWizardTablesForm.h"
#include "IconFactory.h"

ReverseEngineerWizardObjectListForm::ReverseEngineerWizardObjectListForm(QWidget *parent, Mode t) :
    QWizardPage(parent),
    ui(new Ui::ReverseEngineerWizardTablesForm),  m_mode(t)
{
    ui->setupUi(this);

    switch(m_mode)
    {
    case REVERSE_ENGINEER_TABLES:
        setTitle(tr("Select Tables"));
        setSubTitle(tr("Please select the tables you want to reverse engineer."));
        break;
    case REVERSE_ENGINEER_VIEWS:
        setTitle(tr("Select Views"));
        setSubTitle(tr("Please select the views you want to reverse engineer."));
        break;
    case REVERSE_ENGINEER_PROCS:
        setTitle(tr("Select Procedures"));
        setSubTitle(tr("Please select the procedures you want to reverse engineer."));
        break;
    }
    QIcon*p = new QIcon(strFroggieIcon);
    QIcon*p1 = new QIcon(strIcon);
    setPixmap(QWizard::WatermarkPixmap, p->pixmap(150, 187));
    setPixmap(QWizard::LogoPixmap, p1->pixmap(32, 32));
}

ReverseEngineerWizardObjectListForm::~ReverseEngineerWizardObjectListForm()
{
    delete ui;
}

void ReverseEngineerWizardObjectListForm::addTable(const QString & tab)
{
    QListWidgetItem* lwi = new QListWidgetItem(tab, ui->listWidget);
    QIcon c = IconFactory::getTablesIcon();
    if(m_mode == REVERSE_ENGINEER_VIEWS) c = IconFactory::getViewsIcon();
    if(m_mode == REVERSE_ENGINEER_PROCS) c = IconFactory::getProcedureIcon();

    QIcon b = QIcon(c.pixmap(16,16));
    lwi->setIcon(b);
    lwi->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    lwi->setCheckState(Qt::Unchecked);
}

void ReverseEngineerWizardObjectListForm::checkStateChanged(int state)
{
    if(state == Qt::Checked || state == Qt::Unchecked )
    {
        for(int i=0; i<ui->listWidget->count(); i++)
        {
            ui->listWidget->item(i)->setCheckState((Qt::CheckState)state);
        }
    }
}

QStringList ReverseEngineerWizardObjectListForm::getSelectedItems()
{
    QStringList result;
    for(int i=0; i<ui->listWidget->count(); i++)
    {
        if(ui->listWidget->item(i)->checkState() == Qt::Checked)
        {
            result.append(ui->listWidget->item(i)->text());
        }
    }
    return result;
}

void ReverseEngineerWizardObjectListForm::clearList()
{
    ui->listWidget->clear();
}
