#ifndef DESERIALIZATIONFACTORY_H
#define DESERIALIZATIONFACTORY_H

#include <QDomDocument>
#include <QDomElement>

class UserDataType;
class Index;
class Table;
class Column;
class MajorVersion;
class Solution;
class Project;
class ForeignKey;
class DatabaseEngine;
class DiagramTableDescriptor;
class DiagramNoteDescriptor;
class DiagramFKDescriptor;
class Diagram;
class Version;
class TableInstance;
class View;
class QueryComponent;
class Procedure;
class Trigger;
class Function;
class SpInstance;
class ObjectWithSpInstances;
class Patch;
class ObjectWithUid;
class Query;

#include <QObject>

class DeserializationFactory
{

public:

    static UserDataType* createUserDataType(Version *v, const QDomDocument& doc, const QDomElement& element);
    static Index* createIndex(DatabaseEngine* engine, Table* table, const QDomDocument& doc, const QDomElement& element, Version *v);
    static Table* createTable(DatabaseEngine* engine, Version* ver, const QDomDocument& doc, const QDomElement& element);
    static Column* createColumn(DatabaseEngine* engine, Version* ver, const QDomDocument& doc, const QDomElement& element);
    static void createMajorVersion(MajorVersion* mv, Project* p, DatabaseEngine* engine, const QDomDocument& doc, const QDomElement& element);
    static void createSolution(Solution* s, const QDomDocument& doc, const QDomElement& element);
    static void createProject(Project* project, const QDomDocument& doc, const QDomElement& element);
    static ForeignKey* createForeignKey(Table* tab, const QDomDocument& doc, const QDomElement& element);
    static DiagramTableDescriptor* createDiagramTableDescriptor(const QDomDocument& doc, const QDomElement& element, Version *v);
    static DiagramNoteDescriptor* createDiagramNoteDescriptor(const QDomDocument& doc, const QDomElement& element, Version *v);
    static DiagramFKDescriptor* createDiagramFKDescriptor(const QDomDocument& doc, const QDomElement& element, Version *v);
    static Diagram* createDiagram(Version* v, const QDomDocument& doc, const QDomElement& element);
    static TableInstance* createTableInstance(Version* v, const QDomDocument& doc, const QDomElement& element, bool secondStep = false);
    static View* createView(Version* v, DatabaseEngine *engine, const QDomDocument& doc, const QDomElement& element);
    static QueryComponent* createComponent(Query* query, QueryComponent* parent, Project* p, Version* v, const QDomDocument& doc, const QDomElement& element);
    static Procedure* createProcedure(Version* v, const QDomDocument& doc, const QDomElement& element);
    static Trigger* createTrigger(Version* v, const QDomDocument& doc, const QDomElement& element);
    static Function* createFunction(Version*,  const QDomDocument&, const QDomElement& element);
    static Patch* createPatch(Project*, Version*,  const QDomDocument&, const QDomElement& element);
    static SpInstance* createSpInstance(DatabaseEngine* engine, const QString& sql_role_uid, const QString& spi_uid);
    static void createObjectWithSpInstances(DatabaseEngine* engine, ObjectWithSpInstances* obj, const QDomDocument &doc, const QDomElement &element);

    static ObjectWithUid* createElementForClassUid(const QString& classUid, const QString& serialized, Version* v);


private:
    DeserializationFactory();
};

#endif // DESERIALIZATIONFACTORY_H
