#ifndef CONFIGURATION_H
#define CONFIGURATION_H

class Configuration
{
public:

    static const Configuration& instance()
    {
        static Configuration inst;
        return inst;
    }

private:

    Configuration();

    ~Configuration();

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

private:

    // whether the "New Data Type" window should populate the default lengths
    bool useDefaultLengths;

    // if the application allows that the foreign keys are being propagated to the "child" tables
    bool m_allowForeignKeyPropagation;

    // if we want to draw the specific table types (persistent, temporary) with double/dashed line
    bool m_drawTableTypes;
};

#endif // CONFIGURATION_H
