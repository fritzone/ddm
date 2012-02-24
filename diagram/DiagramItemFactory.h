#ifndef DIAGRAMITEMFACTORY_H
#define DIAGRAMITEMFACTORY_H

#include <QGraphicsItemGroup>

class DraggableGraphicsViewItem;
class DraggableGraphicsViewItemForForeignKey;
class Table;
class ForeignKey;

class DiagramItemFactory
{
public:
    DiagramItemFactory();

    static DraggableGraphicsViewItem* getDiagramEntityForTable(Table* t);
    static DraggableGraphicsViewItemForForeignKey* getDiagramEntityForForeignKey(ForeignKey*);
};

#endif // DIAGRAMITEMFACTORY_H
