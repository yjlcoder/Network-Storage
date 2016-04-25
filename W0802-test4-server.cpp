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

void createServer(int portNumber) {
    int socketfd;
    if((socketfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        perror("Cannot create socket");
        exit(0);
    }

    sockaddr_in servaddr;
    sockaddr_in remaddr;
    socklen_t addrlen = sizeof(servaddr);
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(uint16_t(portNumber));

    if(bind(socketfd, (sockaddr*)&servaddr, sizeof(servaddr)) < 0){
        perror("Bind Failed");
        exit(0);
    }

    const int BUFSIZE = 10240;
    int recvlen;
    char buf[BUFSIZE];
    ofstream fout("main.pid.txt");

    for(;;){
        recvlen = recvfrom(socketfd, buf, BUFSIZE, 0, (sockaddr*)&remaddr, &addrlen);
        if(recvlen > 0){
            buf[recvlen] = '\0';
            fout << buf << endl;
        }
    }
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
