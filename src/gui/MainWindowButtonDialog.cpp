#include "MainWindowButtonDialog.h"
#include "ui_MainWindowButtonDialog.h"
#include "MainWindow.h"
#include "gui_HelpWindow.h"
#include "helper_MostRecentlyUsedFiles.h"
#include "Configuration.h"
#include <QDebug>
#include <QApplication>
#include <QDesktopWidget>
#include <QKeyEvent>

MainWindowButtonDialog::MainWindowButtonDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::MainWindowButtonDialog)
{
    m_ui->setupUi(this);
    QPalette thePalette = this->palette();
    QLinearGradient gradient(0, 0, 0, 50);
    gradient.setColorAt(0, QColor(227,177,27));
    gradient.setColorAt(0.25, QColor(170,177,167));

    gradient.setColorAt(1, Qt::white);
    QBrush brush(gradient);


    thePalette.setBrush(QPalette::Window, brush);

    setPalette(thePalette);

    fixButtons();
}

void MainWindowButtonDialog::fixButtons()
{
    m_ui->btnQuick1->hide();
    m_ui->btnQuick2->hide();
    m_ui->btnQuick3->hide();
    m_ui->btnQuick4->hide();
    m_ui->btnQuick5->hide();
    m_ui->grpRecent->hide();
    m_ui->lblRecent->hide();

    QVector<QString> mru = MostRecentlyUsedFiles::instance().getFiles();
    if(mru.size()>=1 && mru[0].length() > 0) {fixButton(m_ui->btnQuick1, mru[0]);}
    if(mru.size()>=2 && mru[1].length() > 0) {fixButton(m_ui->btnQuick2, mru[1]);}
    if(mru.size()>=3 && mru[2].length() > 0) {fixButton(m_ui->btnQuick3, mru[2]);}
    if(mru.size()>=4 && mru[3].length() > 0) {fixButton(m_ui->btnQuick4, mru[3]);}
    if(mru.size()>=5 && mru[4].length() > 0) {fixButton(m_ui->btnQuick5, mru[4]);}

}

void MainWindowButtonDialog::fixButton(QCommandLinkButton *b, const QString &s)
{
    QString s1 = s.mid(s.lastIndexOf('/') + 1);
    b->setText(s1);
    b->show();
    b->setToolTip(s);
    m_ui->grpRecent->show();
    m_ui->lblRecent->show();
}

MainWindowButtonDialog::~MainWindowButtonDialog()
{
    delete m_ui;
}

void MainWindowButtonDialog::onNewProject()
{
    MainWindow::instance()->onNewSolution();
}

void MainWindowButtonDialog::onOpenProject()
{
    MainWindow::instance()->onOpenSolution();
}

void MainWindowButtonDialog::onNewConnection()
{
    MainWindow::instance()->onNewConnection();
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
    hw->showHelp(QString("/doc/startup.html"));
    hw->show();
}

void MainWindowButtonDialog::onQuick1()
{
    QString v = m_ui->btnQuick1->toolTip();
    onQuick(v);
}

void MainWindowButtonDialog::onQuick2()
{
    QString v = m_ui->btnQuick2->toolTip();
    onQuick(v);
}

void MainWindowButtonDialog::onQuick3()
{
    QString v = m_ui->btnQuick3->toolTip();
    onQuick(v);
}

void MainWindowButtonDialog::onQuick4()
{
    QString v = m_ui->btnQuick4->toolTip();
    onQuick(v);
}

void MainWindowButtonDialog::onQuick5()
{
    QString v = m_ui->btnQuick5->toolTip();
    onQuick(v);

}

void MainWindowButtonDialog::onQuick(const QString &f)
{
    hide();
    MainWindow::instance()->doLoadSolution(f, true);
}

void MainWindowButtonDialog::onBtnClose()
{
    hide();
}

void MainWindowButtonDialog::showMe()
{
    setModal(false);
    setWindowFlags(
            #ifdef Q_WS_WIN
                Qt::SplashScreen |
            #endif
                Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);

    QDesktopWidget *d = QApplication::desktop();

    QRect t = d->availableGeometry(MainWindow::instance());
    move(mapToGlobal(this->geometry().topLeft()).x() + t.center().x() - width() / 2, t.center().y()- height() / 2);
    show();
    raise();
}

void MainWindowButtonDialog::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Escape)
    {
        close();
    }
}

void MainWindowButtonDialog::onDontShowThis(bool a) // true: do not show
{
    Configuration::instance().enableStartupDialog(!a);
}
