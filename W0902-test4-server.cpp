#include <iostream>
#include <fstream>
#include <winsock2.h>
#include <windows.h>
#include <sstream>

#define BUFSIZE 4096

#pragma comment(lib, "ws2_32.lib")

using namespace std;

DWORD WINAPI myThread(LPVOID lpParameter);

int main(){
    //Read From file
    ifstream fin("winsock_server.cfg");
    string s;
    int bind_port;
    string mysql_ip;
    string mysql_user;
    string mysql_password;
    string mysql_dbname;
    string mysql_tbname1;
    string mysql_tbname2;
    string mysql_tbname3;
    string mysql_tbname4;
    while(fin >> s){
        if(s == "bind_port"){
            fin >> s;
            fin >> bind_port;
        } else if (s == "mysql_ip"){
            fin >> s;
            fin >> mysql_ip;
        } else if (s == "mysql_user"){
            fin >> s;
            fin >> mysql_user;
        } else if (s == "mysql_password"){
            fin >> s;
            fin >> mysql_password;
        } else if (s == "mysql_dbname"){
            fin >> s;
            fin >> mysql_dbname;
        } else if (s == "mysql_tbname1"){
            fin >> s;
            fin >> mysql_tbname1;
        } else if (s == "mysql_tbname2"){
            fin >> s;
            fin >> mysql_tbname2;
        } else if (s == "mysql_tbname3"){
            fin >> s;
            fin >> mysql_tbname3;
        } else if (s == "mysql_tbname4"){
            fin >> s;
            fin >> mysql_tbname4;
        }
        //cout << bind_port << endl << mysql_ip << endl << mysql_user << endl << mysql_password << endl << mysql_dbname << endl << mysql_tbname1 << endl << mysql_tbname2 << endl << mysql_tbname3 << endl << mysql_tbname4 << endl;

        SOCKET soc;
        sockaddr_in server, si_other;
        int slen, recv_len;
        char buf[BUFSIZE];
        WSADATA wsa;

        slen = sizeof(si_other);

        cout << "Initialising Winsock... " << endl;
        if(WSAStartup(MAKEWORD(2, 2), &wsa) != 0){
            cout << "Failed. Error Code: " << WSAGetLastError() << endl;
            exit(EXIT_FAILURE);
        }
        cout << "Initialised." << endl;

        if((soc = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET){
            cout << "Could not create a soc" << endl;
            exit(0);
        }
        cout << "Socket Created" << endl;

        u_long iMode;
        int iResult = ioctlsocket(soc, FIONBIO, &iMode);
        if (iResult != NO_ERROR){
            cout << "ioctlsocket failed because " << iResult;
        }
        
        memset(&server, 0, sizeof(server));
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = INADDR_ANY;
        server.sin_port = htons(bind_port);

        if(bind(soc, (sockaddr *)&server, sizeof(server)) == SOCKET_ERROR){
            cout << "Bind failed with error code" << WSAGetLastError() << endl;
            exit(EXIT_FAILURE);
        }

        cout << "Bind Done" << endl;

        while(true){

            FD_SET ReadSet;
            FD_SET WriteSet;

            while (true){
                FD_ZERO(&ReadSet);
                FD_SET(soc, &ReadSet);
                int total = select(soc + 1, &ReadSet, NULL, NULL, NULL);
                if (total > 0){
                    if (FD_ISSET(soc, &ReadSet)){
                        if((recv_len = recvfrom(soc, buf, BUFSIZE, 0, (sockaddr *) &si_other, &slen)) == SOCKET_ERROR){
                            cout << "Recvfrom with error code " << WSAGetLastError() << endl;
                            exit(EXIT_FAILURE);
                        }
                        buf[recv_len] = '\0';
                        cout << "Received packet from " << inet_ntoa(si_other.sin_addr) << " : " << ntohs(si_other.sin_port) << endl;
                        cout << "Data: " << buf << endl;
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
                        cin >> buf;
                        cout << buf << " " << strlen(buf) << endl;

                        if(sendto(soc, buf, strlen(buf), 0, (sockaddr*) &si_other, slen) == SOCKET_ERROR){
                            cout << "Sendto() failed with error code : " << WSAGetLastError() << endl;
                            exit(EXIT_FAILURE);
                        }
                        break;
                    }
                }

            }
        }

        closesocket(soc);
        WSACleanup();

        return 0;
    }
}

DWORD WINAPI myThread(LPVOID lpParameter){
    cout << "Thread Inside " << GetCurrentThreadId() << endl;
    return 0;
}
