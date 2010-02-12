#include "NewTableForm.h"
#include "ui_NewTableForm.h"
#include "IconFactory.h"
#include "DatabaseEngine.h"
#include "Project.h"
#include "Version.h"
#include "UserDataType.h"

NewTableForm::NewTableForm(DatabaseEngine* db, Project* prj, QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::NewTableForm),
    m_dbEngine(db), m_project(prj)
{
    m_ui->setupUi(this);

    const QVector<UserDataType*>& dts = m_project->getWorkingVersion()->getDataTypes();
    for(int i=0; i<dts.size(); i++)
    {
        m_ui->cmbNewColumnType->addItem(dts[i]->getIcon(), dts[i]->getName());
    }
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

}
