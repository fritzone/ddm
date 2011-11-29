#ifndef QLISTWIDGETFORCODECOMPLETION_H
#define QLISTWIDGETFORCODECOMPLETION_H

#include <QListWidget>
#include <QDebug>
#include <QKeyEvent>

class QListWidgetForCodeCompletion : public QListWidget
{
    Q_OBJECT
public:

    QListWidgetForCodeCompletion(QWidget* p) : QListWidget(p){}
    QListWidgetItem *itemFromIndex(const QModelIndex &index) const
    {
        return QListWidget::itemFromIndex(index);
    }

    void keyPressEvent(QKeyEvent *event);
    void selectNext();
};

#endif // QLISTWIDGETFORCODECOMPLETION_H
