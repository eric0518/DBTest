#include "cdatabasemanager.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>
#include <QFile>
#include <Windows.h>

CDatabaseManager::CDatabaseManager(QObject *parent) : QObject(parent)
{
    m_tableName = "";
}

// public slots
QVariant CDatabaseManager::searchQuery(QString tableName, QString searchText, bool isVirtual)
{
    QString sql = "";
    if (isVirtual)
    {
        sql = QString("SELECT * FROM %1 WHERE %2 MATCH \'%3\'").arg(tableName).arg(tableName).arg(searchText);
    }
    else
    {
        sql = QString("SELECT * FROM %1 WHERE body LIKE \'%%2%\' OR title LIKE \'%%2%\'").arg(tableName).arg(searchText);
    }
    qDebug() << "sql = " << sql;

    QVariant result = exec(sql);
    return result;
}

QVariant CDatabaseManager::countQuery(QString tableName, bool isVirtual)
{
    QString sql = "";
    if (isVirtual)
    {
        sql = QString("SELECT COUNT(*) FROM %1").arg(tableName);
    }
    else
    {
        sql = QString("SELECT COUNT(*) FROM %1").arg(tableName);
    }
    qDebug() << "sql = " << sql;

    QVariant result = exec(sql);
    return result;
}

// private
bool CDatabaseManager::openDatabase(QString name)
{
    m_sqlDatabase = QSqlDatabase::addDatabase("QSQLITE");
    m_sqlQuery = QSqlQuery();

    m_sqlDatabase.setDatabaseName(name);
    if (!m_sqlDatabase.open())
    {
        qDebug() << "Failed to Open DB : " << m_sqlDatabase.lastError();
        return false;
    }

    return true;
}

void CDatabaseManager::closeDatabase()
{
    m_sqlDatabase.close();
}

bool CDatabaseManager::createTable(QString tableName, QStringList fieldNames, bool isVirtual)
{
    m_tableName = tableName;
    m_fileds = fieldNames;

    QString createSql = makeCreateSql(tableName, fieldNames, isVirtual);
    qDebug() << "create sql = " << createSql;

    m_sqlQuery.prepare(createSql);
    if(!m_sqlQuery.exec())
    {
        qDebug()<<QObject::tr("Failed to Create Table : ") << m_sqlQuery.lastError();
        return false;
    }

    return true;
}

bool CDatabaseManager::insertRecord(QString tableName, QStringList record, int fieldCount)
{
    if (record.size() < fieldCount)
    {
        qDebug() << "Failed Insert : parameter error";
        return false;
    }

    QString insertSql = QString("INSERT INTO %1 VALUES(?,?)").arg(tableName);
    m_sqlQuery.prepare(insertSql);

    for (int i=0; i<fieldCount; i++)
    {
        m_sqlQuery.addBindValue(QStringList() << record.at(i));
    }

    if(!m_sqlQuery.execBatch())
    {
        qDebug()<< tr("Failed Insert : ") << m_sqlQuery.lastError() << m_sqlQuery.executedQuery();
        return false;
    }

    return true;
}

//bool CDatabaseManager::insertRecords(QList<QStringList> records, int fieldCount)
//{
//    if (!records.size())
//    {
//        return false;
//    }



////    QList<QStringList> columns = QList<QStringList>();
////    columns.reserve(fieldCount);

////    for (int i=0; i<records.size(); i++)
////    {
////        if (records.at(i))
////        for (int colunmIndex = 0; colunmIndex < fieldCount; colunmIndex++)
////        {
////            columns
////        }
////    }

//    return true;
//}

QList<QStringList> CDatabaseManager::allRecord(QString tableName)
{
    QList<QStringList> records = QList<QStringList>();
    QString selectAllSql = QString("select * from %1").arg(tableName);
    m_sqlQuery.prepare(selectAllSql);
    if(!m_sqlQuery.exec())
    {
        qDebug() << tr("Failed : ") << m_sqlQuery.lastError();
        return records;
    }

    while(m_sqlQuery.next())
    {
        QStringList record;
        for (int i=0; i<m_fileds.size(); i++)
        {
            record.append(m_sqlQuery.value(i).toString());

        }
        records.append(record);
    }

    return records;
}

// private
QVariant CDatabaseManager::exec(QString cmd, QVariantMap bindValues)
{
    QVariant result(false);
    do
    {
        if (!m_sqlDatabase.isOpen())
        {
            break;
        }

        QSqlQuery query(m_sqlDatabase);
        // prepare command
        if (!query.prepare(cmd))
        {
            qDebug() << query.lastError().text();
            break;
        }

        // bind values
        for (auto index = bindValues.begin(); index != bindValues.end(); ++index)
        {
            query.bindValue(index.key(), index.value());
        }

        // execute command
        if (!query.exec())
        {
            qDebug() << query.lastError().text();
            break;
        }

        // get result
        if (query.isSelect())
        {
            QVariantList buff;
            QVariantMap tmp;
            QSqlRecord record;

            while (query.next())
            {
                record = query.record();
                tmp.clear();
                for (auto index = 0; index != record.count(); ++index)
                {
                    tmp.insert(record.fieldName(index), record.value(index));
                }
                buff.push_back(tmp);
            }
            result = QVariant(buff);
        }
        else
        {
            result = QVariant(true);
        }
    }
    while(false);

    return result;
}

QString CDatabaseManager::makeCreateSql(QString tableName, QStringList fieldNames, bool isVirtual)
{
    QString sql = "";
    if (isVirtual)
    {
        sql = QString("CREATE VIRTUAL TABLE IF NOT EXISTS %1 ").arg(tableName);
        sql += "USING fts4(";
        for (int i=0; i<fieldNames.size(); i++)
        {
            sql += QString("%1, ").arg(fieldNames.at(i));
        }
        sql += "tokenize = \'unicode61\');";
    }
    else
    {
        sql = QString("CREATE TABLE IF NOT EXISTS %1 ").arg(tableName);
        sql += "(";
        for (int i=0; i<fieldNames.size(); i++)
        {
            sql += QString("%1 TEXT %2 ").arg(fieldNames.at(i)).arg((i<(fieldNames.size() - 1)) ? "," : "");
        }
        sql += ");";
    }

    return sql;
}

QString CDatabaseManager::makeInsertSql(QString tableName, QStringList fieldNames)
{
    return "";
}

