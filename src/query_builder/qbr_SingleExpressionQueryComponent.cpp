#include "qbr_SingleExpressionQueryComponent.h"
#include "qbr_CellForSingleExpression.h"
#include "Workspace.h"
#include "db_DatabaseEngine.h"
#include "db_DatabaseBuiltinFunction.h"
#include "qbr_DatabaseFunctionInstantiationComponent.h"
#include "core_Table.h"
#include "core_Column.h"
#include "Version.h"
#include "Configuration.h"
#include "qbr_SelectQueryJoinComponent.h"
#include "qbr_SelectQuerySelectComponent.h"
#include "qbr_CellAsCommand.h"
#include "qbr_SelectQueryAsComponent.h"
#include "strings.h"
#include "uids.h"
#include "core_TableInstance.h"
#include "qbr_Query.h"

#include <QDebug>

SingleExpressionQueryComponent::SingleExpressionQueryComponent(Query* q,
                                                               QueryComponent* p,
                                                               int l,
                                                               Version *v) :
    QueryComponent(q, p, l, v),
    m_helper(0), m_gritm(0), m_elements(), m_columnsAtGivenPosition(),
    m_functionsAtGivenPosition(), m_functionInstantiationAtGivenPosition(),
    m_typedValuesAtGivenPosition(), m_forcedType(NOT_FORCED),
    m_ownedByOn(false), m_onOwner(0), m_onComponent(0), m_as(0),
    m_hasForcedType(false)
{
}

QueryGraphicsItem* SingleExpressionQueryComponent::createGraphicsItem(QueryGraphicsHelper * helper, QueryGraphicsItem * parent)
{
    m_helper = helper;
    int l = getLevel();
    int adder = 1;

    if(l == QueryComponent::PARAMETER_LEVEL)
    {
        adder = 0;
    }

    m_gritm = new CellForSingleExpression(l + adder, helper, parent, this);
    if(m_as)
    {
        m_gritm->setAs(new CellAsCommand(helper, nextLevel(), m_gritm, m_as, true));
    }

    return m_gritm;

}

QMap<CellTypeChooserType,QString> SingleExpressionQueryComponent::prepareMappings() const
{
    QMap<CellTypeChooserType,QString> textMappings;
    textMappings[CELLTYPE_NOT_TEXT] = strLogNot;
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
    textMappings[CELLTYPE_NOT] = strNotOp;
    textMappings[CELLTYPE_XOR] = strXorOp;
    textMappings[CELLTYPE_LSHIFT] = strLShiftOp;
    textMappings[CELLTYPE_RSHIFT] = strRShiftOp;
    textMappings[CELLTYPE_OR] = strOrOp;
    textMappings[CELLTYPE_AND] = strAndOp;
    textMappings[CELLTYPE_NEGATE] = strNegOp;
    textMappings[CELLTYPE_MINUS] = strMinusOp;
    textMappings[CELLTYPE_PLUS] = strPlusOp;
    textMappings[CELLTYPE_DIVIDE] = strDivOp;
    textMappings[CELLTYPE_MULTIPLY] = strMulOp;
    textMappings[CELLTYPE_MOD] = strModOp;
    textMappings[CELLTYPE_EQUAL] = strEqOp;
    textMappings[CELLTYPE_NOTEQUAL] = strNEqOp;
    textMappings[CELLTYPE_LESS] = strLessOp;
    textMappings[CELLTYPE_GREATER] = strGrOp;
    textMappings[CELLTYPE_LESS_OR_EQUAL] = strLEOp;
    textMappings[CELLTYPE_GREATER_OR_EQUAL] = strGEOp;
    textMappings[CELLTYPE_OPEN_PARANTHESES_FOR_FUNCTION_CALL] = strOpenParantheses;
    textMappings[CELLTYPE_CLOSE_PARANTHESES_FOR_FUNCTION_CALL] = strCloseParantheses;
    textMappings[CELLTYPE_ROLLUP] = strRollup;
    textMappings[CELLTYPE_QUERY_OR] = strLogOr;
    textMappings[CELLTYPE_QUERY_AND] = strLogAnd;

    return textMappings;

}

QString SingleExpressionQueryComponent::get() const
{
    QMap<CellTypeChooserType,QString> textMappings = prepareMappings();
    bool errorFound = false;

    QString result = "";
    for(int i=0; i<m_elements.size(); i++)
    {
        result +=getSpacesForLevel();
        if(textMappings.contains(m_elements.at(i)))
        {
            result += textMappings[m_elements.at(i)];
        }
        else
        {
            switch(m_elements.at(i))
            {
            case CELLTYPE_COLUMN:
            {
                QString t;
                if(!m_columnsAtGivenPosition[i]->tinst)
                {
                    t= m_columnsAtGivenPosition[i]->tab->getName();
                }
                else
                {
                    t= m_columnsAtGivenPosition[i]->tinst->getName();
                }
                t = getQuery()->applyAlias(t);
                result += t + "." + m_columnsAtGivenPosition[i]->c->getName();   // this goes into IssueOriginator! Beware!

                break;
            }
            case CELLTYPE_FUNCTION:
                result += m_functionInstantiationAtGivenPosition[i]->get();
                break;
            case CELLTYPE_LITERAL:
                {
                QString g = m_typedValuesAtGivenPosition[i].startsWith("~")?m_typedValuesAtGivenPosition[i].mid(1):m_typedValuesAtGivenPosition[i];
                g = g.trimmed();
                bool onlyNumber = true;
                bool appendQuote = true;
                for(int k=0; k<g.size(); k++)
                {
                    if(!g.at(k).isNumber())
                    {
                        onlyNumber = false;
                    }
                }
                if(!onlyNumber)
                {
                    if(g.at(0) == '"' && g.at(g.length() - 1) == '"') appendQuote = false;
                }
                if(onlyNumber) appendQuote = false;
                result += !appendQuote?g:("\"" + g + "\"");
                break;
                }
            default:
                void (0);
            }
        }
    }

    if(errorFound)
    {
        result += QString("(") + QString("?") + QString("?") + QString(")");
    }

    if(result.trimmed().length() == 0)
    {
        result = QString("(") + QString("?") + QString("?") + QString(")");
    }

    return result;
}

void SingleExpressionQueryComponent::serialize(QDomDocument& doc, QDomElement& parent) const
{
    QDomElement expressionElement = doc.createElement("Expression");
    expressionElement.setAttribute("elements", m_elements.size());
    expressionElement.setAttribute("forced", m_forcedType);
    if(m_as)
    {
        expressionElement.setAttribute("alias", m_as->getAs());
    }
    for(int i=0; i<m_elements.size(); i++)
    {
        QDomElement element = doc.createElement("Element");
        element.setAttribute("Type", m_elements.at(i));
        element.setAttribute("idx", i);
        if(m_elements.at(i) == CELLTYPE_COLUMN)
        {
            element.setAttribute("Table", m_columnsAtGivenPosition[i]->tab->getName());
            element.setAttribute("Column", m_columnsAtGivenPosition[i]->c->getName());
            element.setAttribute("TabInstance",m_columnsAtGivenPosition[i]->tinst?m_columnsAtGivenPosition[i]->tinst->getName():"");
        }
        if(m_elements.at(i) == CELLTYPE_LITERAL)
        {
            element.setAttribute("InputString", m_typedValuesAtGivenPosition[i]);
        }
        if(m_elements.at(i) == CELLTYPE_FUNCTION)
        {
             QDomElement functionElement = doc.createElement("FunctionInstantiation");
             m_functionInstantiationAtGivenPosition[i]->serialize(doc, functionElement);
             element.appendChild(functionElement);
        }
        expressionElement.appendChild(element);
    }
    QueryComponent::serialize(doc, expressionElement);
    parent.appendChild(expressionElement);
}


QSet<OptionsType> SingleExpressionQueryComponent::provideOptions()
{
    QSet<OptionsType> t;
    if(dynamic_cast<SelectQuerySelectComponent*>(getParent()))
    {
        t.insert(OPTIONS_AS);
    }
    return t;
}

QueryComponent* SingleExpressionQueryComponent::duplicate()
{
    SingleExpressionQueryComponent* newc = new SingleExpressionQueryComponent(getQuery(), getParent(), getLevel(), version());
    return newc;
}

CloneableElement* SingleExpressionQueryComponent::clone(Version *sourceVersion, Version *targetVersion)
{
    SingleExpressionQueryComponent* newc = new SingleExpressionQueryComponent(getQuery(), getParent(), getLevel(), targetVersion);
    newc->m_elements = m_elements;
    // the m_columnsAtGivenPosition has columns from sourceVersion but should be referring to the tables in the targetVersion
    newc->m_columnsAtGivenPosition = m_columnsAtGivenPosition;
    for(int i=0; i<m_columnsAtGivenPosition.keys().size(); i++)
    {
        Table* sourceTable = m_columnsAtGivenPosition[m_columnsAtGivenPosition.keys().at(i)]->tab;
        // find the table in the targetVersion which has sourceUid soruceTable.getObjectUid
        Table* descTable = targetVersion->getDescendantTable(sourceTable);
        TableInstance* descTinst = m_columnsAtGivenPosition[m_columnsAtGivenPosition.keys().at(i)]->tinst?
                    targetVersion->getDescendantTableInstance(m_columnsAtGivenPosition[m_columnsAtGivenPosition.keys().at(i)]->tinst):0;
        ColumnOfTabWithTabInstance* coft = new ColumnOfTabWithTabInstance;
        coft->c = descTable->getDescendantColumn(m_columnsAtGivenPosition[m_columnsAtGivenPosition.keys().at(i)]->c);
        coft->tab = descTable;
        coft->tinst = descTinst;
        newc->m_columnsAtGivenPosition[m_columnsAtGivenPosition.keys().at(i)] = coft;
    }

    newc->m_functionsAtGivenPosition = m_functionsAtGivenPosition;
    newc->m_functionInstantiationAtGivenPosition = m_functionInstantiationAtGivenPosition;
    newc->m_typedValuesAtGivenPosition = m_typedValuesAtGivenPosition;
    newc->m_forcedType = m_forcedType;
    newc->m_hasForcedType = m_hasForcedType;
    newc->m_as = m_as?dynamic_cast<SelectQueryAsComponent*>(m_as->clone(sourceVersion,targetVersion)):0;
    // the ownedByON is set on the graphics drawing
    {
        newc->m_ownedByOn = false;
        newc->m_onOwner = 0;
        newc->m_onComponent = 0;
    }
    cloneTheChildren(sourceVersion, targetVersion, newc);
    newc->setSourceUid(getObjectUid());
    return newc;
}

void SingleExpressionQueryComponent::shiftElementsToTheLeft(int after)
{
    for(int i=after+1; i<m_elements.size(); i++)
    {
        if(m_functionsAtGivenPosition.contains(i))
        {
            m_functionsAtGivenPosition.insert(i-1, m_functionsAtGivenPosition.value(i));
            m_functionsAtGivenPosition.remove(i);
        }

        if(m_functionInstantiationAtGivenPosition.contains(i))
        {
            m_functionInstantiationAtGivenPosition.insert(i-1, m_functionInstantiationAtGivenPosition.value(i));
            m_functionInstantiationAtGivenPosition.remove(i);
        }

        if(m_columnsAtGivenPosition.contains(i))
        {
            m_columnsAtGivenPosition.insert(i-1, m_columnsAtGivenPosition.value(i));
            m_columnsAtGivenPosition.remove(i);
        }

        if(m_typedValuesAtGivenPosition.contains(i))
        {
            m_typedValuesAtGivenPosition.insert(i-1, m_typedValuesAtGivenPosition.value(i));
            m_typedValuesAtGivenPosition.remove(i);
        }
    }
}

const DatabaseBuiltinFunction* SingleExpressionQueryComponent::getFunctionAt(int i)
{
    if(m_functionsAtGivenPosition.contains(i)) return m_functionsAtGivenPosition[i];
    return 0;
}

const ColumnOfTabWithTabInstance* SingleExpressionQueryComponent::getColumnAt(int i)
{
    if(m_columnsAtGivenPosition.contains(i)) return m_columnsAtGivenPosition[i];
    return 0;
}

const QString SingleExpressionQueryComponent::getTypedInValueAt(int i)
{
    if(m_typedValuesAtGivenPosition.contains(i)) return m_typedValuesAtGivenPosition[i];
    return "";
}

DatabaseFunctionInstantiationComponent* SingleExpressionQueryComponent::getFunctionInstantiationAt(int pos)
{
    if(m_functionInstantiationAtGivenPosition.contains(pos)) return m_functionInstantiationAtGivenPosition[pos];
    return 0;
}

void SingleExpressionQueryComponent::handleAction(const QString& action, QueryComponent* /*referringObject*/)
{
    QString localAction = action;

    QMap<QString,CellTypeChooserType> mappings;
    mappings[strMathMinus] = CELLTYPE_MINUS;
    mappings[strMathPlus] = CELLTYPE_PLUS;
    mappings[strMathDivide] = CELLTYPE_DIVIDE;
    mappings[strMathMultiply] = CELLTYPE_MULTIPLY;
    mappings[strMathMod] = CELLTYPE_MOD;

    mappings[strLogNeg] = CELLTYPE_NEGATE;
    mappings[strLogNot] = CELLTYPE_NOT;
    mappings[strLogOr] = CELLTYPE_OR;
    mappings[strLogAnd] = CELLTYPE_AND;
    mappings[strLogXor] = CELLTYPE_XOR;
    mappings[strLogLShift] = CELLTYPE_LSHIFT;
    mappings[strLogRShift] = CELLTYPE_RSHIFT;

    mappings[strCmpEqual] = CELLTYPE_EQUAL;
    mappings[strCmpNotEqual] = CELLTYPE_NOTEQUAL;
    mappings[strCmpLess] = CELLTYPE_LESS;
    mappings[strCmpGreater] = CELLTYPE_GREATER;
    mappings[strCmpLessOrEqual] = CELLTYPE_LESS_OR_EQUAL;
    mappings[strCmpGreaterOrEqual] = CELLTYPE_GREATER_OR_EQUAL;

    mappings[strNotText] = CELLTYPE_NOT_TEXT;
    mappings[strLike] = CELLTYPE_LIKE;
    mappings[strIn] = CELLTYPE_IN;
    mappings[strIs] = CELLTYPE_IS;
    mappings[strBetween] = CELLTYPE_BETWEEN;
    mappings[strExists] = CELLTYPE_EXISTS;
    mappings[strDistinct] = CELLTYPE_DISTINCT;
    mappings[strStar] = CELLTYPE_STAR;
    mappings[strRollup] = CELLTYPE_ROLLUP;
    mappings[strNull] = CELLTYPE_NULL;
    mappings[strOpenParantheses] = CELLTYPE_OPEN_PARANTHESES;
    mappings[strCloseParantheses] = CELLTYPE_CLOSE_PARANTHESES;

    if(localAction.startsWith("*") && localAction.length()>1 && localAction[1] != '^')  // this comes from the order by ... let's just re-map to something more familiar
    {
        QString starLess = localAction.mid(1);
        if(starLess.startsWith("#"))    // this will be transformed to a typed in value
        {
            QString typedIn = starLess.mid(1);
            localAction = QString("$") + typedIn;
        }
        else
        if(starLess.startsWith("$"))    // this is a column... will be mapped to a ... column
        {
            QString chosen = starLess.mid(1);
            QStringList t = chosen.split('.');
            localAction = QString("#") + t.at(0) + "+" + t.at(1);
        }
        else
        if(starLess.startsWith("~"))    // this is a plain alias. Will be mapped to a typed in value, with the leading ~ which will be mapped out at a later stage in CellTypeChooser
        {
            localAction = QString("$") + starLess;
        }
    }

    if(localAction.indexOf(strActionIndexSeparator)&& !localAction.startsWith(strRemove)&&!localAction.startsWith("@")&&!localAction.startsWith("#")&&!localAction.startsWith("$"))
    {
        QString pureAction = localAction.left(localAction.indexOf(strActionIndexSeparator));
        if(mappings.contains(pureAction))
        {
            // let's check if this is ROLLUP. There can be only one
            if(mappings[pureAction] == CELLTYPE_ROLLUP)
            {
                for(int i=0; i<m_elements.size(); i++)
                {
                    if(m_elements.at(i) == CELLTYPE_ROLLUP) // only one ROLLUP enabled
                    {
                        return;
                    }
                }
            }
            int indx = localAction.section(strActionIndexSeparator, 1).toInt();
            if(indx < m_elements.size())
            {
                m_elements[indx] = mappings[pureAction];
                if(m_functionsAtGivenPosition.find(indx) != m_functionsAtGivenPosition.end())   // check if we overwrote a function
                {
                    m_functionInstantiationAtGivenPosition.remove(indx);
                    m_functionsAtGivenPosition.remove(indx);
                }
                if(m_columnsAtGivenPosition.find(indx) != m_columnsAtGivenPosition.end())   // check if we overwrote a column
                {
                    m_columnsAtGivenPosition.remove(indx);
                }

                if(m_typedValuesAtGivenPosition.contains(indx)) // check if we overwrote a typed in value
                {
                    m_typedValuesAtGivenPosition.remove(indx);
                }
            }
            else
            {
                m_elements.append(mappings[pureAction]);
            }
        }
        else
        if(localAction == ADD_ALIAS)
        {
            m_as = new SelectQueryAsComponent(getQuery(), this, nextLevel(), version());
            addChild(m_as);
            m_helper->triggerReRender();
        }
        m_helper->triggerReRender();
        return;
    }

    if(localAction.startsWith(strRemove) && !m_elements.isEmpty())
    {
        int idx = localAction.right(localAction.length() - 7).toInt(); // because actually REMOVE^xxx comes in
        m_elements.remove(idx);
        shiftElementsToTheLeft(idx);
        m_helper->triggerReRender();
        return;
    }

    if(localAction.startsWith("@"))          // this is a function...
    {
        QString fName = localAction.mid(1, localAction.indexOf(strActionIndexSeparator) - 1);
        int index = localAction.section(strActionIndexSeparator, 1).toInt();
        if(index < m_elements.size())   // let's see if we overwrote something
        {
            m_elements[index] = CELLTYPE_FUNCTION;
        }
        else
        {
            m_elements.append(CELLTYPE_FUNCTION);
        }

        if(m_functionsAtGivenPosition.find(index) == m_functionsAtGivenPosition.end())
        {
            m_functionsAtGivenPosition.insert(index, &Workspace::getInstance()->currentProjectsEngine()->getBuiltinFunction(fName));
            m_functionInstantiationAtGivenPosition.insert(index,
                                                          new DatabaseFunctionInstantiationComponent(
                                                              getQuery(),
                                                              new SingleExpressionQueryComponent(
                                                                  getQuery(), this, nextLevel(), version()
                                                                  ),
                                                              Workspace::getInstance()->currentProjectsEngine()->getBuiltinFunction(fName),
                                                              false,
                                                              version())
                                                          );
        }
        else
        {
            m_functionsAtGivenPosition[index] = &Workspace::getInstance()->currentProjectsEngine()->getBuiltinFunction(fName);
            m_functionInstantiationAtGivenPosition[index] = new DatabaseFunctionInstantiationComponent(
                        getQuery(),
                        new SingleExpressionQueryComponent(getQuery(), this, getLevel() + 1, version()),
                        Workspace::getInstance()->currentProjectsEngine()->getBuiltinFunction(fName),
                        false,
                        version()
                        );
        }

        // let's see if we overwrote a column
        if(m_columnsAtGivenPosition.contains(index))
        {
            m_columnsAtGivenPosition.remove(index);
        }

        // or a typed in value
        if(m_typedValuesAtGivenPosition.contains(index))
        {
            m_typedValuesAtGivenPosition.remove(index);
        }
        m_helper->triggerReRender();
    }

    if(localAction.startsWith("#"))  // This is a table->column at given position: "#PERSON+NAME^0"
    {
        QStringList t = localAction.split('+');
        QString t1 = t.at(0);
        QString tabName = t1.right(t1.length() - 1);
        t1 = t.at(1);
        QStringList t2 = t1.split('^');
        QString colName = t2.at(0);
        int index = t2.at(1).toInt();
        Table* tab = Workspace::getInstance()->workingVersion()->getTable(tabName);
        TableInstance* tinst = 0;
        if(tab == 0)
        {
            tinst = Workspace::getInstance()->workingVersion()->getTableInstance(tabName);
            tab = tinst->table();
            if(tab == 0) return;
        }
        Column* c = tab->getColumn(colName);
        if(c == 0) c = tab->getColumnFromParents(colName);
        if(c == 0)
        {
            return;
        }
        ColumnOfTabWithTabInstance * coft = new ColumnOfTabWithTabInstance;
        coft->c = c;
        coft->tab = tab;
        coft->tinst = tinst;
        m_columnsAtGivenPosition[index] = coft;

        if(index < m_elements.size())   // let's see if we overwrote something
        {
            m_elements[index] = CELLTYPE_COLUMN;
        }
        else
        {
            m_elements.append(CELLTYPE_COLUMN);
        }

        if(m_functionsAtGivenPosition.find(index) != m_functionsAtGivenPosition.end())  // we had a function at this position
        {
            m_functionsAtGivenPosition.remove(index);
            m_functionInstantiationAtGivenPosition.remove(index);
        }
        // or a typed in value
        if(m_typedValuesAtGivenPosition.contains(index))
        {
            m_typedValuesAtGivenPosition.remove(index);
        }
        m_helper->triggerReRender();
    }

    if(localAction.startsWith("$"))  // this is a typed in value
    {
        int t = localAction.lastIndexOf(strActionIndexSeparator);
        QString typedText = localAction.mid(1, t-1);
        QString sIdx = localAction.right(localAction.length() - t - 1);
        int index = sIdx.toInt();
        m_typedValuesAtGivenPosition[index] = typedText;
        if(index < m_elements.size())   // let's see if we overwrote something
        {
            m_elements[index] = CELLTYPE_LITERAL;
        }
        else
        {
            m_elements.append(CELLTYPE_LITERAL);
        }
        if(m_functionsAtGivenPosition.find(index) != m_functionsAtGivenPosition.end())  // we had a function at this position
        {
            m_functionsAtGivenPosition.remove(index);
            m_functionInstantiationAtGivenPosition.remove(index);
        }
        // let's see if we overwrote a column
        if(m_columnsAtGivenPosition.contains(index))
        {
            m_columnsAtGivenPosition.remove(index);
        }
        m_helper->triggerReRender();
    }
}

bool SingleExpressionQueryComponent::hasFunctionAtIndex(int i)
{
    return m_functionsAtGivenPosition.find(i) != m_functionsAtGivenPosition.end();
}

QVector<CellTypeChooserType> SingleExpressionQueryComponent::getChoosableTypes() const
{
    return QVector<CellTypeChooserType> ();
}

void SingleExpressionQueryComponent::setForcedType(ForcedSingleExpressionQueryComponent t, bool fromDeserialization)
{
    m_forcedType = t;
    m_hasForcedType = true;
    if(fromDeserialization)
    {
        return;
    }
    if(m_forcedType == FORCED_OR)
    {
        m_elements.append(CELLTYPE_QUERY_OR);
    }
    if(m_forcedType == FORCED_AND)
    {
        m_elements.append(CELLTYPE_QUERY_AND);
    }

}

void SingleExpressionQueryComponent::removeFromOn()
{
    m_onOwner->removeExpression(m_onComponent);
}

void SingleExpressionQueryComponent::onClose()
{
    getParent()->removeChild(this);
    if(m_ownedByOn)
    {
        removeFromOn();
    }
    m_helper->triggerReRender();
}

bool SingleExpressionQueryComponent::hasGroupByFunctions()
{
    QMap<int, const DatabaseBuiltinFunction*>::const_iterator it = m_functionsAtGivenPosition.constBegin();
    while (it != m_functionsAtGivenPosition.constEnd())
    {
        if(it.value()->getType().isAggregate())
        {
            return true;
        }
        ++it;
    }
    return false;
}

bool SingleExpressionQueryComponent::hasAtLeastOneColumnSelected()
{
    for(int i=0; i<m_elements.size(); i++)
    {
        if(m_elements.at(i) == CELLTYPE_COLUMN) return true;
    }
    return false;
}

QVector<const ColumnOfTabWithTabInstance*> SingleExpressionQueryComponent::getColumns()
{
    QVector<const ColumnOfTabWithTabInstance*> result;
    QMap<int, const ColumnOfTabWithTabInstance*>::const_iterator it = m_columnsAtGivenPosition.begin();
    while(it != m_columnsAtGivenPosition.end())
    {
        result.push_back(it.value());
        ++ it;
    }
    return result;
}

void SingleExpressionQueryComponent::removeAs()
{
    m_as = 0;
    m_helper->triggerReRender();
}

const SelectQueryAsComponent* SingleExpressionQueryComponent::hasAs()
{
    return m_as;
}

bool SingleExpressionQueryComponent::hasStar()
{
    for(int i=0; i<m_elements.size(); i++)
    {
        if(m_elements.at(i) == CELLTYPE_STAR) return true;
    }
    return false;
}

QUuid SingleExpressionQueryComponent::getClassUid() const
{
    return QUuid(uidSingleExpressionComponent);
}
