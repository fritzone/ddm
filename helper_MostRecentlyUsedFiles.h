#ifndef HELPER_MOSTRECENTLYUSEDFILES_H
#define HELPER_MOSTRECENTLYUSEDFILES_H

#include <QString>
#include <QVector>

class MostRecentlyUsedFiles
{
public:

    static MostRecentlyUsedFiles& instance();

public:

    MostRecentlyUsedFiles();
    ~MostRecentlyUsedFiles();

    void addFile(const QString& fileName);

    QVector<QString> getFiles();

private:

    void readFiles();
    void writeFiles();

private:

    QVector<QString> m_files;
    static MostRecentlyUsedFiles* m_instance;
};

#endif // HELPER_MOSTRECENTLYUSEDFILES_H
