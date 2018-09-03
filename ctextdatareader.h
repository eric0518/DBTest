#ifndef CTEXTDATAREADER_H
#define CTEXTDATAREADER_H

#include <QObject>
#include <QStringList>
#include <QDir>

class QFile;

class CTextDataReader : public QObject
{
    Q_OBJECT
public:
    explicit CTextDataReader(QObject *parent = nullptr);
    ~CTextDataReader();

public slots:
    bool openDirectory(QDir dirName);
    QStringList readRecordText(int fieldCount);

private:
    bool tryOpenNewFile();
    bool openFile(QString fileName);
    void closeFile();

private:
    QFile *m_file;
    QFileInfoList m_fileList;
};

#endif // CTEXTDATAREADER_H
