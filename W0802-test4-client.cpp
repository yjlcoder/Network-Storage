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

void initDaemon();

void createClient(char* ipaddr, int PortNumber);

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

void initDaemon(){
    setsid();
    umask(0);
    for(int i = 0; i < NOFILE; i++){
        close(i);
    }
}

void createClient(char* ipaddr, int PortNumber) {
    int socketfd;
    if ((socketfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Cannot create socket");
        exit(0);
    }

    sockaddr_in myaddr;
    sockaddr_in remaddr;
    int slen = sizeof(myaddr);
    memset(&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(0);
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(socketfd, (sockaddr *) &myaddr, sizeof(myaddr)) < 0) {
        perror("bind failed");
        exit(0);
    }

    memset(&remaddr, 0, sizeof(remaddr));
    remaddr.sin_family = AF_INET;
    remaddr.sin_port = htons(PortNumber);
    if(inet_aton(ipaddr, &remaddr.sin_addr) == 0){
        perror("inet_aton() failed");
        exit(1);
    }

    const int BUFSIZE = 10240;
    char buf[BUFSIZE];

    buf[0] = '1';
    buf[1] = '2';
    buf[2] = '3';
    buf[3] = '\0';

    for(int i = 0; i < 5; i++){
        if(sendto(socketfd, buf, BUFSIZE, 0, (sockaddr*)&remaddr, slen) == -1){
            cout << "sendto" << endl;
        }
    }
    close(socketfd);
    exit(0);
}
