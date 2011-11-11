#include "gui_HelpWindow.h"
#include "ui_HelpWindow.h"
#include "Workspace.h"
#include "Version.h"
#include "VersionGuiElements.h"
#include "MainWindow.h"

HelpWindow* HelpWindow::m_instance = 0;

HelpWindow* HelpWindow::instance()
{
    if(m_instance == 0)
    {
        m_instance = new HelpWindow(Workspace::getInstance()->workingVersion()->getGui()->getMainWindow());
    }
    return m_instance;
}

HelpWindow::HelpWindow(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::HelpWindow)
{
    ui->setupUi(this);
    setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    setFeatures(QDockWidget::AllDockWidgetFeatures);
    setFloating(false);
    setMinimumSize(300, 340);
    setMaximumSize(500, 9999);
    setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum));
    resize(400,600);
}

HelpWindow::~HelpWindow()
{
    delete ui;
}

void HelpWindow::changeEvent(QEvent *e)
{
    QDockWidget::changeEvent(e);
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
    ui->webView->setUrl(QApplication::applicationDirPath() + h);
}

void HelpWindow::onDestroyed()
{
    m_instance = 0;
}

void HelpWindow::onVisibilityChanged(bool v)
{
    if(v)
    {

    }
    else
    {

    }
}
