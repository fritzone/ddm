#include "gui_HelpWindow.h"
#include "ui_HelpWindow.h"
#include "Workspace.h"
#include "Version.h"
#include "VersionGuiElements.h"
#include "MainWindow.h"
#include "strings.h"

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
    qDebug() << h;
    m_cindex ++;
    m_buttonNavigate = true;
    ui->webView->setUrl(QApplication::applicationDirPath() + h);
    m_links.erase(m_links.begin() + m_cindex, m_links.end());
    m_links.append(QApplication::applicationDirPath() + h);

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
}

void HelpWindow::treeItemChanged(QTreeWidgetItem* c, QTreeWidgetItem*)
{
    QVariant a = c->data(0, Qt::UserRole);
    QString l = a.toString();
    showHelp(QString("/doc/") + l);
}
