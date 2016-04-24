#include <iostream>
#include <zconf.h>
#include <cstring>
#include <sys/stat.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <fstream>
#include <fcntl.h>
#include <poll.h>
#include <errno.h>
#include <sstream>

using namespace std;

void initDaemon();
void createServer(int portNumber);
void sigHandle(int signNum);

int main(int argc, char * argv[]){
    if (argc < 2){
        cout << "Usage: test2-server <PortNnumber>" << endl;
        exit(0);
    }

    int portNumber;
    portNumber = atoi(argv[1]);

    pid_t pid = fork();
    if(pid < 0){
        perror("Fork Error\n");
        exit(0);
    } else if (pid > 0){
        //Parent Process
        exit(0);
    } else if (pid == 0){
        initDaemon();
        // Now is a Daemon Process
        createServer(portNumber);
    }
}

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
    char buff[10240];
    int n;
    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        cout << "Create Socket Failed" << endl;
        exit(0);
    }

    fcntl(listenfd, O_NONBLOCK);

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

        pollfd poll_fd;
        poll_fd.fd = connfd;
        poll_fd.events = POLLIN;

        cout << "Connection Built" << endl;

        srand(time(NULL) + getpid());
        int number = rand()%5000 + 5000;

        stringstream ss("");
        ss << "pidtimestr" << number;

        if(send(connfd, ss.str().c_str(), strlen(ss.str().c_str()), 0) < 0){
            cout << "Send Message Error" << endl;
            exit(0);
        }

        int pollValue = 1;
        while(pollValue > 0){
            pollValue = poll(&poll_fd, 1, -1);
            if(pollValue == -1 && errno == EINTR) continue;
            if(pollValue == -1) break;
            n = recv(connfd, buff, 10240, 0 );
            buff[n] = '\0';
            cout << "Recieve: " << buff << std::flush;
            break;
        }

        pollValue = 1;
        while(pollValue > 0){
            pollValue = poll(&poll_fd, 1, -1);
            if(pollValue == -1 && errno == EINTR) continue;
            if(pollValue == -1) break;
            n = recv(connfd, buff, 10240, 0 );
            buff[n] = '\0';
            cout << "Recieve: " << buff << std::flush;
            break;
        }

        pollValue = 1;
        while(pollValue > 0){
            pollValue = poll(&poll_fd, 1, -1);
            if(pollValue == -1 && errno == EINTR) continue;
            if(pollValue == -1) break;
            n = recv(connfd, buff, 10240, 0 );
            buff[n] = '\0';
            cout << "Recieve: " << buff << std::flush;
            break;
        }

        close(connfd);
    }
    close(listenfd);
}

void sigHandle(int sigNum){

}
