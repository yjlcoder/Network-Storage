//
// Created by LovingYoung on 5/2/2016.
//

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <sstream>
#include <winsock2.h>
#include <windows.h>
#include <time.h>

using namespace std;

#define BUFSIZE 10240

#pragma comment(lib, "ws2_32.lib")

DWORD WINAPI mythread(LPVOID lpParameter);

struct Para{
    string server_ip;
	int server_port;
	int socket_num;
	Para(string server_ip, int server_port, int socket_num){
		this->server_ip = server_ip;
		this->server_port = server_port;
		this->socket_num = socket_num;
	}
};

int main(int argc, char * argv[]){
	ifstream fin("winsock_client.cfg");
	string s;
	string server_ip;
	int server_port;
	int socket_num;
	while (fin >> s){
		if (s == "server_ip"){
			fin >> s;
			fin >> server_ip;
		}
		else if (s == "server_port"){
			fin >> s;
			fin >> server_port;
		}
		else if (s == "socket_num"){
			fin >> s;
			fin >> socket_num;
		}
	}
	//cout << server_ip << endl << server_port << endl << socket_num << endl;

	Para para(server_ip, server_port, socket_num);


	HANDLE * handles = new HANDLE[socket_num];
	for (int i = 0; i < socket_num; i++){
		DWORD myThreadID;
		handles[i] = CreateThread(0, 0, mythread, &para, 0, &myThreadID);
	}


	int start = 0;
	while (start < socket_num){
		int remain = socket_num - start;
		remain = remain < 64 ? remain : 64;
		WaitForMultipleObjects(remain, &(handles[start]), TRUE, INFINITE);
		start += remain;
	}
	cout << "END" << endl;
	/*
	for (int i = 0; i < socket_num; i++){
		CloseHandle(handles[i]);
	}
	*/
	delete[] handles;
	return 0;
}

DWORD WINAPI mythread(LPVOID lpParameter){
	Para * para = (Para *)lpParameter;
	int server_port = para->server_port;
	string server_ip = para->server_ip;
	int socket_num = para->socket_num;
	sockaddr_in si_other;
	int soc;
	int slen = sizeof(si_other);
	char buf[BUFSIZE];
	char message[BUFSIZE];
	WSADATA wsa;

	cout << "Initialising Winsock..." << endl;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0){
		cout << "Failed, Error Code : " << WSAGetLastError() << endl;
		exit(EXIT_FAILURE);
	}
	cout << "Initialised ." << endl;

	if ((soc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR) {
		printf("socket() failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	cout << "Socket Created" << endl;

	u_long iMode = 1;
	int iResult = ioctlsocket(soc, FIONBIO, &iMode);
	if (iResult != NO_ERROR){
		cout << "ioctlsocket failed because " << iResult;
	}

	memset((char *)&si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(server_port);
	si_other.sin_addr.S_un.S_addr = inet_addr(server_ip.c_str());

	FD_SET ReadSet;
	FD_SET WriteSet;

	while (true){
		FD_ZERO(&WriteSet);
		FD_SET(soc, &WriteSet);
		int total = select(soc + 1, NULL, &WriteSet, NULL, NULL);
		if (total > 0){
			if (FD_ISSET(soc, &WriteSet)){
				u_long currentThreadId = GetCurrentThreadId();
				u_long threadId = htonl(currentThreadId);
				if (sendto(soc, (char *)&threadId, 4, 0, (sockaddr*)&si_other, slen) == SOCKET_ERROR){
					cout << "Send to Error Code: " << WSAGetLastError() << endl;
					exit(EXIT_FAILURE);
				}
				break;
			}
		}
	}

	while (true){
		FD_ZERO(&WriteSet);
		FD_SET(soc, &WriteSet);
		int total = select(soc + 1, NULL, &WriteSet, NULL, NULL);
		if (total > 0){
			if (FD_ISSET(soc, &WriteSet)){
				time_t now = time(0);
				strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
				string time_str(buf);
				if (sendto(soc, buf, 19, 0, (sockaddr*)&si_other, slen) == SOCKET_ERROR){
					cout << "Send to Error Code: " << WSAGetLastError() << endl;
					exit(EXIT_FAILURE);
				}
				buf[19] = '\0';
				break;
			}
		}
	}

	int randomNumber;
	while (true){
		FD_ZERO(&ReadSet);
		FD_SET(soc, &ReadSet);
		int total = select(soc + 1, &ReadSet, NULL, NULL, NULL);
		if (total > 0){
			if (FD_ISSET(soc, &ReadSet)){
				int recvlen;
				if ((recvlen = recvfrom(soc, buf, 7, 0, (sockaddr*)&si_other, &slen)) == SOCKET_ERROR){
					cout << "recvfrom Error Code: " << WSAGetLastError() << endl;
					exit(EXIT_FAILURE);
				}
				char num[5];
				num[0] = buf[3];
				num[1] = buf[4];
				num[2] = buf[5];
				num[3] = buf[6];
				num[4] = '\0';
				randomNumber = atoi(num);
				break;
			}
		}
	}


	while (true){
		FD_ZERO(&WriteSet);
		FD_SET(soc, &WriteSet);
		int total = select(soc + 1, NULL, &WriteSet, NULL, NULL);
		if (total > 0){
			if (FD_ISSET(soc, &WriteSet)){
				const char* charpool = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
				const int charNum = 52;
				srand(time(0) + GetCurrentThreadId());
				for (int i = 0; i < randomNumber; i++){
					buf[i] = charpool[rand()%charNum];
				}
				if (sendto(soc, buf, randomNumber, 0, (sockaddr*)&si_other, slen) == SOCKET_ERROR){
					cout << "Send to Error Code: " << WSAGetLastError() << endl;
					exit(EXIT_FAILURE);
				}
				buf[randomNumber] = '\0';
				break;
			}
		}
	}

	while (true){
		FD_ZERO(&ReadSet);
		FD_SET(soc, &ReadSet);
		int total = select(soc + 1, &ReadSet, NULL, NULL, NULL);
		if (total > 0){
			if (FD_ISSET(soc, &ReadSet)){
				int recvlen;
				if ((recvlen = recvfrom(soc, buf, 3, 0, (sockaddr*)&si_other, &slen)) == SOCKET_ERROR){
					cout << "recvfrom Error Code: " << WSAGetLastError() << endl;
					exit(EXIT_FAILURE);
				}
				buf[3] = '\0';
				cout << buf << endl;
				break;
			}
		}
	}

	closesocket(soc);
	WSACleanup();
	cout << "EXIT" << endl;
	return 0;
}
