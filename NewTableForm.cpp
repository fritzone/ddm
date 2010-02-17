#include "NewTableForm.h"
#include "ui_NewTableForm.h"
#include "IconFactory.h"
#include "DatabaseEngine.h"
#include "Project.h"
#include "Version.h"
#include "UserDataType.h"
#include "Column.h"
#include "Table.h"

NewTableForm::NewTableForm(DatabaseEngine* db, Project* prj, QWidget *parent) : QWidget(parent), m_ui(new Ui::NewTableForm), m_dbEngine(db), m_project(prj), m_table(new Table())
{
    m_ui->setupUi(this);

    const QVector<UserDataType*>& dts = m_project->getWorkingVersion()->getDataTypes();
    for(int i=0; i<dts.size(); i++)
    {
        m_ui->cmbNewColumnType->addItem(dts[i]->getIcon(), dts[i]->getName());
    }

    m_ui->lstColumns->header()->resizeSection(0,50);
}

NewTableForm::~NewTableForm()
{
    delete m_ui;
}

void NewTableForm::focusOnName()
{
    m_ui->txtTableName->setFocus();
}

void NewTableForm::changeEvent(QEvent *e)
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

void NewTableForm::setMainWindow(MainWindow *mw)
{
    this->m_mw = mw;
}

void NewTableForm::onAddColumn()
{
    // create the tree widget item
    QStringList a("");
    a.append(m_ui->txtNewColumnName->text());
    a.append(m_ui->cmbNewColumnType->currentText());

    int idx = m_ui->cmbNewColumnType->currentIndex();

    QTreeWidgetItem* item = new QTreeWidgetItem((QTreeWidget*)0, a);
    if(m_ui->chkPrimary->checkState())
    {
        item->setIcon(0, IconFactory::getKeyIcon());
    }
    item->setIcon(2, m_project->getWorkingVersion()->getDataTypes()[idx]->getIcon());
    m_ui->lstColumns->addTopLevelItem(item);

    // now create the Column object for it
    Column* col = new Column(m_ui->txtNewColumnName->text(),  m_project->getWorkingVersion()->getDataType(m_ui->cmbNewColumnType->currentText()), m_ui->chkPrimary->checkState()) ;
    m_table->addColumn(col);

}

void NewTableForm::onDeleteColumn()
{
    delete m_ui->lstColumns->currentItem();
}

void NewTableForm::onMoveColumnDown()
{
    if(m_ui->lstColumns->selectedItems().size() > 0)
    {
        QModelIndex x = m_ui->lstColumns->currentIndex();
        if(x.row() < m_ui->lstColumns->topLevelItemCount() - 1)
        {
            m_table->moveColumnDown(x.row());
            QTreeWidgetItem* w = new QTreeWidgetItem(m_ui->lstColumns->currentItem()->type());
            *w = *m_ui->lstColumns->currentItem();
            delete m_ui->lstColumns->currentItem();
            m_ui->lstColumns->insertTopLevelItem(x.row() + 1, w);
            m_ui->lstColumns->setCurrentItem(w);
        }
    }
}

void NewTableForm::onMoveColumnUp()
{
    if(m_ui->lstColumns->selectedItems().size() > 0)
    {
        QModelIndex x = m_ui->lstColumns->currentIndex();
        if(x.row() > 0)
        {
            m_table->moveColumnUp(x.row());
            QTreeWidgetItem* w = new QTreeWidgetItem(m_ui->lstColumns->currentItem()->type());
            *w = *m_ui->lstColumns->currentItem();
            delete m_ui->lstColumns->currentItem();
            m_ui->lstColumns->insertTopLevelItem(x.row() - 1, w);
            m_ui->lstColumns->setCurrentItem(w);
        }
    }
}

void NewTableForm::onItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{

}

void NewTableForm::onItemSelected(QTreeWidgetItem* current, int column)
{
    QModelIndex x = m_ui->lstColumns->currentIndex();

    m_ui->txtNewColumnName->setText(m_table->getColumn(x.row())->getName());
    m_ui->cmbNewColumnType->setCurrentIndex(m_project->getWorkingVersion()->getDataTypeIndex(m_table->getColumn(x.row())->getdataType()->getName()));
    m_ui->chkPrimary->setChecked(m_table->getColumn(x.row())->isPk());

    m_ui->btnAdd->setIcon(IconFactory::getApplyIcon());
}

