#ifndef __MYBD_H__
#define __MYDB_H__
#include<iostream>  
#include<string>  
#include<mysql/mysql.h>  
#include<vector>
using namespace std;  
  
class MyDB  
{  
public:  
    MyDB();  
    ~MyDB();  
    bool initDB(string host, string user, string pwd, string db_name);  
    bool exeSQL(string sql, vector<string> &SQL_ans);  
    bool exeSQL_SELECT(string sql, vector<string> &SQL_ans);  
private:  
    MYSQL *connection;  
    MYSQL_RES *result;  
    MYSQL_ROW row;  
};

#endif
