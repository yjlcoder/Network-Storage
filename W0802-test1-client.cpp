//
// Created by liuyang on 16-4-21.
//

#include <iostream>
#include <zconf.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <cstring>
#include <arpa/inet.h>
#include <fstream>
#include <sstream>

using namespace std;

void initDaemon(){
    setsid();
    umask(0);
    for(int i = 0; i < NOFILE; i++){
        close(i);
    }
}

void createClient(char* ipaddr, int PortNumber){
    ofstream cout("/tmp/output_client");
    int sockfd, n;
    char recvLine[4096], sendLine[4096];
    sockaddr_in servaddr;

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        cout << "Create Socket ERROR" << endl;
        exit(0);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PortNumber);
    if(inet_pton(AF_INET, ipaddr, &servaddr.sin_addr) <= 0){
        cout << "Inet_pton ERROR" << endl;
        exit(0);
    }

    if(connect(sockfd, (sockaddr*)&servaddr, sizeof(servaddr)) < 0){
        cout << "Connect Error" << endl;
        exit(0);
    }

    stringstream ss("Message\n");
    if(send(sockfd, ss.str().c_str(), strlen(ss.str().c_str()), 0) < 0){
        cout << "Send Message Error" << endl;
        exit(0);
    }
    close(sockfd);
    exit(0);
}

int main(int argc, char * argv[]){
    if(argc < 3){
        cout << "Usage: test1-client <IP_ADDR> <PORT_NUMBER>" << endl;
        exit(0);
    }

    //GET IP
    char * ipaddr = argv[1];

    //GET PORT
    int portNumber;
    portNumber = atoi(argv[2]);

    pid_t pid = fork();
    if(pid == -1){
        perror("fork new process error\n");
    } else if (pid > 0){
        //Parent Process
        return 0;
    } else {
        //Child Process
        initDaemon();
        createClient(ipaddr, portNumber);
    }
}