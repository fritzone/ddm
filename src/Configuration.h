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

    enum ForeignKeyPosition
    {
        InTable = 1,
        AfterTable = 2,
        OnlyInternal = 3
    };

    enum PrimaryKeyPosition
    {
        PkInColumnDeclaration = 0,
        PkAfterColumnsDeclaration = 1,
        PkAfterTableDeclaration = 2
    };

    bool defaultLengths() const
    {
        return m_useDefaultLengths;
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

    bool continuousValidation() const
    {
        return m_continuousValidation;
    }

    void setContinuousValidation(bool a)
    {
        m_continuousValidation = a;
    }

    bool showStartupdialog() const
    {
        return m_showStartupDialog;
    }

    void enableStartupDialog(bool b)
    {
        m_showStartupDialog = b;
    }

    ForeignKeyPosition sqlOptsGetFkPosition(const QHash<QString, QString> &);

    PrimaryKeyPosition sqlOptsGetPkPosition(const QHash<QString, QString> &);

    bool sqlOptsGetUpcase(const QHash<QString, QString> &);

private:

    // whether the "New Data Type" window should populate the default lengths
    bool m_useDefaultLengths;

    // if the application allows that the foreign keys are being propagated to the "child" tables
    bool m_allowForeignKeyPropagation;

    // if we want to draw the specific table types (persistent, temporary) with double/dashed line
    bool m_drawTableTypes;

    // if we want to perform a validation when the user changes something in a column
    bool m_continuousValidation;

    QHash<QString,QString> m_sqlOpts;

    bool m_showStartupDialog;
};

#endif // CONFIGURATION_H
