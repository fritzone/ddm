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

class DeserializationFactory
{
public:

    static UserDataType* createUserDataType(const QDomDocument& doc, const QDomElement& element);
    static Index* createIndex(Table* table, const QDomDocument& doc, const QDomElement& element);
    static Table* createTable(DatabaseEngine* engine, MajorVersion* ver, const QDomDocument& doc, const QDomElement& element);
    static Column* createColumn(MajorVersion* ver, const QDomDocument& doc, const QDomElement& element);
    static MajorVersion* createMajorVersion(DatabaseEngine* engine, const QDomDocument& doc, const QDomElement& element);
    static Solution* createSolution(const QDomDocument& doc, const QDomElement& element);
    static Project* createProject(const QDomDocument& doc, const QDomElement& element);
    static ForeignKey* createForeignKey(Table* tab, const QDomDocument& doc, const QDomElement& element);

private:
    DeserializationFactory();
};

#endif // DESERIALIZATIONFACTORY_H
