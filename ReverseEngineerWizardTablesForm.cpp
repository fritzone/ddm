#include "ReverseEngineerWizardTablesForm.h"
#include "ui_ReverseEngineerWizardTablesForm.h"
#include "IconFactory.h"

ReverseEngineerWizardTablesForm::ReverseEngineerWizardTablesForm(QWidget *parent, Mode t) :
    QWizardPage(parent), m_mode(t),
    ui(new Ui::ReverseEngineerWizardTablesForm)
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
    }
    QIcon*p = new QIcon(strFroggieIcon);
    QIcon*p1 = new QIcon(strIcon);
    setPixmap(QWizard::WatermarkPixmap, p->pixmap(150, 187));
    setPixmap(QWizard::LogoPixmap, p1->pixmap(32, 32));
}

ReverseEngineerWizardTablesForm::~ReverseEngineerWizardTablesForm()
{
    delete ui;
}

void ReverseEngineerWizardTablesForm::addTable(const QString & tab)
{
    QListWidgetItem* lwi = new QListWidgetItem(tab, ui->listWidget);
    QIcon c = IconFactory::getTablesIcon();
    if(m_mode == REVERSE_ENGINEER_VIEWS) c = IconFactory::getViewsIcon();
    QIcon b = QIcon(c.pixmap(16,16));
    lwi->setIcon(b);
    lwi->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    lwi->setCheckState(Qt::Unchecked);
}

void ReverseEngineerWizardTablesForm::checkStateChanged(int state)
{
    if(state == Qt::Checked || state == Qt::Unchecked )
    {
        for(int i=0; i<ui->listWidget->count(); i++)
        {
            ui->listWidget->item(i)->setCheckState((Qt::CheckState)state);
        }
    }
}

QVector<QString> ReverseEngineerWizardTablesForm::getSelectedItems()
{
    QVector<QString> result;
    for(int i=0; i<ui->listWidget->count(); i++)
    {
        if(ui->listWidget->item(i)->checkState() == Qt::Checked)
        {
            result.append(ui->listWidget->item(i)->text());
        }
    }
    return result;
}

void ReverseEngineerWizardTablesForm::clearList()
{
    ui->listWidget->clear();
}
