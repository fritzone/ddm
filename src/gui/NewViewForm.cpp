
#include "NewViewForm.h"
#include "ui_NewViewForm.h"
#include "db_DatabaseEngine.h"
#include "SqlHighlighter.h"
#include "qbr_QueryGraphicsView.h"
#include "qbr_QueryGraphicsScene.h"
#include "qbr_QueryGraphicsItem.h"
#include "SqlSourceTreeItem.h"
#include "Project.h"
#include "Configuration.h"
#include "core_View.h"
#include "qbr_SelectQuery.h"
#include "qbr_SingleExpressionQueryComponent.h"
#include "qbr_SelectQueryAsComponent.h"
#include "core_Column.h"
#include "Workspace.h"
#include "Version.h"
#include "VersionGuiElements.h"
#include "gui_HelpWindow.h"
#include "MainWindow.h"
#include "InjectSqlDialog.h"
#include "ConnectionManager.h"
#include "FrameForLineNumbers.h"
#include "GuiElements.h"
#include "WidgetForSpecificProperties.h"

#include <QScrollBar>
#include <QVBoxLayout>
#include <QtGui>
#include <QToolTip>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>

NewViewForm::NewViewForm(Version* v, bool queryBuilder, QueryGraphicsHelper* c, QWidget *parent) :
    SourceCodePresenterWidget(v, parent),
    ui(new Ui::NewViewForm),
    m_qgh(c),
    m_queryBuilder(queryBuilder), m_updateSqlAfterNameChange(true),
    m_autoChange(false), m_version(v), m_mainWsp(0)
{
    ui->setupUi(this);
    txtSql = new TextEditWithCodeCompletion(ui->groupBox_3);
    txtSql->setObjectName(QString::fromUtf8("txtSql"));
    txtSql->setMinimumSize(QSize(0, 200));

    QObject::connect(txtSql, SIGNAL(textChanged()), this, SLOT(onSqlChange()));

    if(m_queryBuilder)
    {
        ui->horizontalLayout_8->addWidget(txtSql);

        m_qgs = new QueryGraphicsScene(c, ui->groupBox);

        m_qgv = new QueryGraphicsView(ui->groupBox);
        m_qgv->setObjectName(QString::fromUtf8("queryGraphicsView"));
        m_qgv->setDragMode(QGraphicsView::RubberBandDrag);
        m_qgv->setAcceptDrops(true);
        m_qgv->setScene(m_qgs);

        m_qgv->setSceneRect(0,0, 10000, 10000);
        m_qgv->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        m_qgv->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

        ui->verticalLayout->insertWidget(1, m_qgv);

        txtSql->setReadOnly(true);
        ui->tabWidget->setCurrentIndex(0);
    }
    else
    {
        ui->tabWidget->removeTab(0);
        ui->tabWidget->removeTab(0);    //remains only the SQL tab

        delete ui->txtViewName;

        ui->txtViewName = new QLineEdit(ui->groupBox_3);
        ui->txtViewName->setObjectName(QString::fromUtf8("txtViewName"));
        ui->txtViewName->setToolTip(QObject::tr("The name of the View"));
        ui->groupBox->hide();
        ui->verticalLayout_4->insertWidget(0, ui->txtViewName);
        connect(ui->txtViewName, SIGNAL(textChanged(QString)), this, SLOT(onNameChange(QString)));

        FrameForLineNumbers* m_frameForLineNumbers = new FrameForLineNumbers(this);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(m_frameForLineNumbers->sizePolicy().hasHeightForWidth());
        m_frameForLineNumbers->setSizePolicy(sizePolicy);
        m_frameForLineNumbers->setMinimumSize(QSize(48, 0));
        m_frameForLineNumbers->setMaximumSize(QSize(48, 16777215));
        m_frameForLineNumbers->setFrameShape(QFrame::StyledPanel);
        m_frameForLineNumbers->setFrameShadow(QFrame::Raised);
        ui->horizontalLayout_8->addWidget(m_frameForLineNumbers);
        ui->horizontalLayout_8->addWidget(txtSql);
        txtSql->setLineNumberFrame(m_frameForLineNumbers);
        txtSql->updateLineNumbers();
        txtSql->setFocus();
    }
}

NewViewForm::~NewViewForm()
{
    delete ui;
}

void NewViewForm::changeEvent(QEvent *e)
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

void NewViewForm::disableEditingControls(bool dis)
{
    ui->lstColumnsForView->setDisabled(dis);
    m_mainWsp->setDisabled(dis);
    m_qgv->setDisabled(dis);
}

void NewViewForm::setGraphicsItem(QueryGraphicsItem * itm)
{
    if(m_queryBuilder)
    {
        m_qgs->clear();
        m_qgv->centerOn(0, 0);
        m_qgs->addItem(itm);
    }
}

void NewViewForm::scrollTo(int hor, int ver)
{
    if(m_queryBuilder)
    {
        m_qgv->centerOn(hor, ver);
        m_qgs->invalidate();
    }
}

void NewViewForm::getCenter(int &x, int &y)
{
    if(m_queryBuilder)
    {
//        QPoint centre = m_qgv->viewport()->mapTo(this, QPoint(0,0));;
//        x = m_qgv->mapToScene(centre).x();
//        y = m_qgv->mapToScene(centre).y();
        QPoint centre = m_qgv->viewport()->rect().center();
        x = centre.x() + m_qgv->horizontalScrollBar()->value();;
        y = centre.y() + m_qgv->verticalScrollBar()->value();;

    }
}

void NewViewForm::presentSql(Project* p, Version *)
{
    // TODO: this is still not right, it's here just to work.
    QHash<QString, QString> opts = Configuration::instance().sqlGenerationOptions();
    QStringList sqls = getSqlSourceEntity()->generateSqlSource(p->getEngine()->getSqlGenerator(), opts, 0);
    QString sql = "";
    for(int i=0; i<sqls.size(); i++)
    {
        sql += sqls.at(i);
    }
    txtSql->setPlainText(sql);
}

void NewViewForm::setView(View *v)
{
    m_view = v;

    ui->txtViewName->setText(v->getName());

    ui->btnUndelete->hide();

    // TODO: duplicate with the other forms ... at least, the logic!
    if(!m_view->wasLocked())
    {
        if(m_view->isDeleted())
        {
            ui->frameForUnlockButton->show();
            ui->btnLock->hide();
            ui->btnUndelete->show();
        }
        else
        {
            ui->frameForUnlockButton->hide();
        }
    }
    else
    {
        ui->frameForUnlockButton->show();
        if(m_view->lockState() == LockableElement::LOCKED)
        {
            ui->btnLock->setIcon(IconFactory::getLockedIcon());
            ui->btnLock->blockSignals(true);
            ui->btnLock->setChecked(false);
            ui->btnLock->blockSignals(false);
//            ui->frame_3->setEnabled(false);
            disableEditingControls(true);
            ui->btnLock->setToolTip(QObject::tr("This view is <b>locked</b>. Click this button to unlock it."));
        }
        else
        {
            ui->btnLock->setIcon(IconFactory::getUnLockedIcon());
            ui->btnLock->blockSignals(true);
            ui->btnLock->setChecked(true);
            ui->btnLock->blockSignals(false);
//            ui->frame_3->setEnabled(true);
            disableEditingControls(false);
            ui->btnLock->setToolTip(QObject::tr("This view is <b>unlocked</b>. Click this button to lock it."));
        }

        if(m_view->isDeleted())
        {
            ui->btnLock->hide();
            ui->btnUndelete->show();
        }
    }

    if(m_qgh) // do this only if we are buildingteh query with the query builder
    {
        // and now create the tab widgets for the SPs
        DatabaseEngine* engine = Workspace::getInstance()->currentProjectsEngine();
        m_mainWsp = new WidgetForSpecificProperties(engine, m_view, this);
        QVector<SpInstance*> allSps = m_view->getSpInstances(engine);
        m_mainWsp->feedInSpecificProperties(allSps, uidView);
        QString dbName = engine->getDatabaseEngineName();
        ui->tabWidget->insertTab(2, m_mainWsp, IconFactory::getIconForDatabase(dbName), dbName);

        SelectQuery* sq = dynamic_cast<SelectQuery*>(m_qgh->getQuery());
        QStringList columnNamesForView;
        if(sq)
        {
            QVector<const QueryComponent*> sel = sq->getSelectedComponents();
            for(int i=0; i<sel.size(); i++)
            {
                bool added = false;
                QVector<const ColumnOfTabWithTabInstance*> columns;
                SingleExpressionQueryComponent* seq = dynamic_cast<SingleExpressionQueryComponent*>(const_cast<QueryComponent*>(sel.at(i)));
                if(seq->hasStar())
                {
                    columnNamesForView.clear();
                    break;
                }
                if(seq)
                {
                    if(seq->hasAtLeastOneColumnSelected())
                    {
                        columns = seq->getColumns();
                    }

                    for(int j=0; j<columns.size(); j++)
                    {
                        QStringList a; a.append(columns.at(j)->c->getName()); a.append(columns.at(j)->c->getName());
                        columnNamesForView.append(columns.at(j)->c->getName());
                        QTreeWidgetItem* w = new QTreeWidgetItem(a);
                        ui->lstColumnsForView->addTopLevelItem(w);
                        added = true;
                    }

                    if(const SelectQueryAsComponent* as = seq->hasAs())             // and the last one is added as the alias
                    {
                        QStringList a; a.append(as->getAs()); a.append(sel.at(i)->get());
                        columnNamesForView.append(as->getAs());
                        QTreeWidgetItem* w = new QTreeWidgetItem(a);
                        ui->lstColumnsForView->addTopLevelItem(w);
                        added = true;
                    }
                    // check if this is a plain * and in that case reset the list, we don't want that
                }

                if(!added)
                {
                    QString n;n.setNum(i);
                    QStringList a; a.append(QString("COL")+n); a.append(sel.at(i)->get());
                    columnNamesForView.append(QString("COL")+n);
                    QTreeWidgetItem* w = new QTreeWidgetItem(a);
                    ui->lstColumnsForView->addTopLevelItem(w);
                }
            }
            v->setColumnNames(columnNamesForView);
        }
    }
}

void NewViewForm::onNameChange(QString a)
{
    a = a.trimmed();
    if(a.isEmpty())
    {
        return;
    }

    QPalette pal;
    pal.setColor(QPalette::Text, Qt::black);
    ui->txtViewName->setPalette(pal);

    // and see if there is a table, column or KEYWORD called "a"
    View* other = m_version->getView(a);
    if(m_version->getProject()->getEngine()->getKeywords().contains(a, Qt::CaseInsensitive) || (other && other != m_view) )
    {
        QPalette pal;
        pal.setColor(QPalette::Text, Qt::red);
        ui->txtViewName->setPalette(pal);

        QString netTooltip = tr("This is not an allowed name for the view.");
        if(m_version->getProject()->getEngine()->getKeywords().contains(a, Qt::CaseInsensitive))
        {
            netTooltip += "<p><b>" + a + "</b>" + tr(" is a reserved keyword for this database. DDM does not allow this to avoid future confusion.");
        }
        else
        {
            netTooltip += "<p>" + tr("There is already a view called <b> ") + a;
        }

        QToolTip::showText(ui->txtViewName->mapToGlobal(QPoint()), netTooltip, ui->txtViewName);
        ui->txtViewName->setToolTip(netTooltip);
        return;
    }
    else
    {
        ui->txtViewName->setToolTip(tr("The name of the view"));
    }

    m_view->setName(a);
    m_view->setDisplayText(a);

    if(m_updateSqlAfterNameChange)
    {
        m_autoChange = true;
        presentSql(Workspace::getInstance()->currentProject(), m_version);
        m_autoChange = false;
    }
}

void NewViewForm::onSqlChange()
{
    if(!m_queryBuilder)
    {
        m_view->setSql(txtSql->toPlainText());
        if(!m_autoChange) getViewNameFromSql();
    }
}

QString NewViewForm::getViewNameFromSql()
{
    QString t = txtSql->toPlainText();
    int i = t.indexOf(QString("VIEW"), 0, Qt::CaseInsensitive);
    if(i != -1)
    {
        i += 4;
        while(i<t.length() && t.at(i).isSpace())
        {
            i++;
        }
        QString n = "";
        while(i<t.length() && !t.at(i).isSpace())
        {
            n += t.at(i);
            i++;
        }
        m_updateSqlAfterNameChange = false;
        ui->txtViewName->setText(n);
        m_view->setDisplayText(n);
        m_view->setName(n);

        m_updateSqlAfterNameChange = true;
        return n;
    }

    return "";
}

void NewViewForm::onSaveSql()
{
    QString name = QFileDialog::getSaveFileName(this, tr("Save sql"), "", tr("SQL files (*.sql)"));
    QFile file(name);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return;
    }
    QTextStream out(&file);
    out << txtSql->toPlainText() << "\n";
}

void NewViewForm::onInject()
{
    InjectSqlDialog* injectDialog = new InjectSqlDialog(Workspace::getInstance()->currentProjectsEngine(), this, 0, m_view->getName());
    //injectDialog->selectCodePage(ui->cmbCharSetForSql->currentIndex());
    injectDialog->setModal(true);
    bool error = false;
    if(injectDialog->exec() == QDialog::Accepted)
    {
        QString tSql;
        QStringList connectionNames = injectDialog->getSelectedConnections();
        for(int i=0; i< connectionNames.size(); i++)
        {
            Connection* c = ConnectionManager::instance()->getConnection(connectionNames.at(i));
            if(c)
            {
                QString sql = txtSql->toPlainText();
                QStringList sqls; sqls << sql;
                if(!Workspace::getInstance()->currentProjectsEngine()->executeSql(c, sqls, QStringList(), tSql, injectDialog->getRollbackOnError()))
                {
                    QMessageBox::critical (this, tr("Error"), tr("<B>Cannot execute a query!</B><P>Reason: ") + Workspace::getInstance()->currentProjectsEngine()->getLastError() + tr(".<P>Query:<PRE>") + tSql+ "</PRE><P>" +
                                           (injectDialog->getRollbackOnError()?tr("Transaction was rolled back."):tr("Transaction was <font color=red><B>NOT</B></font> rolled back, you might have partial data in your database.")), QMessageBox::Ok);
                    error = true;
                }
            }
        }
        MainWindow::instance()->setStatus(QString("Creating view ") + ui->txtViewName->text() + (error?" Failed ":" Succeeded "), error);
    }
}

void NewViewForm::onLockUnlock(bool checked)
{
    if(checked)
    {
        ui->btnLock->setIcon(IconFactory::getUnLockedIcon());
        disableEditingControls(false);
//        ui->frame_3->setEnabled(true);
        m_view->unlock();
        m_view->updateGui();
        ui->btnLock->setToolTip(QObject::tr("This view is <b>unlocked</b>. Click this button to lock it."));

        MainWindow::instance()->finallyDoLockLikeOperation(false, m_view->getObjectUid().toString());
    }
    else
    {
        ui->btnLock->setIcon(IconFactory::getLockedIcon());
        disableEditingControls(true);
//        ui->frame_3->setEnabled(false);
        m_view->lock(LockableElement::LOCKED);
        m_view->updateGui();
        ui->btnLock->setToolTip(QObject::tr("This view is <b>locked</b>. Click this button to unlock it."));

        MainWindow::instance()->finallyDoLockLikeOperation(true, m_view->getObjectUid().toString());
    }

}


void NewViewForm::onUndelete()
{
    QString tempError;
    if(Version::DO_NOT_REMOVE_FROM_PATCH_TREE_FAILURE != m_version->undeleteObject(m_view->getObjectUid().toString(), false, tempError))
    {
        MainWindow::instance()->getGuiElements()->removeItemForPatch(m_version->getWorkingPatch(), m_view->getObjectUid().toString());
        // TODO: Duplicate from above
        if(m_view->lockState() == LockableElement::LOCKED)
        {
            ui->btnLock->setIcon(IconFactory::getLockedIcon());
            ui->btnLock->blockSignals(true);
            ui->btnLock->setChecked(false);
            ui->btnLock->blockSignals(false);
            ui->grpContent->setEnabled(false);
            ui->btnLock->setToolTip(QObject::tr("This view is <b>locked</b>. Click this button to unlock it."));
        }
        else
        {
            ui->btnLock->setIcon(IconFactory::getUnLockedIcon());
            ui->btnLock->blockSignals(true);
            ui->btnLock->setChecked(true);
            ui->btnLock->blockSignals(false);
            ui->grpContent->setEnabled(true);
            ui->btnLock->setToolTip(QObject::tr("This view is <b>unlocked</b>. Click this button to lock it."));
        }
        ui->btnUndelete->hide();
        ui->btnLock->show();
    }
    else
    {
        QMessageBox::critical(this, tr("Error"), tempError);
    }
}
