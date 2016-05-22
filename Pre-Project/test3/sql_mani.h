#ifndef _SQL_MANI_H_
#define _SQL_MANI_H_

#include <mysql/mysql.h>
#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <fstream>

using namespace std;

struct Data{
    int pid_str;
    char time_str[32];
    char randomString_str[10100];
};

void put_to_mysql(Data mysql_data[], const int & data_size);

#endif
