#ifndef CORE_FUNCTION_H
#define CORE_FUNCTION_H

#include "core_StoredMethod.h"
#include "core_ObjectWithUid.h"

class Function : public StoredMethod
{
public:
    explicit Function(){} // dummy

    Function(const QString& pname, const QString& guid, Version *v, bool guided);
    virtual void serialize(QDomDocument& doc, QDomElement& parent) const;
    virtual QUuid getClassUid() const;
    virtual CloneableElement* clone(Version* sourceVersion, Version* targetVersion);
    virtual void updateGui();

    virtual QString keyword()
    {
        return "FUNCTION";
    }

    QString getJavaReturnType() const
    {
        return m_javaReturns;
    }

    void setJavaReturnType(const QString& a)
    {
        m_javaReturns = a;
    }

    static const QVector<Function*>& family(const Function&);

private:

    QString m_javaReturns; // only if method

};

#endif // CORE_FUNCTION_H
