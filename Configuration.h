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

private:

    // whether the "New Data Type" window should populate the default lengths
    bool useDefaultLengths;
};

#endif // CONFIGURATION_H
