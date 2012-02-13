#include "gui_HelpWindow.h"
#include "ui_HelpWindow.h"
#include "Workspace.h"
#include "Version.h"
#include "VersionGuiElements.h"
#include "MainWindow.h"

#include <QDesktopWidget>
#include <QFile>

HelpWindow* HelpWindow::m_instance = 0;

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
    ui(new Ui::HelpWindow), m_links(), m_cindex(-1)
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
        QString html = l.mid(l.indexOf(";"));
        QTreeWidgetItem* itm = new QTreeWidgetItem(QStringList(title));
        ui->treeWidget->addTopLevelItem(itm);
        itm->setData(0, Qt::UserRole, QVariant(html));
        itm->setIcon(0, IconFactory::getIndexIcon());
    }
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
        ui->webView->setUrl(m_links.at(m_cindex));
    }
}

void HelpWindow::onForward()
{
    if(m_cindex < m_links.size() - 1)
    {
        ui->webView->setUrl(m_links.at(m_cindex));
        m_cindex ++;
    }

}

void HelpWindow::resizeEvent(QResizeEvent *e)
{
    spl->resize(e->size());
}
