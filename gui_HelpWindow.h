#ifndef GUI_HELPWINDOW_H
#define GUI_HELPWINDOW_H

#include <QDockWidget>

namespace Ui {
    class HelpWindow;
}

class HelpWindow : public QDockWidget
{
    Q_OBJECT

public:
    static HelpWindow* instance();
    ~HelpWindow();
    void showHelp(const QString&);

public slots:

    void onDestroyed();
    void onVisibilityChanged(bool);

protected:
    void changeEvent(QEvent *e);

private:
    explicit HelpWindow(QWidget *parent = 0);
    static HelpWindow* m_instance;
    Ui::HelpWindow *ui;
};

#endif // GUI_HELPWINDOW_H
