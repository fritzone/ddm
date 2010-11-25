#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QHash>
#include <QString>

class Configuration
{
public:

    static Configuration& instance()
    {
        static Configuration inst;
        return inst;
    }

private:

    Configuration();

    ~Configuration();

    void readFromFile();

    void writeToFile();

public:

    bool defaultLengths() const
    {
        return useDefaultLengths;
    }

    bool allowForeignKeyPropagation() const
    {
        return m_allowForeignKeyPropagation;
    }

    bool drawTableTypes() const
    {
        return m_drawTableTypes;
    }

    QHash<QString,QString> sqlGenerationOptions() const
    {
        return m_sqlOpts;
    }

    void setDrawTabletypes(bool a)
    {
        m_drawTableTypes = a;
    }

    void setAllowForeignKeyPropagation(bool a)
    {
        m_allowForeignKeyPropagation = a;
    }

    QHash<QString,QString>& sqlOpts()
    {
        return m_sqlOpts;
    }

private:

    // whether the "New Data Type" window should populate the default lengths
    bool useDefaultLengths;

    // if the application allows that the foreign keys are being propagated to the "child" tables
    bool m_allowForeignKeyPropagation;

    // if we want to draw the specific table types (persistent, temporary) with double/dashed line
    bool m_drawTableTypes;

    QHash<QString,QString> m_sqlOpts;
};

#endif // CONFIGURATION_H
