#include "MainWindowButtonDialog.h"
#include "ui_MainWindowButtonDialog.h"
#include "MainWindow.h"
#include "gui_HelpWindow.h"

MainWindowButtonDialog::MainWindowButtonDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::MainWindowButtonDialog),
    m_mw(0)
{
    m_ui->setupUi(this);
    QPalette thePalette = this->palette();
    QLinearGradient gradient(0, 0, 0, 50);
    gradient.setColorAt(0, Qt::darkGreen);
    gradient.setColorAt(1, Qt::white);
    QBrush brush(gradient);
    thePalette.setBrush(QPalette::Window, brush);
    setPalette(thePalette);
}

MainWindowButtonDialog::~MainWindowButtonDialog()
{
    delete m_ui;
}

void MainWindowButtonDialog::setMainWindow(MainWindow* mw)
{
    m_mw = mw;
}

void MainWindowButtonDialog::onNewProject()
{
    if(m_mw)
    {
        m_mw->onNewSolution();
    }
}

void MainWindowButtonDialog::onOpenProject()
{
    if(m_mw)
    {
        m_mw->onOpenProject();
    }
}

void MainWindowButtonDialog::onNewConnection()
{
    if(m_mw)
    {
        m_mw->onNewConnection();
    }
}


void MainWindowButtonDialog::onCloseWindow()
{

}

void MainWindowButtonDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindowButtonDialog::onHelp()
{
    HelpWindow* hw = HelpWindow::instance();
    hw->showHelp(QString("/doc/main.html"));
    hw->show();
}
