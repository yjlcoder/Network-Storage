#include <iostream>
#include <bitset>
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
#include <cstdlib>
#include <errno.h>
#include <cstring>
#include <cstdlib>
#include <openssl/rsa.h>
#include <openssl/aes.h>
#include <unistd.h>
#include <memory.h>
#include <cerrno>
#include <cstdio>
#include <ctime>
#include <iostream>

using namespace std;
void initDaemon();
void createClient(int i, char * ipaddr, int PortNumber);


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

  //  createClient(0, ipaddr, portNumber);
   // return 0;

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
        for(int i = 0; i < 500; i++){
            pid = fork();
            if(pid < 0){
                perror("Fork Error");
                exit(0);
            } else if (pid > 0){
                continue;
            } else if (pid == 0){
                createClient(0, ipaddr, portNumber);
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

char fileName[256];
string time_str, msg_str;

void createClient(int i, char* ipaddr, int PortNumber){
    pid_t mypid = getpid();
	stringstream ss("");
    ss << mypid << ".pid.txt";
    ofstream cout(ss.str().c_str());
	
	int sockfd, n;
    char recvLine[4096], sendLine[4096];
    sockaddr_in servaddr;


    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        cout << "Create Socket ERROR" << endl;
        exit(0);
    }

	int val;
	if ((val = fcntl(sockfd, F_GETFL, 0)) < 0) {
		cout << __LINE__ << endl; 
		close(sockfd);
		exit(0);
	}
	if (fcntl(sockfd, F_SETFL, val|O_NONBLOCK) < 0) {
		cout << __LINE__ << endl; 
		close(sockfd);
		exit(0);	
	}

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PortNumber);
    if(inet_pton(AF_INET, ipaddr, &servaddr.sin_addr) <= 0){
        cout << "Inet_pton ERROR" << endl;
        exit(0);
    }

	int times = 100;
	int flag = 0;
	int sel = 1;
    //while ((flag = connect(sockfd, (sockaddr*)&servaddr, sizeof(servaddr))) < 0 && (times-- >0));
	while (times-- && flag <= 0) {	
		flag = connect(sockfd, (sockaddr*)&servaddr, sizeof(servaddr));
		sel = 1;
		if (flag < 0){
			fd_set wfd;
			struct timeval tm;
			FD_ZERO(&wfd);
			FD_SET(sockfd, &wfd);
			tm.tv_sec = 0;
			tm.tv_usec = 500 * 1000;
			sel = select (sockfd + 1, NULL, &wfd, NULL, &tm);
			if (sel <= 0) {
				continue;
			}
			break;
		}
	}

	if (sel <= 0) {
		cout << "Connect Error" << endl;
		exit(0);
	}

    pollfd pollfd;
    pollfd.fd = sockfd;
    pollfd.events = POLLIN;


    char buff[10240];

	while (read(sockfd, buff, 14) <= 0);
	buff[14] = '\0';
    char num[5];
    num[0] = buff[10];
    num[1] = buff[11];
    num[2] = buff[12];
    num[3] = buff[13];
    num[4] = '\0';
    int number = atoi(num);

	char pids[256];
    *((int *)pids) = htonl(mypid);
    if(send(sockfd, pids, 4, 0) < 0){
        cout << "Send Message Error" << endl;
        exit(0);
    }
    
	time_t now = time(0);
    strftime(buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", localtime(&now));
//	cout << Len << endl;
    buff[19] = '\0';
	time_str = buff;
	int sendL = 0;
    if((sendL = send(sockfd, buff, 19, 0)) < 0){
        cout << "Send Message Error" << endl;
        exit(0);
    }
    //free(eninfo);
    const char charpool[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const int charNum = sizeof(charpool)/sizeof(char) - 1;
    srand(time(NULL) + getpid());

    for(int i = 0; i < number; i++) buff[i] = charpool[rand()%charNum];
    if(send(sockfd, buff, number, 0) < 0){
        cout << "Send Message Error" << endl;
        exit(0);
    }
	buff[number] = '\0';
	msg_str = buff;
    //free(eninfo);

    int pollValue = 1;

	//std::cout << mypid << endl << time_str << endl << randomString_str << endl;

    while(pollValue > 0){

        pollValue = poll(&pollfd, 1, -1);
     
	 	if(pollValue == -1 && errno == EINTR) continue;
        if(pollValue == -1) break;
        n = recv(sockfd, buff, 10240, 0 );
        buff[n] = '\0';
        break;
    }

    cout << mypid << endl << time_str << endl << msg_str << endl;

    close(sockfd);
    exit(0);
}

