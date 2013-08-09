#include "core_Repository.h"
#include "core_Role.h"
#include "core_UserDataType.h"
#include "core_Entity.h"

#include "db_GenericDatabaseType.h"
#include "db_DatabaseFunctionCategory.h"
#include "db_DatabaseBuiltinFunction.h"
#include "db_DatabaseBuiltinFunctionsParameter.h"
#include "db_DatabaseEngineManager.h"
#include "db_DatabaseEngine.h"
#include "db_SP.h"
#include "TrueFalseSp.h"
#include "ValueSp.h"
#include "ValueListSp.h"

#include "uids.h"

#include <QApplication>
#include <QDomDocument>
#include <QFile>
#include <QDomElement>

#include <QDebug>

Repository::Repository()
{
    QDomDocument doc ("xml");
    QString filename = QApplication::applicationDirPath() + "/rsrc/repository.xml";

    QFile file (filename);
    if (file.open(QIODevice::ReadOnly))
    {
        if (!doc.setContent(&file))
        {
            file.close();
            qDebug() << "Cannot set XML content. Giving up the idea of repository";
            return;
        }
        file.close();
    }

    QDomElement docElem = doc.documentElement();

    // the initial run: gather all the roles of the system
    for(int i=0; i<docElem.childNodes().size(); i++)
    {
        QDomElement childI = docElem.childNodes().at(i).toElement();
        QString name = childI.nodeName();

        if(name == "roles")
        {
            for(int j=0; j<childI.childNodes().size(); j++)
            {
                QDomElement el = childI.childNodes().at(j).toElement();
                if(el.nodeName() == "role")
                {
                    Role* r = new Role(el.attribute("name"), el.attribute("class-uid"), el.attribute("description"));
                    m_roles.append(r);
                }
            }
        }
    }

    // first run: father the databases
    for(int i=0; i<docElem.childNodes().size(); i++)
    {
        QDomElement childI = docElem.childNodes().at(i).toElement();
        QString name = childI.nodeName();
        if(name == "databases")
        {
            for(int j=0; j<childI.childNodes().size(); j++)
            {
                addDatabase(childI.childNodes().at(j).toElement());
            }
        }
    }

    // second run to be sure that the roles have been fetched
    for(int i=0; i<docElem.childNodes().size(); i++)
    {
        QDomElement childI = docElem.childNodes().at(i).toElement();
        QString name = childI.nodeName();
        if(name == "entities-serialization")
        {
            for(int j=0; j<childI.childNodes().size(); j++)
            {
                QDomElement el = childI.childNodes().at(j).toElement();
                if(el.nodeName() == "entity")
                {
                    addEntity(el);
                }
            }
        }
    }
}

Repository::~Repository()
{
    // TODO: Save the repo
}

void Repository::addDatabase(const QDomElement & el)
{
    if(el.nodeName() == "database")
    {
        QString dbId = el.attribute("id");
        DatabaseEngine* dbe = DatabaseEngineManager::instance().engine(dbId);
        if(dbe)
        {
            m_databases.append(dbe);
        }

        QVector<GenericDatabaseType*> gdbts;
        QStringList keywords;
        QString delimiterKeyword = "";
        QString keywordReturn = "";
        QStringList triggerEvents;
        QStringList triggerTimes;
        QVector<Sp*> sps;
        QMap<QString, QString> spsTooltips;
        QMap<QString, QString> spsSqls;
        QVector<DatabaseFunctionCategory*> funcCategories;
        bool storedMethodsSupported = false;
        QSet<PROGRAMMING_LANGUAGES> storedMethodSupportedLanguage;
        QMap<PARAMETER_FIELD_ROLES, int> parameterFields;
        QMap<PROGRAMMING_LANGUAGES, QString> defaultBodies;

        for(int i=0; i<el.childNodes().size(); i++)
        {
            if(el.childNodes().at(i).nodeName() == "datatype-groups")
            {
                QDomElement elDtgroups = el.childNodes().at(i).toElement();
                for(int j=0; j<elDtgroups.childNodes().size(); j++)
                {
                    QDomElement elDtGroup = elDtgroups.childNodes().at(j).toElement(); // <datatype-group>
                    int type = elDtGroup.attribute("type").toInt();
                    for(int k=0; k<elDtGroup.childNodes().size(); k++)
                    {
                        QDomElement elDt = elDtGroup.childNodes().at(k).toElement(); // <datatype>
                        bool isDefault = elDt.hasAttribute("default") && elDt.attribute("default").toInt() == 1;
                        QString name = elDt.attribute("name");
                        int maxSize = elDt.hasAttribute("max-size")?
                                    elDt.attribute("max-size").toInt():
                                    0;

                        GenericDatabaseType* gdt = new GenericDatabaseType(name, UserDataType::toDtType(type), maxSize, isDefault);
                        gdbts.append(gdt);
                    }
                }
            }

            if(el.childNodes().at(i).nodeName() == "stored-methods")
            {
                storedMethodsSupported = el.childNodes().at(i).toElement().hasAttribute("support") &&
                        el.childNodes().at(i).toElement().attribute("support").toInt() == 1;

                if(storedMethodsSupported)
                {
                    QDomElement languagesEl = el.childNodes().at(i).firstChildElement("languages");
                    for(int j=0; j< languagesEl.childNodes().size(); j++)
                    {
                        QDomElement languageEl = languagesEl.childNodes().at(j).toElement();
                        int iLangId = languageEl.attribute("id").toInt();
                        PROGRAMMING_LANGUAGES langId = static_cast<PROGRAMMING_LANGUAGES>(iLangId);
                        storedMethodSupportedLanguage.insert(langId);
                    }

                    QDomElement fieldOrderEl = el.childNodes().at(i).firstChildElement("parameter_fields");
                    for(int j=0; j< fieldOrderEl.childNodes().size(); j++)
                    {
                        QDomElement fieldEl = fieldOrderEl.childNodes().at(j).toElement();
                        int iFieldId = fieldEl.attribute("id").toInt();
                        int index = fieldEl.attribute("index").toInt();
                        PARAMETER_FIELD_ROLES role = static_cast<PARAMETER_FIELD_ROLES>(iFieldId);
                        parameterFields[role] = index;
                    }

                    QDomElement defaultBodiesEl = el.childNodes().at(i).firstChildElement("default-bodies");
                    for(int j=0; j< defaultBodiesEl.childNodes().size(); j++)
                    {
                        QDomElement bodyEl = defaultBodiesEl.childNodes().at(j).toElement();
                        int iLangId = bodyEl.attribute("language-id").toInt();
                        PROGRAMMING_LANGUAGES langId = static_cast<PROGRAMMING_LANGUAGES>(iLangId);
                        QDomCDATASection cdata = bodyEl.firstChild().toCDATASection();
                        QString body = cdata.data();
                        defaultBodies[langId] = body;
                    }
                }
            }

            if(el.childNodes().at(i).nodeName() == "keywords")
            {
                for(int j=0; j<el.childNodes().at(i).childNodes().size(); j++)
                {
                    QDomElement keywordEl = el.childNodes().at(i).childNodes().at(j).toElement();
                    QString keyword = keywordEl.attribute("text");
                    keywords.append(keyword);

                    if(keywordEl.hasAttribute("role"))
                    {
                        QString role = keywordEl.attribute("role");
                        if(role.toUpper() == "DELIMITER")
                        {
                            delimiterKeyword = keyword;
                        }
                        if(role.toUpper() == "RETURNS")
                        {
                            keywordReturn = keyword;
                        }
                    }
                }
            }
            if(el.childNodes().at(i).nodeName() == "trigger-defs")
            {
                for(int j=0; j<el.childNodes().at(i).childNodes().size(); j++)
                {
                    QDomElement triggerEl = el.childNodes().at(i).childNodes().at(j).toElement();
                    if(triggerEl.hasAttribute("role"))
                    {
                        QString role = triggerEl.attribute("role");
                        QString value = triggerEl.attribute("value");
                        if(role.toUpper() == "EVENT")
                        {
                            triggerEvents.append(value);
                        }
                        else
                        {
                            triggerTimes.append(value);
                        }
                    }
                }
            }
            if(el.childNodes().at(i).nodeName() == "function-categories")
            {
                for(int j=0; j<el.childNodes().at(i).childNodes().size(); j++)
                {
                    QDomElement funcCatgEl = el.childNodes().at(i).childNodes().at(j).toElement();
                    QString sname = funcCatgEl.attribute("name");
                    QString sid = funcCatgEl.attribute("id");
                    int val = funcCatgEl.attribute("value").toInt();
                    bool isAggregate = funcCatgEl.hasAttribute("role") && funcCatgEl.attribute("role").toUpper() == "AGGREGATE"?true:false;
                    DatabaseFunctionCategory* dfc = new DatabaseFunctionCategory(sname, sid, val, isAggregate);
                    funcCategories.append(dfc);
                }
            }

            if(el.childNodes().at(i).nodeName() == "spis")
            {
                for(int j=0; j<el.childNodes().at(i).childNodes().size(); j++)
                {
                    QDomElement spEl = el.childNodes().at(i).childNodes().at(j).toElement();
                    QString spClassUid = spEl.attribute("class-uid");
                    QString spSqlRoleUid = spEl.attribute("sql-role-uid");
                    QString spReferredClassUid = spEl.attribute("referred-class-uid");
                    QString spName = spEl.attribute("name");
                    QString spGuiText = spEl.attribute("gui-text");
                    QString spGuiGroup = spEl.attribute("gui-group");
                    QString spDefault = spEl.attribute("default");
                    QString spDbMajor = spEl.attribute("db-major");
                    QString spDbMinor = spEl.attribute("db-minor");
                    QString spSql = spEl.attribute("sql");

                    spsSqls[spSqlRoleUid] = spSql;

                    QString spTooltip = "";

                    if(spEl.hasChildNodes())
                    {
                        QDomElement spTtNode = spEl.firstChildElement("tooltip");
                        QDomCDATASection cdata = spTtNode.firstChild().toCDATASection();
                        spTooltip = cdata.toText().data();
                        spsTooltips[spSqlRoleUid] = spTooltip;
                    }

                    Sp* sp = 0;

                    if(spClassUid == uidTrueFalseSp)
                    {
                        sp = new TrueFalseSp(spSqlRoleUid, spReferredClassUid,
                                             spName, spGuiText, spGuiGroup,
                                             spDefault.toUpper() == "TRUE" || spDefault == "1",
                                             spDbMajor.toInt(), spDbMinor.toInt(), 0);
                    }
                    else
                    if(spClassUid == uidValueSp)
                    {
                        sp = new ValueSp(spSqlRoleUid, spReferredClassUid,
                                         spName, spGuiText, spGuiGroup,
                                         spDefault, spDbMajor.toInt(), spDbMinor.toInt(), 0);
                    }
                    else
                    if(spClassUid == uidValueListSp)
                    {
                        sp = new ValueListSp(spSqlRoleUid, spReferredClassUid,
                                             spName, spGuiText, spGuiGroup,
                                             spDefault.split(","), 0,
                                             spDbMajor.toInt(), spDbMinor.toInt(), 0);
                    }

                    if(sp)
                    {
                        sps.append(sp);
                    }

                }
            }
        }

        // second run
        QVector<DatabaseBuiltinFunction> dbFunctions;
        // the functions need to be constructed after the other parts
        for(int i=0; i<el.childNodes().size(); i++)
        {
            if(el.childNodes().at(i).nodeName() == "functions")
            {
                for(int j=0; j<el.childNodes().at(i).childNodes().size(); j++)
                {
                    QString X = QString("X");

                    QDomElement funcEl = el.childNodes().at(i).childNodes().at(j).toElement();
                    QString name = funcEl.attribute("name");
                    QString category = funcEl.attribute("category");
                    QString retType = funcEl.attribute("return");
                    int category_value = funcEl.attribute("category-value").toInt();
                    QString desc = "";

                    // the description
                    QDomElement elDesc = funcEl.firstChildElement("description");
                    if(!elDesc.isNull())
                    {
                        desc = elDesc.firstChild().toCDATASection().data();
                    }

                    // building the parameters
                    QVector<DatabaseBuiltinFunctionsParameter> parameters;
                    QDomElement elParams = funcEl.firstChildElement("parameters");
                    if(!elParams.isNull())
                    {
                        int parCount = elParams.attribute("count").toInt();
                        if(parCount != elParams.childNodes().size())
                        {
                            qDebug() << "func par mismatch:" << name;
                        }
                        else
                        {
                            for(int k=0; k<elParams.childNodes().size(); k++)
                            {
                                QDomElement elParam = elParams.childNodes().at(k).toElement();
                                QString paramType = elParam.attribute("type");
                                if(paramType.toUpper().endsWith("PAR_NUMERIC"))
                                {
                                    if(paramType.toUpper().startsWith("O"))
                                    {
                                        parameters.append(DatabaseBuiltinFunctionsParameter(X, UserDataType(X, DT_NUMERIC, nullUid, 0), false));
                                    }
                                    else
                                    {
                                        parameters.append(DatabaseBuiltinFunctionsParameter(X, UserDataType(X, DT_NUMERIC, nullUid, 0), true));
                                    }
                                }
                                if(paramType.toUpper().endsWith("PAR_STRING"))
                                {
                                    if(paramType.toUpper().startsWith("O"))
                                    {
                                        parameters.append(DatabaseBuiltinFunctionsParameter(X, UserDataType(X, DT_STRING, nullUid, 0), false));
                                    }
                                    else
                                    {
                                        parameters.append(DatabaseBuiltinFunctionsParameter(X, UserDataType(X, DT_STRING, nullUid, 0), true));
                                    }
                                }
                                if(paramType.toUpper().endsWith("PAR_VARIABLE"))
                                {
                                    parameters.append(DatabaseBuiltinFunctionsParameter(X, UserDataType(X, DT_VARIABLE, nullUid, 0), true));
                                }

                            }
                        }
                    }

                    // fetching the function category
                    DatabaseFunctionCategory* dbfct = 0;
                    for(int k=0; k<funcCategories.size(); k++)
                    {
                        if(funcCategories[k]->getIdName().toUpper() == category.toUpper())
                        {
                            dbfct = funcCategories[k];
                            break;
                        }
                    }

                    if(dbfct == 0)
                    {
                        qDebug() << "Not found a function category for: " << category << "func name:" << name;
                        return;
                    }

                    if(category_value != dbfct->getId())
                    {
                        qDebug() << "Something wrong with func:" << name;
                        return;
                    }

                    // creating the return type
                    UserDataType* retTypeDataType = 0;
                    if(retType.toUpper() == "RET_NUMERIC")
                    {
                        retTypeDataType = new UserDataType("return", DT_NUMERIC, nullUid, 0);
                    }
                    if(retType.toUpper() == "RET_DATETIME")
                    {
                        retTypeDataType = new UserDataType("return", DT_DATETIME, nullUid, 0);
                    }
                    if(retType.toUpper() == "RET_STRING")
                    {
                        retTypeDataType = new UserDataType("return", DT_STRING, nullUid, 0);
                    }
                    if(retTypeDataType == 0)
                    {
                        qDebug() << "Not found a return type for:" << retType;
                        return;
                    }

                    // and creating the function itself
                    dbFunctions.append(DatabaseBuiltinFunction(name, *dbfct, *retTypeDataType, parameters, desc));

                }
            }
        }

        DatabaseEngineManager::instance().constructDtSupplier(dbId, gdbts);
        DatabaseEngineManager::instance().setKeywords(dbId, keywords);
        DatabaseEngineManager::instance().setDelimiterKeyword(dbId, delimiterKeyword);
        DatabaseEngineManager::instance().setTriggerEvents(dbId, triggerEvents);
        DatabaseEngineManager::instance().setTriggerTimes(dbId, triggerTimes);
        DatabaseEngineManager::instance().setFunctionCategories(dbId, funcCategories);
        if(!sps.empty())
        {
            DatabaseEngineManager::instance().setSps(dbId, sps, spsSqls, spsTooltips);
        }
        DatabaseEngineManager::instance().setFunctions(dbId, dbFunctions);
        DatabaseEngineManager::instance().setStoredMethodSupport(dbId, storedMethodsSupported);
        DatabaseEngineManager::instance().setStoredMethodSupportedLanguages(dbId, storedMethodSupportedLanguage);
        DatabaseEngineManager::instance().setParameterFields(dbId, parameterFields);
        DatabaseEngineManager::instance().setStoredMethodDefaultBodies(dbId, defaultBodies);
        DatabaseEngineManager::instance().setStoredMethodReturnKeyword(dbId, keywordReturn);
    }
}

void Repository::addEntity(const QDomElement &el)
{
    QString name = el.attribute("name");
    QUuid class_uid(el.attribute("class-uid"));
    QUuid target_class_uid(el.attribute("target-class-uid"));
    bool hasDesc = el.hasAttribute("hasDescription") && el.attribute("hasDescription").toInt() == 1;
    bool supportSps = el.hasAttribute("supportsSps") && el.attribute("supportsSps").toInt() == 1;

    qDebug() << "CUID:" << class_uid;
    Entity* ent = new Entity(name, class_uid, target_class_uid, hasDesc, supportSps);
    m_entities.append(ent);

    // and the attributes of the entity
    for(int i=0; i<el.childNodes().size(); i++)
    {
        QDomElement chI = el.childNodes().at(i).toElement();
        if(chI.nodeName() == "attribute")
        {
            Entity::Attribute *a = new Entity::Attribute;
            a->name = chI.hasAttribute("name")?chI.attribute("name"):"Unnamed";
            a->roleUid = chI.hasAttribute("role-class-uid")?chI.attribute("role-class-uid"):nullUid;
            a->builtin = chI.hasAttribute("builtin") && chI.attribute("builtin").toInt() == 1;
            ent->addAttribute(a);
        }
        if(chI.nodeName() == "collection")
        {
            Entity::Collection* c = new Entity::Collection;
            c->name = chI.attribute("name");
            c->roleUid = chI.attribute("role-class-uid");
            c->targetUid = chI.attribute("collection-entity-class-uid");
            c->builtin = chI.hasAttribute("builtin") && chI.attribute("builtin").toInt() == 1;
            ent->addCollection(c);
        }
        if(chI.nodeName() == "reference")
        {
            Entity::Reference *r = new Entity::Reference;
            r->name = chI.attribute("name");
            r->srcEntityUid = chI.attribute("reference-class-uid");
            r->srcRoleUid = chI.attribute("reference-role-uid");
            r->builtin = chI.hasAttribute("builtin") && chI.attribute("builtin").toInt() == 1;
            ent->addReference(r);
        }

    }
}

Repository* Repository::instance()
{
    static Repository repo;
    return &repo;
}
