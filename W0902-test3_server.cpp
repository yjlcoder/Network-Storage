#include "stdafx.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WINSOCK2.H>
#pragma warning( disable : 4996)
#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <string>
#include <ctime>
#include <iostream>
#pragma  comment(lib,"ws2_32.lib")
#pragma  comment(lib,"libmysql.lib")
#include <stdlib.h>
#include <winsock.h>
#include "mysql.h"
#include <cstdlib>
#include <fstream>
#include <map>


using namespace std;

const int MAXC = 500;

int bind_port = 8090;
string mysql_ip = "192.168.9.230";
string mysql_user = "root";
string musql_password = "root123";
string mysql_dbname = "test";
string dbname = "dbtest3";

SOCKET sClient[MAXC];
int status[MAXC];

struct SqlData {
	int pid;
	string time;
	string msg;
	int msgl;
	int l;
}sqlData[MAXC];


class CFG {
public:
	map<string, string> nameToValue;
	CFG(const char * fileName) {
		FILE * fp = fopen("winsock_server.cfg", "r");
		char name[256], value[256];
		while (fscanf(fp, "%[^ =]s", name) > 0) {
			char c = fgetc(fp);
			while (c != '=' && c != -1) {
				c = fgetc(fp);
			}
			fscanf(fp, "%s", value);
			nameToValue[name] = value;
			c = fgetc(fp);
			while (c != '\n' && c != -1) {
				c = fgetc(fp);
			}
			cout << name << ' ' << value << endl;
		}
	}
};

CFG cfg("winsock_server.cfg");

void put_to_mysql(const char * tableName, SqlData mysql_data[], const int & data_size) {
	MYSQL *mysql;
	ofstream fout("sql.txt");
	if ((mysql = mysql_init(NULL)) == NULL) {
		fout << "mysql init error" << endl;
		exit(0);
	}

	if (mysql_real_connect(mysql, cfg.nameToValue["mysql_ip"].c_str(),
		cfg.nameToValue["mysql_user"].c_str(),
		cfg.nameToValue["musql_password"].c_str(),
		cfg.nameToValue["mysql_dbname"].c_str(), 0, NULL, 0) == NULL) {
		fout << "mysql real connect error" << endl;
		exit(0);
	}

	char insert[256];
	sprintf(insert, "INSERT INTO %s(dbtest_pid, dbtest_time, dbtest_str) VALUES", cfg.nameToValue[tableName].c_str());
	char buff[20960];
	for (int i = 0; i < data_size; ++i) {
		fout << buff << endl;
		sprintf(buff, "%s(%d, '%s', '%s')", insert,
			mysql_data[i].pid, mysql_data[i].time.c_str(), mysql_data[i].msg.c_str());
		mysql_query(mysql, buff);
	}
	mysql_close(mysql);
	return;
}


int main(int argc, char* argv[]) {
	//初始化WSA
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		return 0;
	}

	//创建套接字
	SOCKET slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (slisten == INVALID_SOCKET)
	{
		printf("socket error !");
		return 0;
	}

	unsigned long flag = 1;
	if (ioctlsocket(slisten, FIONBIO, &flag) != 0) {
		printf("ioctlsocket socket !");
		closesocket(slisten);
		return -1;
	}

	//绑定IP和端口
	bind_port = atoi(cfg.nameToValue["bind_port"].c_str());
	cout << bind_port << endl;
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(bind_port);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	if (bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR) {
		printf("bind error !");
	}

	//开始监听
	if (listen(slisten, 500) == SOCKET_ERROR)
	{
		printf("listen error !");
		return 0;
	}

	int tot = 0;
	int comp = 0;
	sockaddr_in remoteAddr;
	int nAddrlen = sizeof(remoteAddr);
	char revData[10240];
	char buff[256];
	while (comp < 500) {
		FD_SET ReadSet;
		FD_SET WriteSet;
		timeval waitTime;
		waitTime.tv_sec = 1;
		waitTime.tv_usec = 500 * 1000;
		FD_ZERO(&ReadSet);
		FD_ZERO(&WriteSet);
		FD_SET(slisten, &ReadSet);
		FD_SET(slisten, &WriteSet);
		for (int i = 0; i < tot; ++i) {
			if (sClient[i] < 0) continue;
			FD_SET(sClient[i], &ReadSet);
			FD_SET(sClient[i], &WriteSet);
		}


		waitTime.tv_sec = 2;
		waitTime.tv_usec = 0;
		int nRet = select(0, &ReadSet, &WriteSet, 0, &waitTime);
		if (nRet == 0) continue;

		if (tot < 500 && FD_ISSET(slisten, &ReadSet)) {
			sClient[tot] = accept(slisten, (SOCKADDR *)&remoteAddr, &nAddrlen);
			if (sClient[tot] != INVALID_SOCKET) {
				status[tot] = -1;
				++tot;
			}
		}

		for (int i = 0; i < tot; ++i) {
			//	cout << i << ":" << status[i] << endl;
			if (status[i] == -1 && FD_ISSET(sClient[i], &WriteSet)) {
				int number = rand() % 5000 + 5000;
				sprintf(buff, "pidtimestr%d", number);
				sqlData[i].msgl = number;
				sqlData[i].l = 0;
				send(sClient[i], buff, (int)strlen(buff), 0);
				++status[i];
				continue;
			}
			if (!FD_ISSET(sClient[i], &ReadSet)) continue;
			if (status[i] == 0) {
				int ret = recv(sClient[i], revData, 4, 0);
				if (ret > 0) {
					sqlData[i].pid = ntohl(*((int *)revData));
					++status[i];
				}
				continue;
			}

			if (status[i] == 1) {
				int ret = recv(sClient[i], revData, 19, 0);
				if (ret > 0) {
					revData[19] = '\0';
					sqlData[i].time = revData;
					sqlData[i].msg = "";
					++status[i];
				}
				continue;
			}

			if (status[i] == 2) {
				int ret = recv(sClient[i], revData, sqlData[i].msgl - sqlData[i].l, 0);
				if (ret > 0) {
					revData[ret] = '\0';
					sqlData[i].msg += revData;
					sqlData[i].l += ret;
				}
				if (sqlData[i].l == sqlData[i].msgl) {
					++status[i];
					++comp;
					closesocket(sClient[i]);
					sClient[i] = -1;
				}
				continue;
			}
		}
	}

	for (int i = 0; i < comp; ++i) {
		cout << sqlData[i].pid << endl << sqlData[i].time << endl << sqlData[i].msg.length() << endl;
	}

	put_to_mysql("mysql_dbname3", sqlData, comp);
	closesocket(slisten);
	WSACleanup();
	return 0;
}
