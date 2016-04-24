#include <iostream>
#include <zconf.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <cstring>
#include <sstream>
#include <fstream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <poll.h>

using namespace std;

void initDaemon();
void createClient(char * ipaddr, int PortNumber);

int main(int argc, char * argv[]){
    if (argc < 3){
        std::cout << "Usage: test2-client <IP_addr> <PortNnumber>" << std::endl;
        exit(0);
    }

    //GET IP
    char * ipaddr = argv[1];

    //GET PORT
    int portNumber;
    portNumber = atoi(argv[2]);

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
        for(int i = 0; i < 5; i++){
            pid = fork();
            if(pid < 0){
                perror("Fork Error");
                exit(0);
            } else if (pid > 0){
                continue;
            } else if (pid == 0){
                createClient(ipaddr, portNumber);
            }
        }
    }
}

void initDaemon(){
    setsid();
    umask(0);
    for(int i = 0; i < NOFILE; i++){
        close(i);
    }
}

void createClient(char* ipaddr, int PortNumber){
    pid_t mypid = getpid();
    stringstream ss("");
    ss << "/tmp/output_client_" << mypid << ".txt";
    ofstream cout(ss.str().c_str());
    int sockfd, n;
    char recvLine[4096], sendLine[4096];
    sockaddr_in servaddr;

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        cout << "Create Socket ERROR" << endl;
        exit(0);
    }

    fcntl(sockfd, O_NONBLOCK);

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

    pollfd pollfd;
    pollfd.fd = sockfd;
    pollfd.events = POLLIN;

    char buff[10240];

    int pollValue = 1;
    while(pollValue > 0){
        pollValue = poll(&pollfd, 1, -1);
        if(pollValue == -1 && errno == EINTR) continue;
        if(pollValue == -1) break;
        n = recv(sockfd, buff, 10240, 0 );
        buff[n] = '\0';
        break;
    }

    cout << buff << endl;

    char num[5];
    num[0] = buff[10];
    num[1] = buff[11];
    num[2] = buff[12];
    num[3] = buff[13];
    num[4] = '\0';
    int number = atoi(num);
    cout << number << endl;

    ss.str("");
    ss << getpid();
    if(send(sockfd, ss.str().c_str(), strlen(ss.str().c_str()), 0) < 0){
        cout << "Send Message Error" << endl;
        exit(0);
    }

    time_t now = time(0);
    strftime(buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", localtime(&now));
    if(send(sockfd, buff, 19, 0) < 0){
        cout << "Send Message Error" << endl;
        exit(0);
    }

    const char * charpool = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const int charNum = 52;
    srand(time(NULL) + getpid());

    for(int i = 0; i < number; i++) buff[i] = charpool[rand()%charNum];
    buff[number] = '\0';
    cout << buff << endl;

    if(send(sockfd, buff, number, 0) < 0){
        cout << "Send Message Error" << endl;
        exit(0);
    }

    close(sockfd);
    exit(0);
}
