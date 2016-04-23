//
// Created by liuyang on 16-4-21.
//

#include <zconf.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <string>
#include <string.h>
#include <cstdio>
#include <stdlib.h>
#include <iostream>
#include <netinet/in.h>
#include <cstring>
#include <fstream>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sstream>
using namespace std;

void initDaemon(){
    setsid();
    umask(0);
    for(int i = 0; i < NOFILE; i++){
        close(i);
    }
}
void createServer(char* IP, int portNumber){
    int listenfd, connfd;
    sockaddr_in servaddr;
    ofstream cout("/root/homework/Network_Homework/output_server");
    cout << "test" << endl;
    char buff[4096];
    int n;
    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        cout << "Create Socket Failed" << endl;
        exit(0);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
 //   servaddr.sin_addr.s_addr = inet_addr(IP);
    servaddr.sin_port = htons(uint16_t(portNumber));
    
//    socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1);
    if(bind(listenfd, (sockaddr *)&servaddr, sizeof(servaddr)) == -1){
        cout << "Bind Socket Error:" << strerror(errno) << "errno:" << errno << endl;
        exit(0);
    }

    if(listen(listenfd, 100) == -1){
        cout << "Listen Socket Error:" << strerror(errno) << "errno:" << errno << endl;
        exit(0);
    }

    while(1){
        if((connfd = accept(listenfd, (sockaddr*) NULL, NULL)) == -1){
            cout << "Accept Socket Error" << endl;
            continue;
        }
    //当收到链接请求时，就fork一个子进程，使用子进程进行通信
	pid_t pid = fork();
	if (pid == -1)
		perror("fork new process error\n");
	else if (pid > 0){
		//Parent Process
		continue;
		close(connfd);
	}
	else {
		//Child Process
        	cout << "Connection Built" << endl;
		stringstream ss;
		
		//发送pid，并接收子进程的pid值
		ss << "pid";
		if (send(connfd, ss.str().c_str(), strlen(ss.str().c_str()), 0) < 0)
		{
			cout << "Send pid Error" << strerror(errno) << errno << endl;
			exit(0);
		}
		n = recv(connfd, buff, 4096, 0);
		buff[n] = '\n';
		buff[n+1] = '\0';
		cout << "Recieve: " << buff << std::flush;
		
		//发送time，并接收子进程的time值
		
		ss.clear();
		ss.str("");
		ss << "time";
		if (send(connfd, ss.str().c_str(), strlen(ss.str().c_str()), 0) < 0)
		{
			cout << "Send time Error" << strerror(errno) << errno << endl;
			exit(0);
		}
		n = recv(connfd, buff, 4096, 0);
		buff[n] = '\n';
		buff[n+1] = '\0';
		cout << "Recieve: " << buff << std::flush;

		//发送str，并接收子进程返回的随机字符串
		
		ss.clear();
		ss.str("");
		ss << "str";
		srand(time(0));
		int len = 50 + rand()%50;
		ss << len;
		if (send(connfd, ss.str().c_str(), strlen(ss.str().c_str()), 0) < 0)
		{
			cout << "Send str Error" << strerror(errno) << errno << endl;
			exit(0);
		}
		n = recv(connfd, buff, 4096, 0);
		buff[n] = '\n';
		buff[n+1] = '\0';
		cout << "Recieve: " << buff << std::flush;
		
		//发送end信号给服务端
		ss.clear();
		ss.str("");
		ss << "end";
		if (send(connfd, ss.str().c_str(), strlen(ss.str().c_str()), 0) < 0)
		{
			cout << "Send str Error" << strerror(errno) << errno << endl;
			exit(0);
		}

		close(connfd);		
		exit(0);
	}
    }
    close(listenfd);
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
        createServer(argv[0], portNumber);
    }
    return 0;
}