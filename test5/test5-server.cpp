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
#include "RSA_AES.h"
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
SafeInfo service[BACKLOG];
int numSet[BACKLOG];

void initDaemon();
void createServer(int portNumber);

FILE * logInfo;

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

int sendStrLen(int sockSvr,const int KeyID){
    int number = 5000 + rand()%5000;
	char sbuff[256];
	sprintf(sbuff, "pidtimestr%d", number);
	unsigned char eninfo[16];
	char deinfo[16];
	int l = service[KeyID].encrypt(sbuff, eninfo, strlen(sbuff));
    if(write(sockSvr, eninfo, l) <= 0){
  	    fprintf(logInfo, "Send Message Error\n");
        exit(0);
    }
	return ((number) / 16 + 1) * 16;
}

void createServer(int portNumber){
	srand(time(0));
    struct timeval timeout;
    timeout.tv_sec = 3;                
    timeout.tv_usec = 0;
	
	ofstream cerr("log.txt");
	ofstream cout("main.pid.txt");
	
    int ret, sinSize, RecvedClient, MaxClient = 500;
    int recvLen = 0;
    fd_set readfds, writefds;
    int sockListen, sockSvr, sockMax;
    struct sockaddr_in client_addr;
    int currentClient = 0;

    int listenfd, connfd;
    sockaddr_in servaddr;
    char buff[20240];
    
    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        cerr << __LINE__ << endl; 
        exit(0);
    }

    int val;
	if ((val = fcntl(listenfd, F_GETFL, 0)) < 0) {
		cerr << __LINE__ << endl; 
		close(listenfd);
		exit(0);
	}
	if (fcntl(listenfd, F_SETFL, val|O_NONBLOCK) < 0) {
		cerr << __LINE__ << endl; 
		close(listenfd);
		exit(0);	
	}
    
    sockListen = listenfd;
	
	int opt = 1;
	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0) {
		cerr << __LINE__ << endl; 
		close(listenfd);
		exit(0);	
	}
	
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(uint16_t(portNumber));
    
    if(bind(listenfd, (sockaddr *)&servaddr, sizeof(servaddr)) == -1){
        cerr << __LINE__ << endl; 
        exit(0);
    }

    if(listen(listenfd, 500) == -1){
        cerr << __LINE__ << endl; 
        exit(0);
    }
//	cerr << __LINE__ << endl; 
    for (int i=0; i < BACKLOG; i++){
		client[i].fd = -1;
		Recved_Num[i] = -1;
    }

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

        //读取数据
        for(int i = 0; i < BACKLOG; i++) {
            if(client[i].fd > 0 && FD_ISSET(client[i].fd, &readfds)) {
				if (Recved_Num[i] == -1) {
					//setRSA(poll_fd, buff, 128, i);
					unsigned char KEY[26];
					read(client[i].fd, KEY, sizeof(KEY));
					unsigned char buff[16]; 
					service[i].serviceKeySet(26, KEY, buff);
					write(client[i].fd, buff, sizeof(buff));
					receiveData[i].maxl = sendStrLen(client[i].fd, i);
					receiveData[i].l = 0;
					++Recved_Num[i];
					continue;
				}

				if (Recved_Num[i] == 0) {
					unsigned char buff[16];
					char pids[16];
            //	cerr << __LINE__ << endl; 
					read(client[i].fd, buff, sizeof(buff));
					service[i].decrypt(buff, pids, sizeof(buff));
					the_mysql_data[i].pid_str = ntohl(*((int *)pids));
					++Recved_Num[i];
            //	cerr << __LINE__ << endl; 
					continue;
				}

				if (Recved_Num[i] == 1) {
					unsigned char buff[32];
					char times[32];
					read(client[i].fd, buff, sizeof(buff));
					service[i].decrypt(buff, times, sizeof(buff));
					times[19] = '\0';
					strcpy(the_mysql_data[i].time_str, times);
					++Recved_Num[i];
					continue;
            //	cerr << __LINE__ << endl; 
				}

				if (Recved_Num[i] == 2) {
					char buff[20000];
            //	cerr << __LINE__ << endl; 
					int readL = read(client[i].fd, receiveData[i].msg + receiveData[i].l, receiveData[i].maxl - receiveData[i].l);
					receiveData[i].l += readL;
					if (receiveData[i].maxl == receiveData[i].l) {
						service[i].decrypt(receiveData[i].msg, buff, receiveData[i].maxl);
						strcpy(the_mysql_data[i].randomString_str, buff);
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
