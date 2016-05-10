//
// Created by liuyang on 16-5-7.
//

#include <zconf.h>
#include <fcntl.h>
#include <cstdlib>
#include <sys/param.h>
#include <sys/stat.h>
#include <iostream>
#include <netinet/in.h>
#include <cstring>
#include <fstream>
#include <sstream>
#include <arpa/inet.h>

using namespace std;

const int BUFSIZE = 10240;
const int process = 500;

int main(int argc, char * argv[]) {
    int socketfd;
    char * ipaddr = "127.0.0.1";
    int PortNumber = 4567;
    uint32_t mypid = getpid();
    uint32_t mypid_network = htonl(mypid);
    ofstream fout("out.txt");
    if ((socketfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        fout << "Cannot create socket" << endl;
        exit(0);
    }

    fcntl(socketfd, O_NONBLOCK);

    sockaddr_in myaddr;
    sockaddr_in remaddr;
    socklen_t addrlen = sizeof(myaddr);
    memset(&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(0);
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(socketfd, (sockaddr *) &myaddr, sizeof(myaddr)) < 0) {
        fout << "bind failed" << endl;
        exit(0);
    }

    memset(&remaddr, 0, sizeof(remaddr));
    remaddr.sin_family = AF_INET;
    remaddr.sin_port = htons(PortNumber);
    if(inet_aton(ipaddr, &remaddr.sin_addr) == 0){
        fout << "inet_aton() failed" << endl;
        exit(1);
    }

    int recvlen;
    const int BUFSIZE = 10240;
    char buf[BUFSIZE];

    fd_set readSet;
    fd_set writeSet;
    for(;;) {
        for (; ;) {
            FD_ZERO(&writeSet);
            FD_SET(socketfd, &writeSet);
            int selectValue = select(socketfd + 1, NULL, &writeSet, NULL, NULL);
            if (selectValue <= 0) continue;
            if (!FD_ISSET(socketfd, &writeSet)) continue;
            sendto(socketfd, buf, strlen(buf), 0, (sockaddr *) &remaddr, addrlen);
            fout << "Send: " << buf << endl;
            break;
        }
        for (; ;) {
            FD_ZERO(&readSet);
            FD_SET(socketfd, &readSet);
            int selectValue = select(socketfd + 1, &readSet, NULL, NULL, NULL);
            if (selectValue <= 0) continue;
            if (!FD_ISSET(socketfd, &readSet)) continue;
            recvlen = recvfrom(socketfd, buf, BUFSIZE, 0, (sockaddr *) &remaddr, &addrlen);
            buf[recvlen] = '\0';
            fout << "Recieve: " << buf << endl;
            break;
        }
    }

    close(socketfd);

    exit(0);
}
