#ifndef DIAGRAMFORM_H
#define DIAGRAMFORM_H

#include <QWidget>
#include <QAbstractButton>

class ERGraphicsView;
class Version;
class TableListWidget;
class Diagram;
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
    void setCurrentWorkNoteOnDiagram(int noteIndex);
    void paintDiagram();
    void doneNote();

public slots:

    void saveToImageFile();
    void removeFromDiagram();
    void removeNoteFromDiagram();
    void onAddNote();
    void editorLostFocus(DiagramTextItem *item);
    void onEditNote();
    void printDiagram();
    void onHelp();
    void onNameChange(QString);
    void onZoomOut();
    void onZoomIn();
    void onLockUnlock(bool);

protected:
    void changeEvent(QEvent *e);
    bool saveToFile(const QString& fileName, bool transparent, const char* mode);

private:
    void onSave();
    void onReset();
    void prepareLists();

private:
    Ui::DiagramForm *ui;
    Version* m_version;
    ERGraphicsView *graphicsView;
    TableListWidget *lstTables, *lstDiagramForms;
    Diagram* m_diagram;
    QString m_tabToRemove;
    int m_noteToRemove;
};

#endif // DIAGRAMFORM_H
