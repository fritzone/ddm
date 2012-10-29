#include "TableComparisonForm.h"
#include "ui_TableComparisonForm.h"
#include "core_Table.h"
#include "IconFactory.h"
#include "core_Column.h"
#include "core_UserDataType.h"
#include "Workspace.h"
#include "Version.h"
#include "Project.h"

TableComparisonForm::TableComparisonForm(QWidget *parent) : m_leftTable(0), m_rightTable(0),
    QWidget(parent),
    ui(new Ui::TableComparisonForm)
{
    ui->setupUi(this);

    m_spl = new QSplitter(Qt::Horizontal, this);
    m_spl->addWidget(ui->grpLeft);
    m_spl->addWidget(ui->grpRight);
    ui->horizontalLayout->addWidget(m_spl);

    m_trees[LEFT] = ui->treeWidgetLeft;
    m_trees[RIGHT] = ui->treeWidgetRight;

    m_trees[LEFT]->header()->resizeSection(0, 100);
    m_trees[LEFT]->header()->resizeSection(1, 32);
    m_trees[LEFT]->header()->resizeSection(2, 150);
    m_trees[LEFT]->header()->resizeSection(3, 150);

    m_trees[RIGHT]->header()->resizeSection(0, 100);
    m_trees[RIGHT]->header()->resizeSection(1, 32);
    m_trees[RIGHT]->header()->resizeSection(2, 150);
    m_trees[RIGHT]->header()->resizeSection(3, 150);

    QStringList a = Workspace::getInstance()->getAllVersions();
    for(int i=0; i<a.size(); i++)
    {
        ui->cmbVersionLeft->addItem(a[i]);
        ui->cmbVersionRight->addItem(a[i]);
    }

    QObject::connect(ui->cmbVersionLeft, SIGNAL(activated(QString)), this, SLOT(leftItemSelected(QString)));
    QObject::connect(ui->cmbVersionRight, SIGNAL(activated(QString)), this, SLOT(rightItemSelected(QString)));
}

TableComparisonForm::~TableComparisonForm()
{
    delete ui;
}

void TableComparisonForm::rightItemSelected(const QString& v)
{

}

void TableComparisonForm::leftItemSelected(const QString& v)
{
    Version* ver = Workspace::getInstance()->currentProject()->getVersionNamed(v);
    if(!ver) return;

    // get the table from the specific version
    Table* t = ver->getTable(m_leftTable->getName());
    if(!t)
    {
        // try to get the table y the UID
        int maj = m_leftTable->version()->getMajor();
        int min = m_leftTable->version()->getMinor();
        if(maj > ver->getMajor())
        {
            // our left table is in a version above the selected one. Source UID of our table should lead downwards N steps to the table in
            // the requried version.
        }
        else
        {
            // our table is in a version below (or in a minor) the slected one. See all the tables in the selected version which has
            // a source leading towards our table.
        }
    }
    else
    {
        m_leftTable = t;
        populateTree();
    }
}

void TableComparisonForm::populateTree()
{
    ui->txtTab1->setText(m_leftTable->getName());
    if(m_rightTable) ui->txtTab2->setText(m_rightTable->getName());
    else ui->txtTab2->setText("Deleted");

    m_trees[LEFT]->clear();
    m_trees[RIGHT]->clear();

    populateColumns();
}

void TableComparisonForm::populateColumns()
{
    QStringList lstCols;
    lstCols << QObject::tr("");
    QTreeWidgetItem* colItemLeft = new QTreeWidgetItem(lstCols);
    m_trees[LEFT]->addTopLevelItem(colItemLeft);
    colItemLeft->setIcon(0, IconFactory::getColumnIcon());
    QTreeWidgetItem* colItemRight = new QTreeWidgetItem(lstCols);
    m_trees[RIGHT]->addTopLevelItem(colItemRight);
    colItemRight->setIcon(0, IconFactory::getColumnIcon());

    colItemRight->setExpanded(true);
    colItemLeft->setExpanded(true);

    const QVector<Column*> leftCols = m_leftTable->getColumns();
    for(int i=0; i<leftCols.size(); i++)
    {
        QStringList col;
        col << ""<< ""<< leftCols.at(i)->getName() << leftCols[i]->getDataType()->getName();
        QTreeWidgetItem* itm = new QTreeWidgetItem(colItemLeft, col);
        itm->setIcon(2, IconFactory::getColumnIcon());
        m_trees[LEFT]->addTopLevelItem(itm);

        if(leftCols[i]->isPk())
        {
            itm->setIcon(1, IconFactory::getKeyIcon());
        }
        itm->setIcon(3, IconFactory::getIconForDataType(leftCols[i]->getDataType()->getType()));

        if(m_rightTable)
        {
            const QVector<Column*> rightCols = m_rightTable->getColumns();
            for(int j=0; j<rightCols.size(); j++)
            {
                if(rightCols[j]->getObjectUid() == leftCols[i]->getObjectUid()) // same table?
                {

                    QTreeWidgetItem* itmRight = new QTreeWidgetItem(colItemRight, col);
                    itmRight->setIcon(2, IconFactory::getColumnIcon());
                    m_trees[RIGHT]->addTopLevelItem(itmRight);

                    if(rightCols[j]->isPk())
                    {
                        itmRight->setIcon(1, IconFactory::getKeyIcon());
                    }
                    itmRight->setIcon(3, IconFactory::getIconForDataType(rightCols[j]->getDataType()->getType()));
                }
                else
                {// walk up/down the list of source uids ... find the correspondign table
                    // rightCols[j]->getSourceUid() == leftCols[i]->getObjectUid() ||

//                    if(i ==j)
//                    {
//                        // now we foudn the two columns occupy the same position.
//                        // see if they are equal too

//                        int eqind = leftCols[i]->checkEquality(rightCols[j]);
//                        if(eqind == 0)
//                        {// equal
//                        }
//                        else
//                        {// not equal
//                            itmRight->setBackgroundColor(eqind, Qt::red);
//                        }
//                    }
//                    else
//                    {
//                        // the two columns do not occupy the same position

//                    }

                }
            }

        }

    }
}


void TableComparisonForm::setLeftTable(Table *t)
{
    m_leftTable = t;
    ui->cmbVersionLeft->setCurrentIndex(ui->cmbVersionLeft->findText(t->version()->getVersionText()));
}


void TableComparisonForm::setRightTable(Table *t)
{
    m_rightTable = t;
    ui->cmbVersionRight->setCurrentIndex(ui->cmbVersionRight->findText(t->version()->getVersionText()));
}
