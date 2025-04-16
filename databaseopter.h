#ifndef DATABASEOPTER_H
#define DATABASEOPTER_H
#include <QSqlDatabase>
#include <QObject>
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>

struct dataStruct
{
    int id;
    QString content;
    int status;
};

class DataBaseOpter:public QObject
{
    Q_OBJECT
public:
    DataBaseOpter(QString dbPath);
    ~DataBaseOpter();
    void InsertToDataBase(QString data,QString tableName);
    QList<dataStruct> getDataFromDataBase(QString tableName);
    bool updateStatusBatch(const QString& tableName, const QList<QPair<int, int>>& idStatusList);


private:
    QSqlDatabase db;
};

#endif // DATABASEOPTER_H
