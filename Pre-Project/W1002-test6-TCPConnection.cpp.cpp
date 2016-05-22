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
#include <unistd.h>
#include <memory.h>
#include <cerrno>
#include <cstdio>
#include <ctime>
#include <list>
#include <string>
#include <arpa/inet.h>
#include <vector>

using namespace std;

struct TCP_NAT{
    struct sockaddr_in sockInAddr, sockOutAddr;
    int sockInfd;
	int closed;
	int inPort, outPort;
	int totIn, totOut;
	string outAddr;
	string inAddr;
	struct timeval timeout;

	TCP_NAT(){
		timeout.tv_sec = 2;
		timeout.tv_usec = 500*1000;
		inAddr = "0.0.0.0";
		inPort = 0;
		outAddr = "0.0.0.0";
		outPort = 0;
		closed = 0;
		totIn = 0;
		totOut = 0;
		
	}
	
	TCP_NAT(string iAm, int inP, string oAm, int outP){
		timeout.tv_sec = 2;
		timeout.tv_usec = 500*1000;
		inAddr = iAm;
		inPort = inP;
		outAddr = oAm;
		outPort = outP;
		closed = 0;
		totIn = 0;
		totOut = 0;
	}

	int build(){
		if((sockInfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
			cerr << __LINE__ << endl; 
			return -1;
		}
		int val;
		if ((val = fcntl(sockInfd, F_GETFL, 0)) < 0) {
			cerr << __LINE__ << endl; 
			close(sockInfd);
			return -1;
		}
		if (fcntl(sockInfd, F_SETFL, val|O_NONBLOCK) < 0) {
			cerr << __LINE__ << endl; 
			close(sockInfd);
			return -1;	
		}
		
		int opt = 1;
		if (setsockopt(sockInfd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0) {
			cerr << __LINE__ << endl;
			close(sockInfd);
			return -1;
		}
		
		sockInAddr.sin_family = AF_INET;
		sockInAddr.sin_port = htons(uint16_t(inPort));
		if(inet_pton(AF_INET, inAddr.c_str(), &sockInAddr.sin_addr) <= 0){
			clog << "Inet_pton ERROR" << endl;
			close(sockInfd);			
			return -1;
		}
		
		if(bind(sockInfd, (sockaddr *)&sockInAddr, sizeof(sockInAddr)) == -1){
			clog << __LINE__ << endl; 
			return -1;
		}
		if(listen(sockInfd, 100) == -1){
			clog << __LINE__ << endl; 
			return -1;
		}
		return 0;
	}

	int newConnect(){
		ofstream clog("log.txt", ofstream::app);
		int sockfd;
		if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
			clog << "Create Socket ERROR" << endl;
			return -1;
		}

		int val;
		if ((val = fcntl(sockfd, F_GETFL, 0)) < 0) {
			clog << __LINE__ << endl; 
			close(sockfd);
			return -1;
		}
		if (fcntl(sockfd, F_SETFL, val|O_NONBLOCK) < 0) {
			clog << __LINE__ << endl; 
			close(sockfd);
			return -1;
		}

		memset(&sockOutAddr, 0, sizeof(sockOutAddr));
		sockOutAddr.sin_family = AF_INET;
		sockOutAddr.sin_port = htons(outPort);
		if(inet_pton(AF_INET, outAddr.c_str(), &sockOutAddr.sin_addr) <= 0){
			clog << "Inet_pton ERROR" << endl;
			close(sockfd);
			return -1;
		}

		if (connect(sockfd, (sockaddr*)&sockOutAddr, sizeof(sockOutAddr)) <= 0) {
			fd_set wfd;
			FD_ZERO(&wfd);
			FD_SET(sockfd, &wfd);
			struct timeval timeout;
			timeout.tv_sec = 1;
			timeout.tv_usec = 500 * 1000;
			if (select (sockfd + 1, NULL, &wfd, NULL, &timeout) <= 0) {
				clog << "Connect Error" << endl;
				close(sockfd);
				return -1;
			}
		}
		clog.close();
		return sockfd;
	}

	int singleT(int sockCli, char * inAddr, int inPort){
		ofstream clog("log.txt", ofstream::app);
		int sockSvr = newConnect();
		if (sockSvr > 0) {
			char buff[32];
			time_t now = time(0);
			strftime(buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", localtime(&now));
			clog << "new connection in(" << buff << "): " << endl
				<< "    " << inAddr << ":" << ntohs(inPort) << " -> " 
				<<	outAddr << ":" << outPort << endl << flush;
		} else {
			if (sockCli >= 0) close(sockCli);
			if (sockSvr >= 0) close(sockSvr);
			return -1;
		}
	
		int infd = sockCli, outfd = sockSvr, inL = 0, outL = 0;
		char buff[10240];
		time_t outLastT = time(0), inLastT = time(0);
		while (1) {
			fd_set readfds;
			FD_ZERO(&readfds);
			FD_SET(infd, &readfds);
			FD_SET(outfd, &readfds);
			int sockMax = max(infd, outfd);
			timeout.tv_sec = 10;                
			timeout.tv_usec = 500 * 1000;
			int ret = select((int)sockMax+1, &readfds, NULL, NULL, &timeout);
			
			if (ret == 0) continue;
			else if (ret < 0) {
				clog << "select error : errno = " << errno << endl;
				close(infd); 
				close(outfd);
				return -1;
			}
			
			int Len1 = -1, Len2 = -1;

			if (FD_ISSET(infd, &readfds)) {
				inLastT = time(0);
				Len1 = read(infd, buff, 10240);
			}

			if (FD_ISSET(outfd, &readfds)) {
				outLastT = time(0);
				Len2 = read(outfd, buff, 10240);
			}

			if (Len1 > 0 && Len2 != 0) write(outfd, buff, Len1);
			if (Len2 > 0 && Len1 != 0) write(infd, buff, Len2);

			if (Len1 > 0) inL += Len1; 
			if (Len2 > 0) outL += Len2;
			if (Len1 == 0 || Len2 == 0) {
				char buff[32];
				time_t now = time(0);
				strftime(buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", localtime(&now));
				
				clog << "connection end in(" << buff << "): " << endl 
					<< "    " << inAddr << ":" << ntohs(inPort) << " -> " 
					<<	outAddr << ":" << outPort << " " << inL << "Byte" << endl
					<< "    " << outAddr << ":" << outPort << " -> "
					<< inAddr<< ":" << ntohs(inPort) << " " << outL << "Byte" << endl << flush;	
				
				close(infd);
				close(outfd);
				exit(0);
			}
		}
    }

	int tranfom(){
		ofstream clog("log.txt", ofstream::app);
		fd_set readfds;
		FD_ZERO(&readfds);
		FD_SET(sockInfd, &readfds);
		int sockMax = sockInfd;
	
    	timeout.tv_sec = 10;                
    	timeout.tv_usec = 500 * 1000;
		
		int ret = select((int)sockMax+1, &readfds, NULL, NULL, &timeout);
        if(ret == 0) {
			return 0;
        }else {
			if (ret < 0) clog << "select error : " << errno << ", code in line" << __LINE__ << endl;
		}
		
		if(FD_ISSET(sockInfd, &readfds)) {
			struct sockaddr_in client_addr;
			unsigned int sin_size = sizeof(client_addr);
			int sockCli = accept (sockInfd, ( struct sockaddr * ) & client_addr, & sin_size);
			if (sockCli <0 ) {
				clog << "accept error" << endl;
			}
			char IPdotdec[32];
			inet_ntop(AF_INET, (void *)&client_addr.sin_addr, IPdotdec, 32);
			int pid = fork();
			if (pid == 0) {
				return singleT(sockCli, IPdotdec, client_addr.sin_port);
			} else {
				close(sockCli);
			}
        }
		clog.close();
		return 0;
	}

	int TCP_close(){
		if (!closed) {
			clog << "close sockInfd" << endl;
			close(sockInfd);
		}
		closed = 1;
		return 0;
	};

	~TCP_NAT(){
		if (!closed) {
			close(sockInfd);
		}
	}
};


TCP_NAT test;

void exitSocket(int sig){
	ofstream clog("log.txt", ofstream::app);
	clog << "结束  " << test.inAddr << ":" << test.inPort
		<< "到" << test.outAddr << ":" << test.outPort << "的映射" << endl << flush;
	clog.close();
	test.TCP_close();
	exit(0);
}

int work(char * iD, int iP, char * oD, int oP){
	ofstream clog("log.txt", ofstream::app);
	signal(SIGQUIT, exitSocket);
	test.inAddr = iD;
	test.inPort = iP;
	test.outPort = oP;
	test.outAddr = oD;
	if (test.build()!=0) exit(0);

	clog << "开始将" << test.inAddr << ":" << test.inPort
		<< "映射至" << test.outAddr << ":" << test.outPort << endl << flush;
	clog.close();

	while (1) {
		test.tranfom();
	}	
	return 0;
}

vector<unsigned int> pids;
void exitMain(int sig){
	for (int i = 0; i < pids.size(); ++i){
		kill(pids[i], SIGQUIT);
	}
	exit(0);
}

int createServer(int argv, char * argc[]){

//按照监听IP，监听端口，目标IP，目标端口的顺序
	//
	//TCP_NAT test(7999, "192.168.80.235", 7962);
//	int a[4] = {7990, 7991, 7992, 7993};
//	int b[4] = {80, 22, 80, 7963};
//	char addr[] =  "192.168.80.235";

	for (int i = 1; i < argv; i += 4) {
		unsigned int cpid = fork();
		if (cpid == 0){
			int inP = atoi(argc[i + 1]);
			int outP = atoi(argc[i + 3]);
			work(argc[i], inP, argc[i + 2], outP);	
		} else {
			pids.push_back(cpid);
		}
	}
	signal(SIGQUIT, exitMain);
	while (1);
	return 0;
}

void initDaemon(){
    setsid();
    umask(0);
    for(int i = 0; i < NOFILE; i++){
        close(i);
    }
}


pid_t pid;
void exitFather(int sig){
	kill(3, pid);
	exit(0);
}


int main(int argc, char * argv[]){
    if (argc < 2){
        std::cerr << "Usage: test2-server <PortNnumber>" << std::endl;
        exit(0);
    }

    signal(SIGCHLD, SIG_IGN);
    
    pid = fork();
    if(pid < 0){
        std::cerr << "Fork Error" << std::endl;
        exit(0);
    } else if (pid > 0){
        //Parent Process
        exit(0);
    } else if (pid == 0){
        initDaemon();
        // Now is a Daemon Process
        createServer(argc, argv);
    }
}


