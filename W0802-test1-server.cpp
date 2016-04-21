//
// Created by liuyang on 16-4-21.
//

#include <zconf.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <cstdio>
#include <iostream>
#include <netinet/in.h>
#include <cstring>
#include <fstream>

using namespace std;

void initDaemon(){
    setsid();
    umask(0);
    for(int i = 0; i < NOFILE; i++){
        close(i);
    }
}

void createServer(int portNumber){
    int listenfd, connfd;
    sockaddr_in servaddr;
    ofstream cout("/tmp/output");
    char buff[4096];
    int n;
    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        cout << "Create Socket Failed" << endl;
        exit(0);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(uint16_t(portNumber));

    if(bind(listenfd, (sockaddr *)&servaddr, sizeof(servaddr)) == -1){
        cout << "Bind Socket Error" << endl;
        exit(0);
    }

    if(listen(listenfd, 100) == -1){
        cout << "Listen Socket Error" << endl;
        exit(0);
    }

    while(1){
        if((connfd = accept(listenfd, (sockaddr*) NULL, NULL)) == -1){
            cout << "Accept Socket Error" << endl;
            continue;
        }
        cout << "Connection Built" << endl;
        n = recv(connfd, buff, 4096, 0 );
        buff[n] = '\0';
        cout << "Recieve: " << buff << std::flush;
        close(connfd);
    }
    close(listenfd);
}

int main(int argc, char * argv[]){
    if(argc < 2){
        cout << "Usage : test1-server <port_number> " <<endl;
        exit(0);
    }
    int portNumber;
    portNumber = atoi(argv[1]);
    pid_t pid = fork();
    if(pid == -1){
        perror("fork new process error\n");
    } else if (pid > 0){
        //Parent Process
        return 0;
    } else {
        //Child Process
        initDaemon();
        createServer(portNumber);
    }
}
