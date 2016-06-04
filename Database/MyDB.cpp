#include <iostream>
#include <bitset>
#include <zconf.h>
#include <cstring>
#include <sys/stat.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <fstream>
#include <fcntl.h>
#include <poll.h>
#include <errno.h>
#include <sstream>
#include <cstdlib>
#include <string>
#include <mysql/mysql.h>
#include <string>
#include <vector>
#include "MyDB.h"
using namespace std;

MyDB::MyDB()
{
	connection = mysql_init(NULL); // 初始化数据库连接变量
	if(connection == NULL)
	{
		cout << "Error:" << mysql_error(connection);
		exit(1);
	}
}

MyDB::~MyDB()
{
	if(connection != NULL)  // 关闭数据库连接
	{
		mysql_close(connection);
	}
}

bool MyDB::initDB(string host, string user, string pwd, string db_name)
{
	// 函数mysql_real_connect建立一个数据库连接
	// 成功返回MYSQL*连接句柄，失败返回NULL
	connection = mysql_real_connect(connection, host.c_str(),
			user.c_str(), pwd.c_str(), db_name.c_str(), 0, NULL, 0);
	mysql_set_character_set(connection, "gbk");
	if(connection == NULL)
	{
		cout << "Error:" << mysql_error(connection);
		exit(1);
	}
	return true;
}

bool MyDB::exeSQL(string sql, vector<string> &SQL_ans)
{
	// mysql_query()执行成功返回0，失败返回非0值。与PHP中不一样
//	cout << "进行sql操作" << sql << endl;
	if(mysql_query(connection, sql.c_str()))
	{
		cout << "Query Error:" << mysql_error(connection);
//		exit(1);
		return false;
	}
	else
	{
		result = mysql_store_result(connection); // 获取结果集
		// mysql_field_count()返回connection查询的列数
		for(int i=0; i < mysql_field_count(connection); ++i)
		{
			// 获取下一行
			row = mysql_fetch_row(result);
			if (row == NULL)
				break;
			if(row <= 0)
			{
				break;
			}
			// mysql_num_fields()返回结果集中的字段数
			for(int j=0; j < mysql_num_fields(result); ++j)
			{
//				cout << row[j] << " ";
				SQL_ans.push_back(row[j]);
			}
//			cout << endl;
		}
		// 释放结果集的内存
		mysql_free_result(result);
	}
	return true;
}

bool MyDB::exeSQL_SELECT(string sql, vector<string> &SQL_ans)
{
	// mysql_query()执行成功返回0，失败返回非0值。与PHP中不一样
//	cout << "进行sql操作" << sql << endl;
	if(mysql_query(connection, sql.c_str()))
	{
		cout << "Query Error:" << mysql_error(connection);
//		exit(1);
		return false;
	}
	else
	{
		result = mysql_store_result(connection); // 获取结果集
		// mysql_field_count()返回connection查询的列数
		int fieldcount = mysql_num_fields(result);
//		cout << "fieldcount is " << fieldcount << endl;
		while (row = mysql_fetch_row(result))
		{
			// mysql_num_fields()返回结果集中的字段数
			for(int j=0; j < fieldcount; ++j)
			{
				if (row[j] != NULL)
				{
//					cout << row[j] << " ";
					SQL_ans.push_back(row[j]);
				}
				else 
				{
//					cout << "NULL" << " ";
					SQL_ans.push_back("NULL");
				}
			}
//			cout << endl;
		}
		// 释放结果集的内存
		mysql_free_result(result);
	}
	return true;
}
