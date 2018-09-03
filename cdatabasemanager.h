#ifndef CDATABASEMANAGER_H
#define CDATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariantMap>

class CDatabaseManager : public QObject
{
    Q_OBJECT
public:
    explicit CDatabaseManager(QObject *parent = nullptr);

signals:

public slots:
    bool openDatabase(QString name);
    void closeDatabase();

    bool createTable(QString tableName, QStringList fieldNames, bool isVirtual = false);
    bool insertRecord(QString tableName, QStringList record, int fieldCount);
    //bool insertRecords(QList<QStringList> records, int fieldCount);

    QList<QStringList> allRecord(QString tableName);

    QVariant searchQuery(QString tableName, QString searchText, bool isVirtual = false);
    QVariant countQuery(QString tableName, bool isVirtual = false);

private:
    QVariant exec(QString cmd, QVariantMap bindValues = QVariantMap());
    QString makeCreateSql(QString tableName, QStringList fieldNames, bool isVirtual = false);
    QString makeInsertSql(QString tableName, QStringList fieldNames);

private:
    QSqlDatabase m_sqlDatabase;
    QSqlQuery m_sqlQuery;
    QString m_tableName;
    QStringList m_fileds;
};

#endif // CDATABASEMANAGER_H
