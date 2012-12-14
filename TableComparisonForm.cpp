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
#include "TableUpdateGenerator.h"
#include "SqlHighlighter.h"
#include "db_DatabaseEngine.h"
#include "db_AbstractDTSupplier.h"
#include "VersionUpdateGenerator.h"
#include "InjectSqlDialog.h"
#include "core_ConnectionManager.h"
#include "MainWindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QString>
#include <QTextStream>

TableComparisonForm::TableComparisonForm(Mode m, QWidget *parent) : QWidget(parent), ui(new Ui::TableComparisonForm),
    m_leftTable(0), m_rightTable(0), m_spl(0), m_trees(), highlighter(0), m_mode(m), m_from(0), m_to(0), sqlList(), m_engine(0)
{
    ui->setupUi(this);

    if(m_mode == COMPARE_TABLES)
    {
        m_spl = new QSplitter(Qt::Horizontal, this);
        m_spl->addWidget(ui->grpLeft);
        m_spl->addWidget(ui->grpRight);
        ui->horizontalLayout_2->addWidget(m_spl);

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

        ui->lblFrom->hide();
        ui->lblTo->hide();
    }

    if(m_mode == COMPARE_VERSIONS)
    {
        ui->treeWidgetLeft->hide();
        ui->treeWidgetRight->hide();
        ui->txtTab1->hide();
        ui->txtTab2->hide();
    }

    QStringList a = Workspace::getInstance()->getAllVersions();
    for(int i=0; i<a.size(); i++)
    {
        ui->cmbVersionLeft->addItem(a[i]);
        ui->cmbVersionRight->addItem(a[i]);
    }

    highlighter = new SqlHighlighter(ui->textEdit->document(),Workspace::getInstance()->currentProjectsEngine()->getKeywords(),
                                     Workspace::getInstance()->currentProjectsEngine()->getDTSupplier()->numericTypes(),
                                     Workspace::getInstance()->currentProjectsEngine()->getDTSupplier()->booleanTypes(),
                                     Workspace::getInstance()->currentProjectsEngine()->getDTSupplier()->textTypes(),
                                     Workspace::getInstance()->currentProjectsEngine()->getDTSupplier()->blobTypes(),
                                     Workspace::getInstance()->currentProjectsEngine()->getDTSupplier()->dateTimeTypes(),
                                     Workspace::getInstance()->currentProjectsEngine()->getDTSupplier()->miscTypes(),
                                     Workspace::getInstance()->workingVersion()->getTables());

    QObject::connect(ui->cmbVersionLeft, SIGNAL(activated(QString)), this, SLOT(leftItemSelected(QString)));
    QObject::connect(ui->cmbVersionRight, SIGNAL(activated(QString)), this, SLOT(rightItemSelected(QString)));
}

TableComparisonForm::~TableComparisonForm()
{
    delete ui;
}

void TableComparisonForm::setFromVersion(Version *v)
{
    m_from = v;
    ui->cmbVersionLeft->setCurrentIndex(ui->cmbVersionLeft->findText(v->getVersionText()));
    if(m_from && m_to)
    {
        VersionUpdateGenerator vug = VersionUpdateGenerator(m_from, m_to);
        ui->textEdit->setText(addCommands(vug.getCommands()));
    }
}

void TableComparisonForm::setToVersion(Version *v)
{
    m_to = v;
    ui->cmbVersionRight->setCurrentIndex(ui->cmbVersionRight->findText(v->getVersionText()));
    if(m_from && m_to)
    {
        VersionUpdateGenerator vug = VersionUpdateGenerator(m_from, m_to);
        ui->textEdit->setText(addCommands(vug.getCommands()));
        m_engine = m_to->getProject()->getEngine();
    }
}

void TableComparisonForm::setupNewTable(const QString &v, Table *& table)
{
    Version* ver = Workspace::getInstance()->currentProject()->getVersionNamed(v);
    if(!ver)
    {
        qDebug () << "No version " << v;
        return;
    }

    // get the table from the specific version
    Table* t = ver->getTable(table->getName());
    if(!t)
    {
        // maybe the table was deleted

        // try to get the table by the UID
        int maj = table->version()->getMajor();
//        int min = table->version()->getMinor();
        if(maj > ver->getMajor())
        {
            // our right table is in a version above the selected one. Source UID of our table should lead downwards N steps to the table in
            // the requried version.
            QString srcTabUid = table->getSourceUid();
            Version* srcVer = table->version();
            Table* tab = 0;
            while(srcVer && srcVer->getVersionText() != ver->getVersionText())
            {
                tab = dynamic_cast<Table*>(UidWarehouse::instance().getElement(srcTabUid));
                if(!tab) return;
                srcTabUid = table->getSourceUid();
                srcVer = UidWarehouse::instance().getVersionForUid(srcTabUid);
            }
            if(tab == 0) return;
            table = tab;
            populateTree();
        }
        else
        {
            // our right table is in a version below (or in a minor) the slected one. See all the tables in the selected version which has
            // a source leading towards our table.
            const QVector<Table*> & tabs = ver->getTables();
            for(int i=0; i<tabs.size(); i++)
            {
                if(UidWarehouse::instance().related(table, tabs[i]))
                {
                    table = tabs[i];
                    populateTree();
                    break;
                }
            }
        }
    }
    else
    {
        table = t;
        populateTree();
    }
}

void TableComparisonForm::rightItemSelected(const QString& v)
{
    if(m_mode == COMPARE_TABLES)
    {
        setupNewTable(v, m_rightTable);
    }
    if(m_mode == COMPARE_VERSIONS)
    {
        Version* ver = Workspace::getInstance()->currentProject()->getVersionNamed(v);
        if(!ver)
        {
            return;
        }
        setToVersion(ver);
    }
}

void TableComparisonForm::leftItemSelected(const QString& v)
{
    if(m_mode == COMPARE_TABLES)
    {
        setupNewTable(v, m_leftTable);
    }
    if(m_mode == COMPARE_VERSIONS)
    {
        Version* ver = Workspace::getInstance()->currentProject()->getVersionNamed(v);
        if(!ver)
        {
            return;
        }
        setFromVersion(ver);
    }
}

QString TableComparisonForm::addCommands(const QStringList &cmds)
{
    QString finalSql;
    const QStringList& lines = cmds;
    for(int i=0; i<lines.size(); i++)
    {
        finalSql += lines.at(i);
        finalSql += "\n";
    }
    sqlList << cmds;
    return finalSql;
}

void TableComparisonForm::populateTree()
{
    ui->txtTab1->setText(m_leftTable->getName());
    if(m_rightTable) ui->txtTab2->setText(m_rightTable->getName());
    else ui->txtTab2->setText("Deleted");

    m_trees[LEFT]->clear();
    m_trees[RIGHT]->clear();

    populateColumns();

    TableUpdateGenerator gen (m_leftTable, m_rightTable, Workspace::getInstance()->currentProjectsEngine(), 0);

    // the update commands
    QString finalSql = addCommands(gen.commands());
    finalSql += addCommands(gen.droppedFksCommands());
    finalSql += addCommands(gen.newFksCommands());
    ui->textEdit->setText(finalSql);
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
    }

    // and now put in the other items from the right table is any left
    const QVector<Column*> rightCols = m_rightTable->getColumns();
    for(int j=0; j<rightCols.size(); j++)
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

// TODO: this is duplicate from SqlForm
void TableComparisonForm::onInject()
{
    InjectSqlDialog* injectDialog = new InjectSqlDialog(Workspace::getInstance()->currentProjectsEngine(), this, 0, "");
    injectDialog->setModal(true);
    if(injectDialog->exec() == QDialog::Accepted)
    {
        QString tSql;
        QStringList connectionNames = injectDialog->getSelectedConnections();
        bool error = false;
        for(int i=0; i< connectionNames.size(); i++)
        {
            Connection* c = ConnectionManager::instance()->getConnection(connectionNames.at(i));
            if(c)
            {
                QStringList tempSqlList;
                for(int j=0; j<sqlList.size(); j++)
                {
                    QString s = sqlList[j].trimmed();
                    if(s.endsWith("//"))
                    {
                        s = s.left(s.length() - 2);
                        s += ";";
                    }

                    // because of some funny reason "delimiter" is not accepted here, but if we copy/paste, it's ok
                    if(!s.startsWith("delimiter", Qt::CaseInsensitive))
                    {
                        tempSqlList.append(s);
                    }
                }

                if(!m_engine->executeSql(c, tempSqlList, QStringList(), tSql, injectDialog->getRollbackOnError()))
                {
                    QMessageBox::critical (this, tr("Error"), tr("<B>Cannot execute a query!</B><P>Reason: ") + m_engine->getLastError(), QMessageBox::Ok);
                    error = true;
                }
            }
        }
        MainWindow::instance()->setStatus(QString("SQL injection ") + (error?" failed ":" succeeded "), error);
    }
}

void TableComparisonForm::onSave()
{
    QString name = QFileDialog::getSaveFileName(this, tr("Save sql"), "", tr("SQL files (*.sql)"));
    QFile file(name);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return;
    }
    QTextStream out(&file);
    out << ui->textEdit->toPlainText() << "\n";
}
