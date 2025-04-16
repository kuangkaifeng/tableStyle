#include "databaseopter.h"

DataBaseOpter::DataBaseOpter(QString dbPath)
{
    // 添加数据库驱动，使用 SQLite
    db = QSqlDatabase::addDatabase("QSQLITE");

    // 设置数据库路径
    db.setDatabaseName(dbPath);

    // 尝试打开数据库
    if (!db.open()) {
        qDebug() << dbPath << "无法打开数据库:" << db.lastError().text();
    } else {
        qDebug()<< dbPath << "数据库连接成功！";
    }
}

DataBaseOpter::~DataBaseOpter()
{
    if (db.isOpen()) {
        db.close();
//        qDebug() << "数据库连接已关闭";
    }
}
//
void DataBaseOpter::InsertToDataBase(QString data,QString tableName)
{
    QSqlQuery query;
    QString cmd=QString("INSERT INTO %1 (content) VALUES (?)").arg(tableName);
    query.prepare(cmd);

    query.addBindValue(data);
    if (!query.exec()) {
        qDebug() << "插入数据失败:" << query.lastError().text();
    } else {
        qDebug() << "插入数据成功";
    }
}

QList<dataStruct> DataBaseOpter::getDataFromDataBase(QString tableName)
{
    QList<dataStruct> dataList;

     QSqlDatabase db = QSqlDatabase::database(); // 假设数据库已经打开
     if (!db.isOpen()) {
         qDebug() << "Database is not open!";
         return dataList;
     }

     QSqlQuery query(db);
     QString sql = QString("SELECT id, content, status FROM %1").arg(tableName);
     if (!query.exec(sql)) {
         qDebug() << "Query execution failed:" << query.lastError().text();
         return dataList;
     }

     while (query.next()) {
         dataStruct data;
         data.id = query.value("id").toInt();
         data.content = query.value("content").toString();
         data.status = query.value("status").toInt();

         dataList.append(data);
     }

     return dataList;
}

bool DataBaseOpter::updateStatusBatch(const QString& tableName, const QList<QPair<int, int>>& idStatusList)
{
    if (idStatusList.isEmpty()) {
        qDebug() << "Empty update list.";
        return false;
    }

    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        qDebug() << "Database is not open!";
        return false;
    }

    QSqlQuery query(db);
    db.transaction();  // 开启事务，提升效率并保证一致性

    QString sql = QString("UPDATE %1 SET status = :status WHERE id = :id").arg(tableName);

    query.prepare(sql);

    for (const auto& pair : idStatusList) {
        query.bindValue(":status", pair.second);
        query.bindValue(":id", pair.first);

        if (!query.exec()) {
            qDebug() << "Failed to update id:" << pair.first << " Error:" << query.lastError().text();
            db.rollback();  // 回滚事务
            return false;
        }
    }

    db.commit();  // 提交事务
    return true;
}
