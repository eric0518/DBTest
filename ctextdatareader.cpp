#include "ctextdatareader.h"
#include <QFile>
#include <QStringList>
#include <QDebug>

CTextDataReader::CTextDataReader(QObject *parent) : QObject(parent)
{
    m_file = new QFile(this);
    m_fileList = QFileInfoList();
}

CTextDataReader::~CTextDataReader()
{
    closeFile();
}

// public slots


bool CTextDataReader::openDirectory(QDir dir)
{
    if (!dir.exists())
    {
        qDebug() << "Error : Dir is Not existed.";
        return false;
    }

    m_fileList = dir.entryInfoList(QDir::Files, QDir::Name);
    qDebug() << "m_fileList = " << m_fileList.size();
    return !m_fileList.isEmpty();
}

QStringList CTextDataReader::readRecordText(int fieldCount)
{
    QStringList recordText;

    for (int i=0; i<fieldCount; i++)
    {
        if (m_file->atEnd())
        {
            if (!tryOpenNewFile())
            {
                break;
            }
        }

        QByteArray line = m_file->readLine();
        QString fieldText(QString::fromLocal8Bit(line.data()));

        recordText.append(fieldText);
    }

    return recordText;
}

void CTextDataReader::closeFile()
{
    if (m_file->isOpen())
    {
        m_file->close();
    }
}

// private
bool CTextDataReader::tryOpenNewFile()
{
    if (m_file->isOpen())
    {
        m_file->close();
    }

    if (!m_fileList.size())
    {
        return false;
    }

    QFileInfo fileInfo = m_fileList.takeFirst();
    QString fileName = fileInfo.absoluteFilePath();
    qDebug() << "fileName = " << fileName;
    QDir dir;
    if (fileName.isEmpty() || !dir.exists(fileName))
    {
        return false;
    }

    return openFile(fileName);
}

bool CTextDataReader::openFile(QString fileName)
{
    qDebug() << __FUNCTION__ << "-< try to open file (" << fileName << ")";
    m_file->setFileName(fileName);
    if (!m_file->open(QIODevice::ReadOnly | QIODevice::Text))
    {
         return false;
    }

    return true;
}
