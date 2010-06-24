#ifndef DIAGRAMFORM_H
#define DIAGRAMFORM_H

#include <QWidget>

class ERGraphicsView;
class Version;
class TableListWidget;

namespace Ui
{
    class DiagramForm;
}

class DiagramForm : public QWidget
{
    Q_OBJECT
public:

    DiagramForm(Version* v, QWidget *parent = 0);
    ~DiagramForm();


public slots:

    void saveToImageFile();

protected:
    void changeEvent(QEvent *e);
    bool saveToFile(const QString& fileName, bool transparent, const char* mode);

private:
    Ui::DiagramForm *ui;
    Version* ver;
    ERGraphicsView *graphicsView;
    TableListWidget *lstTables;

};

#endif // DIAGRAMFORM_H
