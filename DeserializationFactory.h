#ifndef DESERIALIZATIONFACTORY_H
#define DESERIALIZATIONFACTORY_H

#include <QDomDocument>
#include <QDomElement>

class UserDataType;
class Index;
class Table;
class Column;
class MajorVersion;

class DeserializationFactory
{
public:

    static UserDataType* createUserDataType(const QDomDocument& doc, const QDomElement& element);
    static Index* createIndex(Table* table, const QDomDocument& doc, const QDomElement& element);
    static Table* createTable(MajorVersion* ver, const QDomDocument& doc, const QDomElement& element);
    static Column* createColumn(MajorVersion* ver, const QDomDocument& doc, const QDomElement& element);
    static MajorVersion* createMajorVersion(const QDomDocument& doc, const QDomElement& element);

private:
    DeserializationFactory();
};

#endif // DESERIALIZATIONFACTORY_H
