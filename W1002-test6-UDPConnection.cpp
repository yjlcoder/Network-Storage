//
// Created by liuyang on 16-5-7.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <sys/socket.h>
#include <sstream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <climits>
#include <zconf.h>
#include <netdb.h>
#include <stdlib.h>
#include <errno.h>
#include "W1002-test6-util.h"

#define BUFSIZE 10240

using namespace std;

struct UDPPara{
    string listenIP;
    int listenPort;
    string transferIP;
    int transferPort;
    int socketfd;
    sockaddr_in servaddr;
};

struct UDPTable{
    uint16_t clientPort;
    in_addr clientAddr;
    sockaddr_in myaddr;
    int mysocketfd;
};

int main(int argc, char * argv[]) {
    ofstream cout("log.dat");
    char logbuf[BUFSIZE];
    char buf[BUFSIZE];
    int maxfd;
    stringstream ss("");

    //Read From Parameters
    if (argc <= 1) {
        cout << "No Parameters : -> END" << endl;
        return 0;
    }
    vector<UDPPara> UDPParas;
    vector<UDPTable> UDPTables;
    for (int i = 1; i < argc; i += 4) {
        UDPPara para;
        para.listenIP = argv[i];
        para.listenPort = atoi(argv[i + 1]);
        para.transferIP = argv[i + 2];
        para.transferPort = atoi(argv[i + 3]);
        para.socketfd = 0;
        UDPParas.push_back(para);
    }

    cout << Log((char *) "Read From Parameters Complete") << endl;

    hostent *he;
    in_addr ** temp_addr;
    for(int i = 0; i < UDPParas.size(); i++){
        he = gethostbyname(UDPParas[i].listenIP.c_str());
        if(he == NULL){
            sprintf(logbuf, "Transfer hostname failed, Source: %s", UDPParas[i].listenIP.c_str());
            cout << Log(logbuf) << endl;
        }
        temp_addr = (in_addr **) he->h_addr_list;
        sprintf(logbuf, "Transfer hostname to IP, from %s to %s", UDPParas[i].listenIP.c_str(), inet_ntoa(*temp_addr[0]));
        cout << Log(logbuf) << endl;
        UDPParas[i].listenIP = inet_ntoa(*temp_addr[0]);

        he = gethostbyname(UDPParas[i].transferIP.c_str());
        if(he == NULL){
            sprintf(logbuf, "Transfer hostname failed, Source: %s", UDPParas[i].transferIP.c_str());
            cout << Log(logbuf) << endl;
        }
        temp_addr = (in_addr **) he->h_addr_list;
        sprintf(logbuf, "Transfer hostname to IP, from %s to %s", UDPParas[i].transferIP.c_str(), inet_ntoa(*temp_addr[0]));
        cout << Log(logbuf) << endl;
        UDPParas[i].transferIP= inet_ntoa(*temp_addr[0]);
    }

    //Create Initial Socket
    for (int i = 0; i < UDPParas.size(); i++) {
        if ((UDPParas[i].socketfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            sprintf(logbuf, "Can't create socket for %s:%d", UDPParas[i].listenIP.c_str(), UDPParas[i].listenPort);
            cout << Log(logbuf) << endl;
            sprintf(logbuf, "Delete Parameters %s:%d to %s:%d", UDPParas[i].listenIP.c_str(), UDPParas[i].listenPort,
                    UDPParas[i].transferIP.c_str(), UDPParas[i].transferPort);
            cout << Log(logbuf) << endl;
            UDPParas.erase(UDPParas.begin() + i);
            i--;
            continue;
        }
        maxfd = UDPParas[i].socketfd;
        fcntl(UDPParas[i].socketfd, F_SETFL, O_NONBLOCK);
        memset(&(UDPParas[i].servaddr), 0, sizeof(UDPParas[i].servaddr));
        UDPParas[i].servaddr.sin_family = AF_INET;
        UDPParas[i].servaddr.sin_port = htons(uint16_t(UDPParas[i].listenPort));
        if (inet_pton(AF_INET, UDPParas[i].listenIP.c_str(), &(UDPParas[i].servaddr.sin_addr)) <= 0) {
            sprintf(logbuf, "Inet_pton Error for %s:%d, ErrorCode:%d", UDPParas[i].listenIP.c_str(),
                    UDPParas[i].listenPort, errno);
            cout << Log(logbuf) << endl;
            sprintf(logbuf, "Delete Parameters %s:%d to %s:%d", UDPParas[i].listenIP.c_str(), UDPParas[i].listenPort,
                    UDPParas[i].transferIP.c_str(), UDPParas[i].transferPort);
            cout << Log(logbuf) << endl;
            UDPParas.erase(UDPParas.begin() + i);
            i--;
            continue;
        }

        if (bind(UDPParas[i].socketfd, (sockaddr *) &(UDPParas[i].servaddr), sizeof(UDPParas[i].servaddr)) < 0) {
            sprintf(logbuf, "Bind Failed for %s:%d", UDPParas[i].listenIP.c_str(), UDPParas[i].listenPort);
            cout << Log(logbuf) << endl;
            sprintf(logbuf, "Delete Parameters %s:%d to %s:%d", UDPParas[i].listenIP.c_str(), UDPParas[i].listenPort,
                    UDPParas[i].transferIP.c_str(), UDPParas[i].transferPort);
            cout << Log(logbuf) << endl;
            UDPParas.erase(UDPParas.begin() + i);
            i--;
            continue;
        }

        sprintf(logbuf, "Bind Succeed with %s:%d", UDPParas[i].listenIP.c_str(), UDPParas[i].listenPort);
        cout << Log(logbuf) << endl;
    }

    sockaddr_in remaddr;
    socklen_t socklen = sizeof(remaddr);

    fd_set readSet, writeSet;

    int recvlen, sendlen;

    for(;;) {
        //Select whitch one can be read
        FD_ZERO(&readSet);
        for (vector<UDPPara>::iterator it = UDPParas.begin(); it != UDPParas.end(); it++) {
            FD_SET(it->socketfd, &readSet);
        }
        for (vector<UDPTable>::iterator it = UDPTables.begin(); it != UDPTables.end(); it++) {
            FD_SET(it->mysocketfd, &readSet);
        }
        int selectValue = select(maxfd + 1, &readSet, NULL, NULL, NULL);
        if (selectValue <= 0) continue;

        for (vector<UDPPara>::iterator it = UDPParas.begin(); it != UDPParas.end(); it++) {
            if (!FD_ISSET(it->socketfd, &readSet)) continue;
            //Read from the port
            recvlen = recvfrom(it->socketfd, buf, BUFSIZE, 0, (sockaddr *) &remaddr, &socklen);
            buf[recvlen] = '\0';
            sprintf(logbuf, "Recieve From %s:%d, Message:%s", inet_ntoa(remaddr.sin_addr), ntohs(remaddr.sin_port), buf);
            cout << Log(logbuf) << endl;

            //Find in the UDPTables. if not exists, create one
            vector<UDPTable>::iterator itTable;
            for(itTable = UDPTables.begin(); itTable != UDPTables.end(); itTable++){
                if(itTable->clientAddr.s_addr == remaddr.sin_addr.s_addr && itTable->clientPort == remaddr.sin_port) break;
            }
            if(itTable == UDPTables.end()){
                UDPTable table;
                table.clientAddr = remaddr.sin_addr;
                table.clientPort = remaddr.sin_port;
                if ((table.mysocketfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
                    sprintf(logbuf, "Can't create socket");
                    cout << Log(logbuf) << endl;
                }
                maxfd = table.mysocketfd;
                cout << Log("Create a new socket") << endl;
                fcntl(table.mysocketfd, F_SETFL, O_NONBLOCK);
                memset(&table.myaddr, 0, sizeof(table.myaddr));
                table.myaddr.sin_family = AF_INET;
                table.myaddr.sin_port = htons((uint16_t)it->transferPort);
                if(inet_aton(it->transferIP.c_str(), &(table.myaddr.sin_addr)) == 0){
                    cout << Log("inet_aton failed. Interrupt the session") << endl;
                    break;
                }

                UDPTables.push_back(table);
                itTable = UDPTables.end() - 1;
            }

            //now itTable point to the UDPTable
            //Send the message to Destination
            for(;;){
                FD_ZERO(&writeSet);
                FD_SET(itTable->mysocketfd, &writeSet);
                selectValue = select(itTable->mysocketfd + 1, NULL, &writeSet, NULL, NULL);
                if(selectValue <= 0) continue;
                if(!FD_ISSET(itTable->mysocketfd, &writeSet)) continue;
                sendlen = sendto(itTable->mysocketfd, buf, strlen(buf), 0, (sockaddr*)&(itTable->myaddr), socklen);
                sprintf(logbuf, "Send to %s:%d, Message:%s", inet_ntoa(itTable->myaddr.sin_addr), ntohs(itTable->myaddr.sin_port), buf);
                cout << Log(logbuf) << endl;
                break;
            }
        }
        for(vector<UDPTable>::iterator it = UDPTables.begin(); it != UDPTables.end(); it++){
            if(!FD_ISSET(it->mysocketfd, &readSet)) continue;
            sockaddr_in inaddr;
            recvlen = recvfrom(it->mysocketfd, buf, BUFSIZE, 0, (sockaddr*)&inaddr, &socklen);
            buf[recvlen] = '\0';
            sprintf(logbuf, "Recieve From %s:%d, Message:%s", inet_ntoa(inaddr.sin_addr), ntohs(inaddr.sin_port), buf);
            cout << Log(logbuf) << endl;

            vector<UDPTable>::iterator itTable;
            for(itTable = UDPTables.begin(); itTable != UDPTables.end(); itTable++){
                if(itTable->myaddr.sin_port == inaddr.sin_port && itTable->myaddr.sin_addr.s_addr == inaddr.sin_addr.s_addr) break;
            }
            if(itTable == UDPTables.end()){
                cout << Log("Can't find itTable in UDPTables, Drop the Session") << endl;
                continue;
            }
            //find socketfd
            vector<UDPPara>::iterator itp;
            for(itp = UDPParas.begin(); itp != UDPParas.end(); itp++){
                if(itp->transferIP == inet_ntoa(inaddr.sin_addr) && itp->transferPort == ntohs(inaddr.sin_port)) break;
            }
            if(itp == UDPParas.end()){
                cout << Log("Can't find it in UDPParas, Drop the Session") << endl;
                continue;
            }
            //Remote Address : inaddr
            memset(&inaddr, 0, sizeof(inaddr));
            inaddr.sin_addr.s_addr = itTable->clientAddr.s_addr;
            inaddr.sin_port = itTable->clientPort;
            inaddr.sin_family = AF_INET;

            for(;;){
                FD_ZERO(&writeSet);
                FD_SET(itp->socketfd, &writeSet);
                selectValue = select(itp->socketfd + 1, NULL, &writeSet, NULL, NULL);
                if(selectValue <= 0) continue;
                if(!FD_ISSET(itp->socketfd, &writeSet)) continue;
                sendlen = sendto(itp->socketfd, buf, strlen(buf), 0, (sockaddr*)&inaddr, socklen);
                sprintf(logbuf, "Send to %s:%d, Message:%s", inet_ntoa(inaddr.sin_addr), ntohs(inaddr.sin_port), buf);
                cout << Log(logbuf) << endl;
                break;
            }
        }
    }
}
