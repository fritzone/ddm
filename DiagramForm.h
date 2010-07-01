#ifndef DIAGRAMFORM_H
#define DIAGRAMFORM_H

#include <QWidget>
#include <QAbstractButton>

class ERGraphicsView;
class Version;
class TableListWidget;
class Diagram;
class MainWindow;
class DiagramTextItem;

namespace Ui
{
    class DiagramForm;
}

class DiagramForm : public QWidget
{
    Q_OBJECT
public:

    DiagramForm(Version* v, Diagram* dgram, QWidget *parent);
    ~DiagramForm();
    void setTableToRemoveFromDiagram(const QString& tabName);
    void doneNote();

public slots:

    void saveToImageFile();
    void onButtonBoxClicked(QAbstractButton*);
    void removeFromDiagram();
    void onAddNote();
    void editorLostFocus(DiagramTextItem *item);


protected:
    void changeEvent(QEvent *e);
    bool saveToFile(const QString& fileName, bool transparent, const char* mode);

private:
    void onSave();
    void onReset();

private:
    Ui::DiagramForm *ui;
    Version* ver;
    ERGraphicsView *graphicsView;
    TableListWidget *lstTables, *lstDiagramForms;
    Diagram* m_diagram;
    MainWindow* m_mw;
    QString m_tabToRemove;
};

#endif // DIAGRAMFORM_H