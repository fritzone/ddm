#include "TableComparisonForm.h"
#include "ui_TableComparisonForm.h"
#include "core_Table.h"
#include "IconFactory.h"
#include "core_Column.h"
#include "core_UserDataType.h"
#include "Workspace.h"
#include "Version.h"
#include "Project.h"
#include "UidWarehouse.h"

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
    if(!ver)
    {
        qDebug () << "No version " << v;
        return;
    }

    // get the table from the specific version
    Table* t = ver->getTable(m_leftTable->getName());
    if(!t)
    {
        // maybe the table was deleted

        // try to get the table by the UID
        int maj = m_leftTable->version()->getMajor();
        int min = m_leftTable->version()->getMinor();
        qDebug() << "J:" << maj << " N:" << min;
        if(maj > ver->getMajor())
        {
            // our left table is in a version above the selected one. Source UID of our table should lead downwards N steps to the table in
            // the requried version.
            QString srcTabUid = m_leftTable->getSourceUid();
            Version* srcVer = UidWarehouse::instance().getVersionForUid(srcTabUid);
            Table* tab = 0;
            while(srcVer && srcVer->getVersionText() != ver->getVersionText())
            {
                tab = dynamic_cast<Table*>(UidWarehouse::instance().getElement(srcTabUid));
                if(!tab) return;
                srcTabUid = m_leftTable->getSourceUid();
                srcVer = UidWarehouse::instance().getVersionForUid(srcTabUid);
            }
            if(tab == 0) return;
            m_leftTable = t;
            populateTree();
        }
        else
        {
            qDebug() << "hm";
            // our table is in a version below (or in a minor) the slected one. See all the tables in the selected version which has
            // a source leading towards our table.
        }
    }
    else
    {
        qDebug() << "left found";
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
        QStringList colLeft;
        colLeft << ""<< ""<< leftCols.at(i)->getName() << leftCols[i]->getDataType()->getName();
        QTreeWidgetItem* itm = new QTreeWidgetItem(colItemLeft, colLeft);
        itm->setIcon(2, IconFactory::getColumnIcon());
        m_trees[LEFT]->addTopLevelItem(itm);

        if(leftCols[i]->isPk())
        {
            itm->setIcon(1, IconFactory::getKeyIcon());
        }
        itm->setIcon(3, IconFactory::getIconForDataType(leftCols[i]->getDataType()->getType()));

        if(m_rightTable)
        {
            populateClosestMatchingColumn(leftCols[i], colItemLeft, colItemRight);
        }
    }

    // and now put in the other items from the right table is any left
    const QVector<Column*> rightCols = m_rightTable->getColumns();
    for(int j=leftCols.size(); j<rightCols.size(); j++)
    {
        QStringList colRightText;
        colRightText << ""<< ""<< rightCols[j]->getName() << rightCols[j]->getDataType()->getName();

        QTreeWidgetItem* itmRight = new QTreeWidgetItem(colItemRight, colRightText);
        itmRight->setIcon(2, IconFactory::getColumnIcon());
        m_trees[RIGHT]->addTopLevelItem(itmRight);

        if(rightCols[j]->isPk())
        {
            itmRight->setIcon(1, IconFactory::getKeyIcon());
        }
        itmRight->setIcon(3, IconFactory::getIconForDataType(rightCols[j]->getDataType()->getType()));
        itmRight->setBackgroundColor(0, QColor(0xff, 0x66, 0));
        itmRight->setBackgroundColor(1, QColor(0xff, 0x66, 0));
        itmRight->setBackgroundColor(2, QColor(0xff, 0x66, 0));
        itmRight->setBackgroundColor(3, QColor(0xff, 0x66, 0));
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

void TableComparisonForm::populateClosestMatchingColumn(Column *left, QTreeWidgetItem* colItemLeft, QTreeWidgetItem* colItemRight)
{
    const QVector<Column*> rightCols = m_rightTable->getColumns();
    for(int j=0; j<rightCols.size(); j++)
    {
        if(rightCols[j]->getObjectUid() == left->getObjectUid()) // same table, same column
        {
            QStringList colRightText;
            colRightText << ""<< ""<< left->getName() << left->getDataType()->getName();

            QTreeWidgetItem* itmRight = new QTreeWidgetItem(colItemRight, colRightText);
            itmRight->setIcon(2, IconFactory::getColumnIcon());
            m_trees[RIGHT]->addTopLevelItem(itmRight);

            if(rightCols[j]->isPk())
            {
                itmRight->setIcon(1, IconFactory::getKeyIcon());
            }
            itmRight->setIcon(3, IconFactory::getIconForDataType(rightCols[j]->getDataType()->getType()));
        }
        else    // same table, different column.
        {
            // is the column left at i equal with the column right j?
            int eqind = left->checkEquality(rightCols[j]);
            if(eqind == 0)
            {
                int i = m_leftTable->getIndexOfColumn(left);
                // they are equal
                if(i ==j)
                {
                    // now we foudn that the two columns occupy the same position.
                    QStringList colRight;
                    colRight << ""<< ""<< rightCols.at(j)->getName() << rightCols[j]->getDataType()->getName();

                    QTreeWidgetItem* itmRight = new QTreeWidgetItem(colItemRight, colRight);
                    itmRight->setIcon(2, IconFactory::getColumnIcon());
                    m_trees[RIGHT]->addTopLevelItem(itmRight);

                    if(rightCols[j]->isPk())
                    {
                        itmRight->setIcon(1, IconFactory::getKeyIcon());
                    }
                    itmRight->setIcon(3, IconFactory::getIconForDataType(rightCols[j]->getDataType()->getType()));

                }
                else
                {
                    // the two columns do not occupy the same position, so it means one of them was moved
                    if(i < j)   // means the left column comes before the right... possibly a new column was inserted in the right table or one deleted from the left
                    {
                        // try to move down the left ... (j - i) steps
                        QTreeWidgetItem* empty = new QTreeWidgetItem(QStringList());
                        for(int k = 0; k<j-i; k++)
                        {
                            colItemLeft->insertChild(i, empty);
                        }

                        // and add the curret item
                        QStringList colRight;
                        colRight << ""<< ""<< rightCols.at(j)->getName() << rightCols[j]->getDataType()->getName();

                        QTreeWidgetItem* itmRight = new QTreeWidgetItem(colItemRight, colRight);
                        itmRight->setIcon(2, IconFactory::getColumnIcon());
                        m_trees[RIGHT]->addTopLevelItem(itmRight);

                        if(rightCols[j]->isPk())
                        {
                            itmRight->setIcon(1, IconFactory::getKeyIcon());
                        }
                        itmRight->setIcon(3, IconFactory::getIconForDataType(rightCols[j]->getDataType()->getType()));
                    }
                    else        // the right comes before the left, a column was deleted from the right table (or added to the left)
                    {
                        // insert empty items and then the requried one
                        QTreeWidgetItem* empty = new QTreeWidgetItem(colItemRight, QStringList());
                        for(int k = 0; k<i-j; k++)
                        {
                            m_trees[RIGHT]->addTopLevelItem(empty);
                        }
                        QStringList colRight;
                        colRight << ""<< ""<< rightCols.at(j)->getName() << rightCols[j]->getDataType()->getName();

                        QTreeWidgetItem* itmRight = new QTreeWidgetItem(colItemRight, colRight);
                        itmRight->setIcon(2, IconFactory::getColumnIcon());
                        m_trees[RIGHT]->addTopLevelItem(itmRight);

                        if(rightCols[j]->isPk())
                        {
                            itmRight->setIcon(1, IconFactory::getKeyIcon());
                        }
                        itmRight->setIcon(3, IconFactory::getIconForDataType(rightCols[j]->getDataType()->getType()));

                    }
                }
            }
            else // these two are not equal.
            {
                int i = m_leftTable->getIndexOfColumn(left);
                if (i == j && eqind > -1)
                {
                    QStringList colRight;
                    colRight << ""<< ""<< rightCols.at(j)->getName() << rightCols[j]->getDataType()->getName();

                    QTreeWidgetItem* itmRight = new QTreeWidgetItem(colItemRight, colRight);
                    itmRight->setIcon(2, IconFactory::getColumnIcon());
                    m_trees[RIGHT]->addTopLevelItem(itmRight);

                    if(rightCols[j]->isPk())
                    {
                        itmRight->setIcon(1, IconFactory::getKeyIcon());
                    }
                    itmRight->setIcon(3, IconFactory::getIconForDataType(rightCols[j]->getDataType()->getType()));
                    if(eqind != -1) itmRight->setBackgroundColor(eqind - 1, QColor(0xff, 0x66, 0));
                    else
                    {
                        itmRight->setBackgroundColor(0, QColor(0xff, 0x66, 0));
                        itmRight->setBackgroundColor(1, QColor(0xff, 0x66, 0));
                        itmRight->setBackgroundColor(2, QColor(0xff, 0x66, 0));
                        itmRight->setBackgroundColor(3, QColor(0xff, 0x66, 0));
                    }
                }
                else
                {

                }
            }

        }
    }

}

