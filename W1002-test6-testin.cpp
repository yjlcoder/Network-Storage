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

using namespace std;

const int BUFSIZE = 10240;
const int process = 500;

int main(int argc, char * argv[]){
    int socketfd;
    int portNumber = 1234;
    ofstream fout("main.pid.txt");
    if ((socketfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        fout << "Cannot create socket" << endl;
        exit(0);
    }

    cout << "begin" << endl;

    fcntl(socketfd, F_SETFL, O_NONBLOCK);

    sockaddr_in servaddr;
    sockaddr_in remaddr;
    socklen_t addrlen = sizeof(servaddr);
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(uint16_t(portNumber));

    if (bind(socketfd, (sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        fout << "Bind Failed" << endl;
        exit(0);
    }
    cout << "bind" << endl;

    int recvlen;
    char buf[BUFSIZE];

    fd_set readSet;
    fd_set writeSet;
    for(;;) {
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

        for (; ;) {
            FD_ZERO(&writeSetudp);
            FD_SET(socketfd, &writeSet);
            int selectValue = select(socketfd + 1, NULL, &writeSet, NULL, NULL);
            if (selectValue <= 0) continue;
            if (!FD_ISSET(socketfd, &writeSet)) continue;
            sendto(socketfd, buf, strlen(buf), 0, (sockaddr *) &remaddr, addrlen);
            fout << "Send: " << buf << endl;
            break;
        }
    }
}

