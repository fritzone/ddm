#include "qbr_QueryItemListDialog.h"
#include "ui_QueryItemListDialog.h"

#include "Workspace.h"
#include "Version.h"
#include "Table.h"
#include "IconFactory.h"
#include "TableInstance.h"
#include "DatabaseEngine.h"
#include "DatabaseBuiltinFunction.h"
#include "qbr_Query.h"
#include "Column.h"
#include "DataType.h"

#include <QListWidget>
#include <QMessageBox>

QueryItemListDialog::QueryItemListDialog(QueryGraphicsHelper* helper, QueryGraphicsHelper::ListType t, QWidget *parent) : QDialog(parent), ui(new Ui::QueryItemListDialog), m_selected(), m_helper(helper)
{
    ui->setupUi(this);

    ui->txtInput->hide();
    ui->frameButtonBox->hide();

    switch(t)
    {
    case QueryGraphicsHelper::LIST_TABLES:

        if(Workspace::getInstance()->currentProjectIsOop())
        {
            for(int i=0; i<Workspace::getInstance()->workingVersion()->getTableInstances().size(); i++)
            {
                TableInstance* tabI = Workspace::getInstance()->workingVersion()->getTableInstances().at(i);
                QListWidgetItem* lwi = new QListWidgetItem(tabI->getName());
                lwi->setFont(QFont("Arial", 14, 2));
                lwi->setIcon(IconFactory::getTabinstIcon());
                ui->lstValues->addItem(lwi);
            }
        }
        else
        {
            for(int i=0; i<Workspace::getInstance()->workingVersion()->getTables().size(); i++)
            {
                Table* tabI = Workspace::getInstance()->workingVersion()->getTables().at(i);
                QListWidgetItem* lwi = new QListWidgetItem(tabI->getName());
                lwi->setFont(QFont("Arial", 14, 2));
                lwi->setIcon(IconFactory::getTablesIcon());
                ui->lstValues->addItem(lwi);
            }

        }

        break;
    case QueryGraphicsHelper::INPUT_TEXT:
        ui->lstValues->hide();
        ui->txtInput->show();
        resize(ui->txtInput->width(), ui->txtInput->height());
        break;
    }
}

QueryItemListDialog::QueryItemListDialog(QueryGraphicsHelper* helper, QStringList lst, QList<QIcon> icons, bool checks, QWidget *parent) : QDialog(parent), ui(new Ui::QueryItemListDialog), m_selected(), m_helper(helper)
{
    ui->setupUi(this);
    ui->txtInput->hide();
    ui->frameButtonBox->hide();

    for(int i=0; i<lst.size(); i++)
    {
        QString str = lst.at(i);
        if(str.startsWith("@"))
        {
            str = str.right(str.length() - 1);
            m_beforeAfter[str] = lst.at(i);
        }


        QListWidgetItem* lwi = new QListWidgetItem(str);
        lwi->setFont(QFont("Arial", 10, 2));
        lwi->setIcon(icons.at(i));
        if(checks)
        {
            QFlags <Qt::ItemFlag> t = lwi->flags();
            t |= Qt::ItemIsUserCheckable;
            lwi->setFlags(t);
            lwi->setCheckState(Qt::Unchecked);
        }
        ui->lstValues->addItem(lwi);
    }
    resize(200, 20 * (lst.size()));
    ui->lstValues->resize(200, 20 * (lst.size()));

}

void QueryItemListDialog::setColumnMode()
{
    ui->txtInput->hide();
    ui->lstValues->hide();
    ui->frameButtonBox->show();
    ui->btnAcceptText->hide();
    ui->btnBetween->hide();
    ui->btnBitwise->hide();
    ui->btnComparison->hide();
    ui->btnDistinct->hide();
    ui->btnExists->hide();
    ui->btnFunctions->hide();
    ui->btnIn->hide();
    ui->btnIs->hide();
    ui->btnLike->hide();
    ui->btnMath->hide();
    ui->btnNot->hide();
    ui->btnStar->hide();
    ui->txtQueryText->hide();
    ui->label->hide();
    ui->frame_2->hide();
    ui->frame_3->hide();
    ui->btnRollup->show();
    resize(200, 100);
}

void QueryItemListDialog::showSymbolPanel()
{
    ui->txtInput->hide();
    ui->lstValues->hide();
    ui->frameButtonBox->show();

    m_mathMenu = new QMenu(this);
    m_bitMenu = new QMenu(this);
    m_functionsMenu = new QMenu(this);
    m_comparisonMenu = new QMenu(this);
    m_tablesMenu = new QMenu(this);
    m_functionsMathMenu = new QMenu(m_functionsMenu);
    m_functionsMathMenu->setTitle("Math");
    m_functionsAggregateMenu = new QMenu(m_functionsMenu);
    m_functionsAggregateMenu->setTitle("Aggregate");

    m_mathMenu->addAction(IconFactory::getPlusIcon(), strMathPlus);
    m_mathMenu->addAction(IconFactory::getMinusIcon(), strMathMinus);
    m_mathMenu->addAction(IconFactory::getMultiplyIcon(), strMathMultiply);
    m_mathMenu->addAction(IconFactory::getDivideIcon(), strMathDivide);
    m_mathMenu->addAction(IconFactory::getModuloIcon(), strMathMod);

    m_bitMenu->addAction(IconFactory::getNotIcon(), strLogNot);
    m_bitMenu->addAction(IconFactory::getBinaryOrIcon(), strLogOr);
    m_bitMenu->addAction(IconFactory::getBinaryAndIcon(), strLogAnd);
    m_bitMenu->addAction(IconFactory::getNegIcon(), strLogNeg);
    m_bitMenu->addAction(IconFactory::getXorIcon(), strLogXor);
    m_bitMenu->addAction(IconFactory::getLeftShiftIcon(), strLogLShift);
    m_bitMenu->addAction(IconFactory::getRightShiftIcon(), strLogRShift);

    m_comparisonMenu->addAction(IconFactory::getEqualIcon(), strCmpEqual);
    m_comparisonMenu->addAction(IconFactory::getNotEqIcon(), strCmpNotEqual);
    m_comparisonMenu->addAction(IconFactory::getLessIcon(), strCmpLess);
    m_comparisonMenu->addAction(IconFactory::getGreaterIcon(), strCmpGreater);
    m_comparisonMenu->addAction(IconFactory::getLessOrEqualIcon(), strCmpLessOrEqual);
    m_comparisonMenu->addAction(IconFactory::getGreaterOrEqualIcon(), strCmpGreaterOrEqual);

    QVector<DatabaseBuiltinFunction> functions = Workspace::getInstance()->currentProjectsEngine()->getBuiltinFunctions();
    for(int i=0; i<functions.size(); i++)
    {
        QAction* tempAction = new QAction(IconFactory::getFunctionIcon(), functions.at(i).getNiceName().toUpper(), this);
        tempAction->setData(functions.at(i).getName().toUpper());
        tempAction->setStatusTip(functions.at(i).getDescription());

        if(functions.at(i).getType() == FT_NUMERIC)
        {
            m_functionsMathMenu->addAction(tempAction);
        }
        if(functions.at(i).getType() == FT_AGGREGATE)
        {
            m_functionsAggregateMenu->addAction(tempAction);
        }
    }

    m_functionsMenu->addMenu(m_functionsMathMenu);
    m_functionsMenu->addMenu(m_functionsAggregateMenu);

    /* Now building the menu system for the columns */
    Query* q = m_helper->getQuery();
    QVector<const Table*> tables = q->getTables();
    for(int i=0; i<tables.size(); i++)
    {
        QAction* tempAction = new QAction(IconFactory::getTablesIcon(), tables.at(i)->getName(), this);
        m_tablesMenu->addAction(tempAction);
        QMenu* colMenu = new QMenu();
        QStringList cols = tables.at(i)->fullColumns();
        for(int j=0; j<cols.size(); j++)
        {
            Column* c = tables.at(i)->getColumn(cols.at(j));
            if(c==0) c = tables.at(i)->getColumnFromParents(cols.at(j));
            if(c==0) continue;
            bool canGo = false;
            if(m_columnsToShow.isEmpty())
            {
                canGo = true;
            }
            else
            {
                // search if the found column is in the m_columnsToShow vector
                for(int k=0; k<m_columnsToShow.size(); k++)
                {
                    if(m_columnsToShow.at(k)->getName() == c->getName() && m_columnsToShow.at(k)->getTable()->getName() == tables.at(i)->getName())
                    {
                        canGo = true;
                    }
                }
            }

            if(canGo)
            {
                QAction* colAction = new QAction(c->getDataType()->getIcon(), c->getName(), this);
                colAction->setData(QString("#") + tables.at(i)->getName()+QString("+")+c->getName());
                colMenu->addAction(colAction);
            }

        }
        tempAction->setMenu(colMenu);
    }

    m_mathMenu->connect(m_mathMenu, SIGNAL(triggered(QAction*)), this, SLOT(actionTriggered(QAction*)));
    m_bitMenu->connect(m_bitMenu, SIGNAL(triggered(QAction*)), this, SLOT(actionTriggered(QAction*)));
    m_functionsMenu->connect(m_functionsMenu, SIGNAL(triggered(QAction*)), this, SLOT(actionTriggered(QAction*)));
    m_comparisonMenu->connect(m_comparisonMenu, SIGNAL(triggered(QAction*)), this, SLOT(actionTriggered(QAction*)));
    m_tablesMenu->connect(m_tablesMenu, SIGNAL(triggered(QAction*)), this, SLOT(actionTriggered(QAction*)));

    resize(410, 250);

    ui->btnMath->setMenu(m_mathMenu);
    ui->btnBitwise->setMenu(m_bitMenu);
    ui->btnFunctions->setMenu(m_functionsMenu);
    ui->btnComparison->setMenu(m_comparisonMenu);
    ui->btnColumn->setMenu(m_tablesMenu);
    ui->btnRollup->hide();

    ui->grpExpressionButtons->setCurrentIndex(-1);
}

void QueryItemListDialog::pageRequested(int a)
{
    if(a == 1)
    {
        m_selected = QString(strRemove);
        close();
    }
}

void QueryItemListDialog::setText(const QString &a)
{
    ui->txtInput->setText(a);
}

void QueryItemListDialog::actionTriggered(QAction* a)
{
    m_selected = a->text();
    if(a->data().canConvert<QString>())
    {
        m_selected = a->data().toString();
    }
    close();
}

QueryItemListDialog::~QueryItemListDialog()
{
    delete ui;
}


void QueryItemListDialog::onDblClickItem(QListWidgetItem* itm)
{
    if(m_beforeAfter.contains(itm->text()))
    {
        m_selected = m_beforeAfter[itm->text()];
    }
    else
    {
        m_selected = itm->text();
    }
    close();
}

void QueryItemListDialog::onTxtInputKeyPress()
{
    m_selected = ui->txtInput->text();
    close();
}

void QueryItemListDialog::btnDistinctClicked()
{
    m_selected = strDistinct;
    close();
}

void QueryItemListDialog::btnStarClicked()
{
    m_selected = strStar;
    close();
}

void QueryItemListDialog::btnLikeClicked()
{
    m_selected = strLike;
    close();
}

void QueryItemListDialog::btnIsClicked()
{
    m_selected = strIs;
    close();
}

void QueryItemListDialog::btnInClicked()
{
    m_selected = strIn;
    close();
}

void QueryItemListDialog::btnBetweenClicked()
{
    m_selected = strBetween;
    close();
}

void QueryItemListDialog::btnExistsClicked()
{
    m_selected = strExists;
    close();
}

void QueryItemListDialog::btnNotClicked()
{
    m_selected = strNotText;
    close();
}

void QueryItemListDialog::btnAcceptText()
{
    m_selected = QString("$") + ui->txtQueryText->text();
    close();
}

void QueryItemListDialog::btnRollupClicked()
{
    m_selected = strRollup;
    close();
}

void QueryItemListDialog::btnNullClicked()
{
    m_selected = strNull;
    close();
}

void QueryItemListDialog::btnCloseParanthesisClicked()
{
    m_selected = strCloseParantheses;
    close();
}

void QueryItemListDialog::btnOpenParanthesisClicked()
{
    m_selected = strOpenParantheses;
    close();
}
