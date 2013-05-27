#ifndef GUI_HELPWINDOW_H
#define GUI_HELPWINDOW_H

#include <QWidget>
#include <QSplitter>
#include <QUrl>
#include <QTreeWidgetItem>

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

protected slots:

    void onDestroyed();
    void onBack();
    void onForward();
    void onNavigate(QUrl);
    void treeItemChanged(QTreeWidgetItem*,QTreeWidgetItem*);
protected:
    void changeEvent(QEvent *e);
    void resizeEvent(QResizeEvent *);
    virtual void keyPressEvent(QKeyEvent *e);

private:
    explicit HelpWindow(QWidget *parent = 0);
    static HelpWindow* m_instance;
    Ui::HelpWindow *ui;
    static QStringList m_links;
    static int m_cindex;
    QSplitter* spl;
    static bool m_buttonNavigate;
};

#endif // GUI_HELPWINDOW_H
