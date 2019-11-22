#include "helper_MostRecentlyUsedFiles.h"
#include "ddm_strings.h"

#include <QtXml/QDomDocument>
#include <QFile>
#include <QVector>
#include <QSettings>


MostRecentlyUsedFiles* MostRecentlyUsedFiles::m_instance = 0;

MostRecentlyUsedFiles::MostRecentlyUsedFiles() : m_files()
{
    readFiles();
}

MostRecentlyUsedFiles::~MostRecentlyUsedFiles()
{
    writeFiles();
}

void MostRecentlyUsedFiles::readFiles()
{
    QVector<QString> result;
    QSettings s(strUnauthorizedFrog, strDDM);

    result.push_back(s.value(strMru1, QVariant("")).toString());
    result.push_back(s.value(strMru2, QVariant("")).toString());
    result.push_back(s.value(strMru3, QVariant("")).toString());
    result.push_back(s.value(strMru4, QVariant("")).toString());
    result.push_back(s.value(strMru5, QVariant("")).toString());

    for(int i=0; i<result.size(); i++)
    {
        if(QFile::exists(result[i]))
        {
            m_files.append(result[i]);
        }
    }
    while(m_files.size() < 5) m_files.append("");
}

void MostRecentlyUsedFiles::writeFiles()
{
    QSettings s(strUnauthorizedFrog, strDDM);
    s.setValue(strMru1, m_files.at(0));
    s.setValue(strMru2, m_files.at(1));
    s.setValue(strMru3, m_files.at(2));
    s.setValue(strMru4, m_files.at(3));
    s.setValue(strMru5, m_files.at(4));
    s.sync();
}

MostRecentlyUsedFiles& MostRecentlyUsedFiles::instance()
{
    if(m_instance == 0)
    {
        m_instance = new MostRecentlyUsedFiles();
    }

    return *m_instance;
}

QVector<QString> MostRecentlyUsedFiles::getFiles()
{
    return m_files;
}

void MostRecentlyUsedFiles::addFile(const QString &fileName)
{
    m_files.push_front(fileName);
    for(int i=1; i<m_files.size(); i++)
    {
        if(m_files.at(i) == fileName)
        {
            m_files.remove(i);
            break;
        }
    }
    while(m_files.size() > 5)
    {
        m_files.remove(5);
    }
    writeFiles();
}
