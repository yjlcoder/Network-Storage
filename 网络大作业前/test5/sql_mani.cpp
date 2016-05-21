#include "sql_mani.h"

using namespace std;

void put_to_mysql(Data mysql_data[], const int & data_size) {
	MYSQL *mysql;
	MYSQL_RES *result;
	MYSQL_ROW row;
	ofstream fout("sql.txt");
	if ((mysql = mysql_init(NULL)) == NULL){
		fout << "mysql init error" << endl;
		exit(0);
	}

	if (mysql_real_connect(mysql,"localhost","root","root123","test",0,NULL,0) == NULL){
		fout << "mysql real connect error" << endl;
		exit(0);
	}

	char buff[20960];
	for (int i = 0; i < data_size; ++i) {
		fout << buff << endl;
		sprintf(buff, "INSERT INTO dbtest5(dbtest_pid, dbtest_time, dbtest_str) VALUES(%d, '%s', '%s')", 
			mysql_data[i].pid_str, mysql_data[i].time_str, mysql_data[i].randomString_str);
		mysql_query(mysql, buff);
	}
	mysql_close(mysql);
	return;
}
