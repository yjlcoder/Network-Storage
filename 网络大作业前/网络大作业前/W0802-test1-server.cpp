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
#include <mysql/mysql.h>
#include <signal.h>
#include <sys/wait.h>
using namespace std;

class Data{
    public:
		int pid;
		char time[30];
		char str[100];
};
void initDaemon(){
    setsid();
    umask(0);
    for(int i = 0; i < NOFILE; i++){
        close(i);
    }
}
//将三项信息存入到数据库当中
void put_to_mysql(Data mysql_data)
{
	int i;
	MYSQL *mysql;
	MYSQL_RES *result;
	MYSQL_ROW row;

	if ((mysql = mysql_init(NULL)) == NULL){
		cout << "mysql init error" << endl;
		exit(0);
	}

	if (mysql_real_connect(mysql,"localhost","root","root123","test",0,NULL,0) == NULL){
		cout << "mysql real connect error" << endl;
		exit(0);
	}
	
	char buff[4096];
	sprintf(buff, "INSERT INTO dbtest1(dbtest_pid, dbtest_time, dbtest_str) VALUES(%d, '%s', '%s');", mysql_data.pid, mysql_data.time, mysql_data.str);
	mysql_query(mysql, buff);
	mysql_close(mysql);
	return;
}
//服务端进程的相关操作
void createServer(char* IP, int portNumber){
    int listenfd, connfd;
    sockaddr_in servaddr;
    ofstream cout("/root/homework/Network_Homework/output_server");
    char buff[4096];
    Data mysql_data;
    int n, i;
    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        cout << "Create Socket Failed" << endl;
        exit(0);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(uint16_t(portNumber));
    
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
		mysql_data.pid = atoi(buff);   //将收到的pid转化为数据库的类型
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
		//mysql_data.time = trans_time_t(buff); //将收到的time转化为数据库的类型
		for (i = 0; i < 30; i ++)
			mysql_data.time[i] = buff[i];
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
		for (i = 0; i < 100; i ++)
			mysql_data.str[i] = buff[i];  //将收到的字符串转化为数据库的类型
		//发送end信号给服务端
		ss.clear();
		ss.str("");
		ss << "end";
		if (send(connfd, ss.str().c_str(), strlen(ss.str().c_str()), 0) < 0)
		{
			cout << "Send str Error" << strerror(errno) << errno << endl;
			exit(0);
		}
		
		while (recv(connfd, buff, 4096, 0) != 0);

		close(connfd);
		put_to_mysql(mysql_data);		
		exit(0);
	}
    }
    close(listenfd);
}

int main(int argc, char * argv[]){
    signal(SIGCHLD, SIG_IGN); //设置SIGCHLD信号的跳转

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
