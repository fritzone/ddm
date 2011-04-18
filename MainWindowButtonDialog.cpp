#include "MainWindowButtonDialog.h"
#include "ui_MainWindowButtonDialog.h"
#include "MainWindow.h"

MainWindowButtonDialog::MainWindowButtonDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::MainWindowButtonDialog),
    m_mw(0)
{
    m_ui->setupUi(this);
    QString g = QApplication::applicationDirPath() + "/doc/main.html";
    m_ui->webView->load(QUrl(g));
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