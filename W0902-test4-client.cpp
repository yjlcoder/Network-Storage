//
// Created by LovingYoung on 5/2/2016.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <winsock2.h>
#include <windows.h>

using namespace std;

#define BUFSIZE 4096

#pragma comment(lib, "ws2_32.lib")

int main(int argc, char * argv[]){
    ifstream fin("winsock_client.cfg");
    string s;
    string server_ip;
    int server_port;
    int socket_num;
    while(fin >> s){
        if(s == "server_ip"){
            fin >> s;
            fin >> server_ip;
        } else if (s == "server_port"){
            fin >> s;
            fin >> server_port;
        } else if (s == "socket_num"){
            fin >> s;
            fin >> socket_num;
        }
    }
    //cout << server_ip << endl << server_port << endl << socket_num << endl;

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

    if ( (soc=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR) {
        printf("socket() failed with error code : %d" , WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    cout << "Socket Created" << endl;

    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(server_port);
    si_other.sin_addr.S_un.S_addr = inet_addr(server_ip.c_str());

    while (true){
        cout << " Enter message : ";
        cin >> message;

        if (sendto(soc, message, strlen(message), 0, (sockaddr*)&si_other, slen) == SOCKET_ERROR){
            cout << "Send to Error Code: " << WSAGetLastError() << endl;
            exit(EXIT_FAILURE);
        }

        int recvlen;
        if ((recvlen = recvfrom(soc, buf, BUFSIZE, 0, (sockaddr*)&si_other, &slen)) == SOCKET_ERROR){
            cout << "recvfrom Error Code: " << WSAGetLastError() << endl;
            exit(EXIT_FAILURE);
        }
        cout << "Recvlen: " << recvlen << endl;
        buf[recvlen] = '\0';
        cout << "BUF: " << buf << endl;
    }

    closesocket(soc);
    WSACleanup();
    return 0;
}
