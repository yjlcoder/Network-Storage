//
// Created by liuyang on 16-4-21.
//

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
#include <stdlib.h>
#include <string.h>
#include <string>
#include <errno.h>
#include <time.h>
using namespace std;
//ofstream cout("/root/homework/Network_Homework/output_client");

void initDaemon(){
    setsid();
    umask(0);
    for(int i = 0; i < NOFILE; i++){
        close(i);
    }
}
//结束socket和退出进程
void Finish_Client(int sockfd)
{
    close(sockfd);
	exit(0);
}
//产生一个长度为len的字符串，并返回到buff里面
void make_rand_str(char s[], int len)
{
	srand(time(0));
	int i, big_small, alpha;
	for (i = 0; i < len; i ++)
	{
		big_small = rand()%2;
		alpha = rand()%26;
		if (big_small == 0)
			s[i] = 'a' + alpha;
		else s[i] = 'A' + alpha;
	}
	s[len] = '\0';
	return;
}
void createClient(int i, char* ipaddr, int PortNumber){
    char buff[4096], s_pid[4096], s_time[4096], s_str[4096];
    ofstream cout("/root/homework/Network_Homework/output_client");
    int sockfd, n;
    char recvLine[4096], sendLine[4096];
    sockaddr_in servaddr;
    pid_t pid;
   //生成子进程对服务端进行链接，父进程直接结束函数调用
    pid = fork();
    if (pid == -1)
	perror("fork new process error\n");
    else if (pid > 0)
	return;
    else if (pid == 0)
    {
	initDaemon();
    	ofstream cout("/root/homework/Network_Homework/output_client");
    }
//    else if (pid == 0)
//	exit(0);
    
    
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        cout << "Create Socket ERROR" << strerror(errno) << errno << endl;
        exit(0);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PortNumber);
    if(inet_pton(AF_INET, ipaddr, &servaddr.sin_addr) <= 0){
        cout << "Inet_pton ERROR" << strerror(errno) << errno << endl;
        exit(0);
    }

    if(connect(sockfd, (sockaddr*)&servaddr, sizeof(servaddr)) < 0){
        cout << "Connect Error" << strerror(errno) << errno <<endl;
        exit(0);
    }

    ofstream file2("/root/homework/Network_Homework/output_client1");
    stringstream ss;
    string st;
   
    //接收服务端的pid，然后返回进程的pid值给服务端 
    n = recv(sockfd, buff, 4096, 0);
    buff[n] = '\0';
    st = buff;
    if (n <= 0)
	Finish_Client(sockfd);
    if (st != "pid")
	Finish_Client(sockfd);
    pid = getpid();
    ss << pid;
    ss >> s_pid;
    if (send(sockfd, ss.str().c_str(), strlen(ss.str().c_str()), 0) < 0)
    {
	cout << "Send pid Error" << strerror(errno) << errno << endl;
	exit(0);
    }

    //接收服务端的time，然后返回进程的time值给服务端 
    n = recv(sockfd, buff, 4096, 0);
    buff[n] = '\0';
    st = buff;
    if (n <= 0)
	Finish_Client(sockfd);
    if (st != "time")
	Finish_Client(sockfd);

    time_t t = time(0);
    strftime(buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", localtime(&t));
    for (i = 0; i <= strlen(buff); i ++)
	s_time[i] = buff[i];
    if (send(sockfd, buff, strlen(buff), 0) < 0)
    {
	cout << "Send time Error" << strerror(errno) << errno << endl;
	exit(0);
    }

    //接收服务端的str，然后返回一个随机字符串给服务端 
    n = recv(sockfd, buff, 4096, 0);
    buff[n] = '\0';
    st = buff;
    if (n <= 0)
	Finish_Client(sockfd);
    if (st.substr(0,3) != "str")
	Finish_Client(sockfd);

    int len = atoi(st.substr(3, 2).c_str());
    make_rand_str(buff, len);
    for (i = 0; i <= strlen(buff); i ++)
	s_str[i] = buff[i];
    if (send(sockfd, buff, strlen(buff), 0) < 0)
    {
	cout << "Send str Error" << strerror(errno) << errno << endl;
	exit(0);
    }

    //接收到服务器的end信号，关闭socket并将内容写入文件中
    n = recv(sockfd, buff, 4096, 0);
    buff[n] = '\0';
    st = buff;
    if (n <= 0)
	Finish_Client(sockfd);
    if (st.substr(0,3) != "end")
	Finish_Client(sockfd);
    close(sockfd);

    ss.str("");
    ss.clear();
    ss << "/root/homework/Network_Homework/output_client_file/"; //文件夹放入的路径
    ss << s_pid;
    ss << ".pid.txt";
    ss >> buff;
    ofstream file3(buff);
    file3 << "pid:" << s_pid << endl;
    file3 << "time:" << s_time << endl;
    file3 << "str:" << s_str << endl;
    exit(0);
}

int main(int argc, char * argv[]){
    ofstream file1("/root/homework/Network_Homework/output_client_count");
    if(argc < 3){
        cout << "Usage: test1-client <IP_ADDR> <PORT_NUMBER>" << endl;
        exit(0);
    }
    file1 << "test" << endl;
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
	int MAX_k = 5;
	initDaemon();
	//生成500个子进程去链接服务端
	for (int i = 0; i < MAX_k; i ++)
	{
        	createClient(i, ipaddr, portNumber);
	}
    }
    return 0;
}