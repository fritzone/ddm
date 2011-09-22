#include "qbr_CellTypeChooser.h"
#include "Workspace.h"
#include "Version.h"
#include "IconFactory.h"
#include "qbr_SingleExpressionQueryComponent.h"
#include "DatabaseBuiltinFunction.h"
#include "qbr_QueryComponents.h"
#include "Column.h"
#include "Table.h"
#include "qbr_SelectQueryGroupByComponent.h"
#include "qbr_SelectQuery.h"
#include "qbr_SelectQueryOrderByComponent.h"

#include <QPen>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QScrollBar>

CellTypeChooser::CellTypeChooser(int level, CellTypeChooserSize size, CellTypeChooserType defaultType, QSet<CellTypeChooserType> allowedTypes, QueryGraphicsHelper *c, QueryGraphicsItem* parent, QueryComponent* owner):
        QueryGraphicsItem(level, parent, c, owner),
        m_defaultType(defaultType), m_allowedTypes(allowedTypes), m_currentType(defaultType), m_rect(0), m_size (size), m_index(-1), m_functionSupport(0), m_text(0), m_literal()
{
}

CellTypeChooser::CellTypeChooser(int level, CellTypeChooserSize size, CellTypeChooserType defaultType, QSet<CellTypeChooserType> allowedTypes, QueryGraphicsHelper *c, QueryGraphicsItem* parent, QueryComponent* owner, int index):
        QueryGraphicsItem(level, parent, c, owner),
        m_defaultType(defaultType), m_allowedTypes(allowedTypes), m_currentType(defaultType), m_rect(0), m_size (size), m_index(index),m_functionSupport(0), m_text(0), m_literal()
{
}

CellTypeChooser::CellTypeChooser(int level, CellTypeChooserSize size, CellTypeChooserType defaultType, QueryGraphicsHelper* helper, QueryGraphicsItem* parent, QueryComponent* owner ):
        QueryGraphicsItem(level, parent, helper, owner),
        m_defaultType(defaultType), m_allowedTypes(QSet<CellTypeChooserType>()), m_currentType(defaultType), m_rect(0), m_size (size), m_index(-1),m_functionSupport(0), m_text(0), m_literal()
{
}

QGraphicsItemGroup* CellTypeChooser::render(int& x, int& y, int& w, int &h)
{
    int width = (m_size == CELLTYPECHOOSER_REGULAR)?(CELL_SIZE+1):(CELL_SIZE*2+1);
    int size = width;
    w = size;
    m_needsFrame = true;
    if(m_currentType == CELLTYPE_CURSOR)
    {
        w /= 2;
        width = w;
    }

    QGraphicsPixmapItem* typeIcon = 0;
    QFont theFont("Arial", 14, QFont::Bold);
    QMap<CellTypeChooserType,QIcon> iconMappings;

    iconMappings[CELLTYPE_NOT] = IconFactory::getBigNotIcon();
    iconMappings[CELLTYPE_XOR] = IconFactory::getXorIcon();
    iconMappings[CELLTYPE_LSHIFT] = IconFactory::getLeftShiftIcon();
    iconMappings[CELLTYPE_RSHIFT] = IconFactory::getRightShiftIcon();
    iconMappings[CELLTYPE_OR] = IconFactory::getBinaryOrIcon();
    iconMappings[CELLTYPE_AND] = IconFactory::getBinaryAndIcon();
    iconMappings[CELLTYPE_NEGATE] = IconFactory::getBigNegIcon();

    iconMappings[CELLTYPE_MINUS] = IconFactory::getBigMinusIcon();
    iconMappings[CELLTYPE_PLUS] = IconFactory::getPlusIcon();
    iconMappings[CELLTYPE_DIVIDE] = IconFactory::getDivideIcon();
    iconMappings[CELLTYPE_MULTIPLY] = IconFactory::getMultiplyIcon();
    iconMappings[CELLTYPE_MOD] = IconFactory::getModuloIcon();

    iconMappings[CELLTYPE_EQUAL] = IconFactory::getEqualIcon();
    iconMappings[CELLTYPE_NOTEQUAL] = IconFactory::getNotEqIcon();
    iconMappings[CELLTYPE_LESS] = IconFactory::getLessIcon();
    iconMappings[CELLTYPE_GREATER] = IconFactory::getGreaterIcon();
    iconMappings[CELLTYPE_LESS_OR_EQUAL] = IconFactory::getLessOrEqualIcon();
    iconMappings[CELLTYPE_GREATER_OR_EQUAL] = IconFactory::getGreaterOrEqualIcon();

    iconMappings[CELLTYPE_OPEN_PARANTHESES_FOR_FUNCTION_CALL] = IconFactory::getOpenParanthesesIcon();
    iconMappings[CELLTYPE_CLOSE_PARANTHESES_FOR_FUNCTION_CALL] = IconFactory::getCloseParanthesesIcon();

    QMap<CellTypeChooserType,QString> textMappings;
    textMappings[CELLTYPE_NOT_TEXT] = strNotText;
    textMappings[CELLTYPE_LIKE] = strLike;
    textMappings[CELLTYPE_DISTINCT] = strDistinct;
    textMappings[CELLTYPE_IN] = strIn;
    textMappings[CELLTYPE_IS] = strIs;
    textMappings[CELLTYPE_BETWEEN] = strBetween;
    textMappings[CELLTYPE_EXISTS] = strExists;
    textMappings[CELLTYPE_STAR] = strStar;
    textMappings[CELLTYPE_ROLLUP] = strRollup;
    textMappings[CELLTYPE_NULL] = strNull;
    textMappings[CELLTYPE_OPEN_PARANTHESES] = strOpenParantheses;
    textMappings[CELLTYPE_CLOSE_PARANTHESES] = strCloseParantheses;

    if(iconMappings.contains(m_currentType))
    {
        typeIcon = new QGraphicsPixmapItem(iconMappings[m_currentType].pixmap(size, size), this);
        m_needsFrame = false;
    }
    else
    if(textMappings.contains(m_currentType))
    {
        QString t = textMappings[m_currentType];
        if(t == strNotText)
        {
            t = "NOT";
        }
        m_text = new QGraphicsTextItem(t, this);
        if(m_currentType == CELLTYPE_STAR)
        {
            theFont.setBold(true);
            theFont.setPixelSize(28);
        }
        m_text->setFont(theFont);
        m_text->setX(x + CELL_SIZE + 1);
        m_text->setY(y - 8);
        addToGroup(m_text);
        width += m_text->boundingRect().width();
        w += m_text->boundingRect().width();
    }
    else
    switch(m_currentType)
    {
    case CELLTYPE_TABLE:
        if(Workspace::getInstance()->currentProjectIsOop())
        {
            typeIcon = new QGraphicsPixmapItem(IconFactory::getTabinstIcon().pixmap(size,size), this);
        }
        else
        {
            typeIcon = new QGraphicsPixmapItem(IconFactory::getTablesIcon().pixmap(size,size), this);
        }
        break;

    case CELLTYPE_FUNCTION:
        typeIcon = new QGraphicsPixmapItem(IconFactory::getFunctionIcon().pixmap(CELL_SIZE+1,CELL_SIZE+1), this);
        if(m_functionSupport)
        {
            m_text = new QGraphicsTextItem(m_functionSupport->getNiceName(), this);
            m_text->setFont(theFont);
            m_text->setX(x + CELL_SIZE + 1);
            m_text->setY(y - 4);
            addToGroup(m_text);
            width += m_text->boundingRect().width();
            w += m_text->boundingRect().width();
        }
        break;

    case CELLTYPE_COLUMN:
        typeIcon = new QGraphicsPixmapItem(IconFactory::getColumnIcon().pixmap(CELL_SIZE+1,CELL_SIZE+1), this);
        if(!m_column) break;
        m_text = new QGraphicsTextItem(m_column->getTable()->getName() + "." + m_column->getName());// TODO: Code below is duplication with code from above...
        m_text->setFont(theFont);
        m_text->setX(x + CELL_SIZE + 1);
        m_text->setY(y - 4);
        addToGroup(m_text);
        width += m_text->boundingRect().width();
        w += m_text->boundingRect().width();
        break;

    case CELLTYPE_LITERAL:
        {
        bool alias = false;
        if(m_literal.startsWith("~"))
        {
            m_literal = m_literal.mid(1);
            alias = true;
        }
        m_text = new QGraphicsTextItem(m_literal, this);
        m_text->setFont(theFont);
        m_text->setX(x + CELL_SIZE + 1);
        m_text->setY(y - 4);
        addToGroup(m_text);
        width += m_text->boundingRect().width();
        w += m_text->boundingRect().width();
        typeIcon = new QGraphicsPixmapItem(IconFactory::getLiteralIcon().pixmap(size,size), this);
        if(alias) typeIcon = new QGraphicsPixmapItem(IconFactory::getAliasIcon().pixmap(16,16), this);
        break;
        }
    case CELLTYPE_QUERY_OR:
        m_text = new QGraphicsTextItem("OR", this);
        theFont.setBold(true);
        m_text->setFont(theFont);
        m_text->setX(x + CELL_SIZE + 1);
        m_text->setY(y - 4);
        addToGroup(m_text);
        width += m_text->boundingRect().width();
        w += m_text->boundingRect().width();
        m_needsFrame = false;
        break;

    case CELLTYPE_QUERY_AND:
        m_text = new QGraphicsTextItem("AND", this);
        theFont.setBold(true);
        m_text->setFont(theFont);
        m_text->setX(x + CELL_SIZE + 1);
        m_text->setY(y - 4);
        addToGroup(m_text);
        width += m_text->boundingRect().width();
        w += m_text->boundingRect().width();
        m_needsFrame = false;
        break;

    default:
        break;
        // this is something wrong
    }

    if(typeIcon)
    {
        typeIcon->setX(x);
        typeIcon->setY(y);
    }

    // parantheses do not take any user input... unless the user wants them
    if(m_currentType == CELLTYPE_CLOSE_PARANTHESES_FOR_FUNCTION_CALL || m_currentType == CELLTYPE_OPEN_PARANTHESES_FOR_FUNCTION_CALL || m_currentType == CELLTYPE_QUERY_AND || m_currentType == CELLTYPE_QUERY_OR ) return this;

    QRect r(x, y, width-1, size-1);
    m_helper->addNewHotCell(this, r);

    addToGroup(m_rect = new QGraphicsRectItem(r));
    if(m_currentType == CELLTYPE_CURSOR)
    {
        QPen dashed(Qt::DashLine);
        m_rect->setPen(dashed);
    }
    if(!m_needsFrame)
    {
        QPen pen;
        pen.setColor(Qt::white);
        m_rect->setPen(pen);
    }
    return this;
}

void CellTypeChooser::setColumn(const Column *c)
{
    m_column = c;
}

void CellTypeChooser::updateWidth(int newWidth)
{
}

void CellTypeChooser::mouseMove(int x, int y)
{
    if(m_currentType == CELLTYPE_CLOSE_PARANTHESES_FOR_FUNCTION_CALL || m_currentType == CELLTYPE_OPEN_PARANTHESES_FOR_FUNCTION_CALL) return;
    if(!m_needsFrame) return;

    QPen thick;
    thick.setWidth(2);
    if(m_currentType == CELLTYPE_CURSOR)
    {
        thick.setStyle(Qt::DashLine);
    }
    m_rect->setPen(thick);

}

void CellTypeChooser::mouseLeft(int x, int y)
{
    if(!m_needsFrame) return;
    QPen normal;
    normal.setWidth(1);
    if(m_currentType == CELLTYPE_CURSOR)
    {
        normal.setStyle(Qt::DashLine);
    }
    m_rect->setPen(normal);

}

void CellTypeChooser::mousePress(int x, int y)
{
    QString selected = "";
    QueryGraphicsHelper::ListType listType = QueryGraphicsHelper::INPUT_SYMBOLS;
    SingleExpressionQueryComponent* seq = dynamic_cast<SingleExpressionQueryComponent*>(m_parent->getOwner());
    QVector<const Column*> columns;
    QStringList orderBy;

    if(seq)
    {
        SelectQueryGroupByComponent* gby = dynamic_cast<SelectQueryGroupByComponent*>(seq->getParent());
        if(gby)
        {
            listType = QueryGraphicsHelper::INPUT_COLUMNS;
            SelectQuery* sq = dynamic_cast<SelectQuery*>(gby->getParent());
            if(sq)
            {
                columns = sq->getSelectedColumns();
            }
        }

        SelectQueryOrderByComponent* oby = dynamic_cast<SelectQueryOrderByComponent*>(seq->getParent());
        if(oby)
        {
            listType = QueryGraphicsHelper::INPUT_ORDERBY;
            SelectQuery* sq = dynamic_cast<SelectQuery*>(oby->getParent());
            if(sq)
            {
                orderBy = sq->getOrderByElements();
            }
        }
    }
    m_helper->setColumns(columns);
    m_helper->setOrderBy(orderBy);

    QPoint a = scene()->views().at(0)->mapToGlobal((m_rect->mapToScene(m_rect->boundingRect().bottomLeft().toPoint())).toPoint() ) ;
    selected = m_helper->presentList(a.x() + 2-scene()->views().at(0)->horizontalScrollBar()->sliderPosition(), a.y() - scene()->views().at(0)->verticalScrollBar()->sliderPosition(), listType);

    if(selected.length() == 0)
    {
        m_helper->triggerReRender();
        return;
    }

    if(m_index == -1)
    {
        m_owner->handleAction(selected + (selected[0]=='@'?"":(m_index>=0?QString::number(m_index):"")), m_owner);
    }
    else
    {
        m_owner->handleAction(selected + strActionIndexSeparator + (m_index>=0?QString::number(m_index):""), m_owner);
    }
}
