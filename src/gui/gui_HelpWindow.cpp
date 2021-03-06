#include "gui_HelpWindow.h"
#include "ui_HelpWindow.h"
#include "Workspace.h"
#include "Version.h"
#include "VersionGuiElements.h"
#include "MainWindow.h"
#include "ddm_strings.h"
#include "NewTableForm.h"
#include "NewViewForm.h"
#include "TableInstanceForm.h"
#include "ProcedureForm.h"
#include "NewDataTypeForm.h"
#include "TriggerForm.h"
#include "DiagramForm.h"
#include <QDesktopWidget>
#include <QFile>

HelpWindow* HelpWindow::m_instance = 0;
QStringList HelpWindow::m_links;
int HelpWindow::m_cindex = -1;
bool HelpWindow::m_buttonNavigate = false;

HelpWindow* HelpWindow::instance()
{
    if(m_instance == 0)
    {
        QDesktopWidget* w = new QDesktopWidget();
        m_instance = new HelpWindow(w->screen());
    }
    return m_instance;
}

HelpWindow::HelpWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HelpWindow)
{
    ui->setupUi(this);
    resize(600,600);

    QFile f(strHelpConfig);
    if(!f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    spl = new QSplitter(Qt::Horizontal, this);
    spl->addWidget(ui->frame_3);
    spl->addWidget(ui->frame_2);

    spl->resize( size() );

    while(!f.atEnd())
    {
        QString l = f.readLine();
        l = l.trimmed();
        QString title = l.left(l.indexOf(";"));
        QString html = l.mid(l.indexOf(";") + 1);
        QTreeWidgetItem* itm = new QTreeWidgetItem(QStringList(title));
        ui->treeWidget->addTopLevelItem(itm);
        itm->setData(0, Qt::UserRole, QVariant(html));
        itm->setIcon(0, IconFactory::getHelpIcon());
    }

    setWindowIcon(IconFactory::getHelpIcon());
}

HelpWindow::~HelpWindow()
{
    delete ui;
}

void HelpWindow::changeEvent(QEvent *e)
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

void HelpWindow::showHelp(const QString &h)
{
    m_cindex ++;
    m_buttonNavigate = true;
    ui->webView->setUrl(QApplication::applicationDirPath() + h);

    m_links.erase(m_links.begin() + m_cindex, m_links.end());
    m_links.append(QApplication::applicationDirPath() + h);

    for(int i=0; i<ui->treeWidget->topLevelItemCount(); i++)
    {
        if(h.endsWith(ui->treeWidget->topLevelItem(i)->data(0, Qt::UserRole).toString()))
        {
            ui->treeWidget->topLevelItem(i)->setSelected(true);
        }
        else
        {
            ui->treeWidget->topLevelItem(i)->setSelected(false);
        }
    }
}

void HelpWindow::onDestroyed()
{
    m_instance = 0;
}

void HelpWindow::onBack()
{
    if(m_cindex > 0)
    {
        m_cindex --;
        m_buttonNavigate = true;
        ui->webView->setUrl(m_links.at(m_cindex));
    }
}

void HelpWindow::onForward()
{
    if(m_cindex < m_links.size() - 1)
    {
        m_buttonNavigate = true;
        ui->webView->setUrl(m_links.at(++m_cindex));
    }
}

void HelpWindow::resizeEvent(QResizeEvent *e)
{
    spl->resize(e->size());
}

void HelpWindow::onNavigate(QUrl a)
{
    if(m_buttonNavigate)
    {
        m_buttonNavigate = false;
        return;
    }

    if(m_cindex >= 0 && ((m_links.begin() + m_cindex+1) < m_links.end()))
    {
        m_links.erase(m_links.begin() + m_cindex+1, m_links.end());
    }

    m_cindex ++;
    m_links.append(a.toString());
    m_buttonNavigate = false;

    for(int i=0; i<ui->treeWidget->topLevelItemCount(); i++)
    {
        if(a.toString().endsWith(ui->treeWidget->topLevelItem(i)->data(0, Qt::UserRole).toString()))
        {
            ui->treeWidget->topLevelItem(i)->setSelected(true);
        }
        else
        {
            ui->treeWidget->topLevelItem(i)->setSelected(false);
        }
    }
}

void HelpWindow::treeItemChanged(QTreeWidgetItem* c, QTreeWidgetItem*)
{
    QVariant a = c->data(0, Qt::UserRole);
    QString l = a.toString();
    showHelp(QString("/doc/") + l);
}

void HelpWindow::showHelpForWidget(QWidget *w)
{
    QString s = QString("/doc/main.html");
    if(dynamic_cast<NewTableForm*>(w))
    {
        s = QString("/doc/tabl.html");
    }
    if(dynamic_cast<TableInstanceForm*>(w))
    {
        s = QString("/doc/tinst.html");
    }
    if(dynamic_cast<NewViewForm*>(w))
    {
        s = QString("/doc/view.html");
    }
    if(dynamic_cast<NewDataTypeForm*>(w))
    {
        s = QString("/doc/dtyp.html");
    }
    if(dynamic_cast<TriggerForm*>(w))
    {
        s = QString("/doc/trig.html");
    }
    if(dynamic_cast<ProcedureForm*>(w))
    {
        s = QString("/doc/proc.html");
    }
    if(dynamic_cast<DiagramForm*>(w))
    {
        s = QString("/doc/dgram.html");
    }
    showHelp(s);
    show();
}

void HelpWindow::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Escape)
    {
        hide();
    }
}
