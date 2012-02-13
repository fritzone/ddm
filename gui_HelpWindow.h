#ifndef GUI_HELPWINDOW_H
#define GUI_HELPWINDOW_H

#include <QWidget>
#include <QSplitter>

namespace Ui {
    class HelpWindow;
}

class HelpWindow : public QWidget
{
    Q_OBJECT

public:
    static HelpWindow* instance();
    ~HelpWindow();
    void showHelp(const QString&);

public slots:

    void onDestroyed();
    void onBack();
    void onForward();

protected:
    void changeEvent(QEvent *e);
    void resizeEvent(QResizeEvent *);

private:
    explicit HelpWindow(QWidget *parent = 0);
    static HelpWindow* m_instance;
    Ui::HelpWindow *ui;
    QStringList m_links;
    int m_cindex;
    QSplitter* spl;
};

#endif // GUI_HELPWINDOW_H
