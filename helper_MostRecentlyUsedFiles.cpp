#include "helper_MostRecentlyUsedFiles.h"
#include <QDomDocument>
#include <QFile>
#include <QVector>

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
    QDomDocument doc ("DDM-MOST-RECENT");
    QFile file ("ddm.mrf");
    if (!file.open(QIODevice::ReadOnly)) return;
    if (!doc.setContent(&file))
    {
        file.close();
        return;
    }
    file.close();
    result.reserve(5);
    result.resize(5);
    QDomElement docElem = doc.documentElement();
    for(int i=0; i<docElem.childNodes().size(); i++)
    {
        if(docElem.childNodes().at(i).nodeName() == "File")
        {
            QString fileName = docElem.childNodes().at(i).toElement().attribute("FileName");
            int idx =  docElem.childNodes().at(i).toElement().attribute("idx").toInt();
            if(idx < 5)
            {
                result[idx] = fileName;
            }
        }
    }

    m_files = result;
}

void MostRecentlyUsedFiles::writeFiles()
{
    QDomDocument doc("DDM-MOST-RECENT");
    QDomElement root = doc.createElement("Files");
    for(int i=0; i<m_files.size(); i++)
    {
        QDomElement f = doc.createElement("File");
        f.setAttribute("idx", i);
        f.setAttribute("FileName", m_files.at(i));
        root.appendChild(f);
    }
    doc.appendChild(root);

    QString xml = doc.toString();
    QFile f1("ddm.mrf");
    if (!f1.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return;
    }
    f1.write(xml.toAscii());
    f1.close();
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
