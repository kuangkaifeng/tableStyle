#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    dbOpter=new DataBaseOpter(QApplication::applicationDirPath()+"/database/log.db");
    QString username = "zhangsan";
    QString time = "2025-04-16T14:30:00";
    QString action = "Login Success";

    QString xml = QString("<log>"
                          "<username>%1</username>"
                          "<loginTime>%2</loginTime>"
                          "<action>%3</action>"
                          "</log>")
                          .arg(username)
                          .arg(time)
                          .arg(action);
    dbOpter->InsertToDataBase(xml,"loginLog");



    QList<QPair<int, int>> updateList;
    updateList.append(qMakePair(1, 1));  // 把 id=1 的 status 改为 2
    updateList.append(qMakePair(3, 0));  // 把 id=3 的 status 改为 0
    updateList.append(qMakePair(5, 1));  // 把 id=5 的 status 改为 1
    dbOpter->updateStatusBatch("loginLog",updateList);

    QList<dataStruct> data=dbOpter->getDataFromDataBase("loginLog");
    qDebug()<<"data:"<<data.at(0).content;
}

Widget::~Widget()
{
    delete ui;
}

