#include "connection.h"

connection::connection()
{

}

bool connection::createconnect()
{
    bool test = false;
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName("employe");
    db.setUserName("nour");
    db.setPassword("sys");

    if (db.open())
        test=true;


    return test;
}
