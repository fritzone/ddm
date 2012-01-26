#include "DataTypesListForm.h"
#include "UserDataType.h"
#include "ui_DataTypesListForm.h"
#include "MainWindow.h"

DataTypesListForm::DataTypesListForm(QWidget *parent) :
    QWidget(parent), m_ui(new Ui::DataTypesListForm)
{
    m_ui->setupUi(this);

    m_ui->btnUp->hide();
    m_ui->btnDown->hide();
    m_ui->btnRemove->hide();
}

DataTypesListForm::~DataTypesListForm()
{
    delete m_ui;
}

void DataTypesListForm::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void DataTypesListForm::feedInDataTypes(const QVector<UserDataType*>& dataTypes)
{
    for(int i=0; i<dataTypes.size(); i++)
    {
        QStringList a(dataTypes[i]->getName());
        a.append(dataTypes[i]->sqlAsString());
        a.append(dataTypes[i]->typeAsString());
        a.append(dataTypes[i]->sizeAsString());

        QTreeWidgetItem* item = new QTreeWidgetItem((QTreeWidget*)0, a);
        item->setIcon(0, dataTypes[i]->getIcon());
        m_ui->treeDataTypes->addTopLevelItem(item);
    }

    m_ui->treeDataTypes->resizeColumnToContents(0);
    m_ui->treeDataTypes->resizeColumnToContents(1);
    m_ui->treeDataTypes->resizeColumnToContents(2);
    m_ui->treeDataTypes->resizeColumnToContents(3);
}

void DataTypesListForm::doubleClickTree(QTreeWidgetItem* item,int)
{
    QString dtName = item->text(0);
    MainWindow::instance()->showDataType(dtName, true);
}
