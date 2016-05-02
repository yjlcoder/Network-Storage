#include <iostream>
#include <bitset>
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
#include <cstdlib>
#include <sys/select.h>
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
#include "sql_mani.h"

#define BACKLOG 600
#define Max_Recved_Num 3

		
class CLIENT{
	public:
		int fd;
		struct sockaddr_in addr;
};

const int RSAPKEYL = 26;
const int pidL = 16;
const int timeL = 32;

struct ReceiveData{
	int l;
	int maxl;
	char msg[20000];
}receiveData[BACKLOG];

//最多处理的connect
//最多处理的connect
CLIENT client[BACKLOG];
Data the_mysql_data[BACKLOG];
int Recved_Num[BACKLOG];
int numSet[BACKLOG];

void initDaemon();
void createServer(int portNumber);

FILE * logInfo;
ofstream mylog("clog.txt");

int main(int argc, char * argv[]){
    if (argc < 2){
        std::cerr << "Usage: test2-server <PortNnumber>" << std::endl;
        exit(0);
    }

    signal(SIGCHLD, SIG_IGN);
    
    int portNumber = atoi(argv[1]);
    
	pid_t pid = fork();
    if(pid < 0){
        std::cerr << "Fork Error" << std::endl;
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

int sendStrLen(int sockSvr,ofstream & cerr){
    int number = 5000 + rand()%5000;
	char sbuff[256];
	sprintf(sbuff, "pidtimestr%d", number);
    if(write(sockSvr, sbuff, strlen(sbuff)) <= 0){
  	    mylog << "Send Message Error" << endl;
        exit(0);
    }
	cerr << sbuff << endl;
	return number;
}

void createServer(int portNumber){
	srand(time(0));
    struct timeval timeout;
    timeout.tv_sec = 3;                
    timeout.tv_usec = 0;
	
	ofstream cerr("log.txt");
	ofstream cout("main.pid.txt");
	
    //cerr << __LINE__ << endl; 
    int ret, sinSize, RecvedClient, MaxClient = 500;
    int recvLen = 0;
    fd_set readfds, writefds;
    int sockListen, sockSvr, sockMax;
    struct sockaddr_in client_addr;
    int currentClient = 0;

    int listenfd, connfd;
    sockaddr_in servaddr;
    char buff[20240];
    
    //    cerr << __LINE__ << endl; 
    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        cerr << __LINE__ << endl; 
        exit(0);
    }

     //   cerr << __LINE__ << endl; 
    int val;
	if ((val = fcntl(listenfd, F_GETFL, 0)) < 0) {
		cerr << __LINE__ << endl; 
		close(listenfd);
		exit(0);
	}
    //    cerr << __LINE__ << endl; 
	if (fcntl(listenfd, F_SETFL, val|O_NONBLOCK) < 0) {
		cerr << __LINE__ << endl; 
		close(listenfd);
		exit(0);	
	}
    
       // cerr << __LINE__ << endl; 
    sockListen = listenfd;
	
     //   cerr << __LINE__ << endl; 
	int opt = 1;
	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0) {
		cerr << __LINE__ << endl; 
		close(listenfd);
		exit(0);	
	}
	
     //   cerr << __LINE__ << endl; 
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(uint16_t(portNumber));
    
     //   cerr << __LINE__ << endl; 
    if(bind(listenfd, (sockaddr *)&servaddr, sizeof(servaddr)) == -1){
        cerr << __LINE__ << endl; 
        exit(0);
    }

      //  cerr << __LINE__ << endl; 
    if(listen(listenfd, 500) == -1){
        cerr << __LINE__ << endl; 
        exit(0);
    }
//	cerr << __LINE__ << endl; 
      //  cerr << __LINE__ << endl; 
    for (int i=0; i < BACKLOG; i++){
		client[i].fd = -1;
		Recved_Num[i] = -1;
    }

    //    cerr << __LINE__ << endl; 
    RecvedClient = 0;
	
	while (RecvedClient < MaxClient) {
		FD_ZERO(&readfds);
		FD_SET(sockListen, &readfds);
		for (int i = 0; i < currentClient; ++i) {
			if (client[i].fd > 0)
				FD_SET(client[i].fd, &readfds);
			if (client[i].fd > sockMax) sockMax = client[i].fd;
		}

        //select
       // cerr << sockMax << endl;
        ret = select((int)sockMax+1, &readfds, NULL, NULL, &timeout);
        if(ret < 0) {
            cerr << __LINE__ << endl; 
            break;
        } else if(ret == 0) {
            continue;
        }

		if(FD_ISSET(sockListen, &readfds)) {
            sockSvr = accept(sockListen, NULL, NULL);
		//	cerr << sockSvr << endl;
			sockMax = (sockSvr > sockMax)? sockSvr : sockMax;
            if(sockSvr == -1) {
            	cerr << __LINE__ << endl; 
            }
			client[currentClient].fd = sockSvr; 
			client[currentClient++].addr = client_addr;
        }
		
		for (int i = 0; i < BACKLOG; i++) {
			if(client[i].fd > 0) { 
				if (Recved_Num[i] == -1) {
					receiveData[i].maxl = sendStrLen(client[i].fd, cerr);
					++Recved_Num[i];
				}
			}
		}

        //    cerr << __LINE__ << endl; 

        //读取数据
        for(int i = 0; i < BACKLOG; i++) {
            if(client[i].fd > 0 && FD_ISSET(client[i].fd, &readfds)) {
				if (Recved_Num[i] == 0) {
					char pids[16];
					read(client[i].fd, pids, 4);
					the_mysql_data[i].pid_str = ntohl(*((int *)pids));
					++Recved_Num[i];
            //	cerr << __LINE__ << endl; 
				}

				if (Recved_Num[i] == 1) {
					char times[32];
					read(client[i].fd, buff, 19);
					times[19] = '\0';
					strcpy(the_mysql_data[i].time_str, times);
					++Recved_Num[i];
            //	cerr << __LINE__ << endl; 
				}

				if (Recved_Num[i] == 2) {
					char buff[20000];
					int readL = read(client[i].fd, receiveData[i].msg + receiveData[i].l, receiveData[i].maxl - receiveData[i].l);
					receiveData[i].l += readL;
					if (receiveData[i].maxl == receiveData[i].l) {
						receiveData[i].msg[receiveData[i].maxl] = '\0';
						strcpy(the_mysql_data[i].randomString_str, receiveData[i].msg);
						++Recved_Num[i];
						++RecvedClient;
						close(client[i].fd);
						client[i].fd = -1;
            //	cerr << __LINE__ << endl; 
					}
				}
			//	cerr << "over : " << RecvedClient << endl;
            }
        }
        
    } 
	
//	cerr << "sql_mani_begin" << endl;
	put_to_mysql(the_mysql_data, MaxClient);
	//cerr << "sql_mani_end" << endl;
  //  for (int i = 0; i < MaxClient; i ++) {
	//	cout << the_mysql_data[i].pid_str << ' ' << the_mysql_data[i].time_str << endl;
	//	cout << the_mysql_data[i].randomString_str << endl;	
	//}

    close(sockListen);
}
