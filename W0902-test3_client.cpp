
#include "stdafx.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WINSOCK2.H>
#pragma warning( disable : 4996)
#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>

#include <string>
#include <ctime>

#pragma  comment(lib,"ws2_32.lib")

using namespace std;

string mysql_ip = "127.0.0.1";
string bind_port = "8989";
int client_num = 100;

class mySocketClass {
public:
	WORD sockVersion;
	WSADATA data;
	SOCKET sclient;
	sockaddr_in serAddr;
	FILE * fp;
	int id;
	int status;
	int datal;

	int init() {
		sockVersion = MAKEWORD(2, 2);
		data;
		if (WSAStartup(sockVersion, &data) != 0) {
			printf("WSAStarup error !");
			return -1;
		}
		sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sclient == INVALID_SOCKET) {
			printf("invalid socket !");
			return -1;
		}

		unsigned long flag = 1;
		if (ioctlsocket(sclient, FIONBIO, &flag) != 0) {
			printf("ioctlsocket socket !");
			closesocket(sclient);
			return -1;
		}

		int portNum;
		sscanf(bind_port.c_str(), "%d", &portNum);
		serAddr;
		serAddr.sin_family = AF_INET;
		serAddr.sin_port = htons(portNum);
		serAddr.sin_addr.S_un.S_addr = inet_addr(mysql_ip.c_str());
		return 0;
	}

	int myConnect() {
		int times = 100;
		while (times--) {
			if (connect(sclient, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR) {
				struct timeval timeout;
				fd_set r;
				int ret;
				FD_ZERO(&r);
				FD_SET(sclient, &r);
				timeout.tv_sec = 1;
				timeout.tv_usec = 500 * 1000;
				ret = select(0, 0, &r, 0, &timeout);
				if (ret > 0) {
					return 0;
				}
			}
		}
		printf("%d connect error!", id);
		closesocket(sclient);
		return -1;
	}

	int myRecv(void * recData, int maxl, int mode) {
		return recv(sclient, (char *)recData, maxl, mode);
	}

	int mySend(void * recData, int datal, int mode) {
		return send(sclient, (char *)recData, datal, mode);
	}

	FILE * setFile(const int & port) {
		char fileName[256];
		sprintf(fileName, "%d.pid.txt", port);
		return (fp = fopen(fileName, "w"));
	}

	void logOut() {
		closesocket(sclient);
	}

	~mySocketClass() {

	}

}test[500 * 2];
int error[500 * 2];
int ok[500 * 2];

char buff[10240];
#include <iostream>
using namespace std;

void getCFG() {
	FILE * fp = fopen("winsock_client.cfg", "r");
	if (!fp) return;
	string ip = "server_ip";
	string port = "server_port";
	string num = "socket_num";

	char str[256];
	for (int i = 0; i < 3; ++i) {
		fscanf(fp, "%[^ =]s", str);
		char c;
		while (1) {
			fscanf(fp, "%c", &c);
			if (c == '=') break;
		}
		string charar = str;
		if (charar == ip) {
			fscanf(fp, "%s", str);
			mysql_ip = str;
		}
		if (charar == port) {
			fscanf(fp, "%s", str);
			bind_port = str;
		}
		if (charar == num) {
			int num;
			fscanf(fp, "%d", &num);
			client_num = num;
		}
		while (fgetc(fp) != '\n');
	}
}


int main(int argc, char* argv[]) {
	getCFG();
	cout << mysql_ip << ' ' << bind_port << ' ' << client_num << endl;
	char recData[255];
	int tot = 0;
	for (int i = 0; i < client_num; ++i) {
		printf("%d\n", i);
		test[i].id = i;
		test[i].status = -1;
		error[i] = test[i].init();
		if (error[i] == 0) test[i].myConnect();
	}

	while (tot < client_num) {
		for (int i = 0; i < client_num; ++i) {
			if (test[i].status == -1) {
				int ret = test[i].myRecv(recData, 255, 0);
				if (ret > 0) {
					recData[ret] = 0x00;
					printf("%d : %s\n", i, recData);
					test[i].status = 0;

					recData[14] = '\0';
					sscanf(recData + 10, "%d", &test[i].datal);
					printf("%d : %d\n", i, test[i].datal);
					continue;
				}
			}
			else if (test[i].status == 0) {
				sockaddr_in addrMy;
				memset(&addrMy, 0, sizeof(addrMy));
				int len = sizeof(addrMy);
				getsockname(test[i].sclient, (sockaddr*)&addrMy, &len);
				int num = addrMy.sin_port;

				test[i].setFile(num);
				fprintf(test[i].fp, "%d\n", num);

				num = htonl(num);
				test[i].mySend(&num, 4, 0);
				time_t now = time(0);
				strftime(buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", localtime(&now));
				test[i].mySend(buff, 19, 0);
				fprintf(test[i].fp, "%s\n", buff);

				const char charpool[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
				const int charNum = sizeof(charpool) / sizeof(char) - 1;
				for (int k = 0; k < test[i].datal; k++) buff[k] = charpool[rand() % charNum];
				test[i].mySend(buff, test[i].datal, 0);
				buff[test[i].datal] = 0;
				++test[i].status;
				fprintf(test[i].fp, "%s\n", buff);
			}
			else if (test[i].status == 1) {
				int ret = test[i].myRecv(recData, 255, 0);
				if (ret <= 0) {
					++tot;
					++test[i].status;
					test[i].logOut();
				}
			}
		}
	}

	WSACleanup();
	return 0;
}
