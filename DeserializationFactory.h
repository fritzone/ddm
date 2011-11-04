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

class DeserializationFactory
{
public:

    static UserDataType* createUserDataType(const QDomDocument& doc, const QDomElement& element);
    static Index* createIndex(Table* table, const QDomDocument& doc, const QDomElement& element);
    static Table* createTable(DatabaseEngine* engine, Version* ver, const QDomDocument& doc, const QDomElement& element);
    static Column* createColumn(Version* ver, const QDomDocument& doc, const QDomElement& element);
    static MajorVersion* createMajorVersion(Project* p, DatabaseEngine* engine, const QDomDocument& doc, const QDomElement& element);
    static Solution* createSolution(const QDomDocument& doc, const QDomElement& element);
    static Project* createProject(const QDomDocument& doc, const QDomElement& element);
    static ForeignKey* createForeignKey(Table* tab, const QDomDocument& doc, const QDomElement& element);
    static DiagramTableDescriptor* createDiagramTableDescriptor(const QDomDocument& doc, const QDomElement& element);
    static DiagramNoteDescriptor* createDiagramNoteDescriptor(const QDomDocument& doc, const QDomElement& element);
    static DiagramFKDescriptor* createDiagramFKDescriptor(const QDomDocument& doc, const QDomElement& element);
    static Diagram* createDiagram(Version* v, const QDomDocument& doc, const QDomElement& element);
    static TableInstance* createTableInstance(Version* v, const QDomDocument& doc, const QDomElement& element, bool secondStep = false);
    static View* createView(Version* v, const QDomDocument& doc, const QDomElement& element);

private:
    DeserializationFactory();
};

#endif // DESERIALIZATIONFACTORY_H
