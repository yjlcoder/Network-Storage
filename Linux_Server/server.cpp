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

#define PATHSIZE 1024
#define NAMESIZE 256
#define PSSWSIZE 256
#define BUFFSIZE 2048
#define FAILURE -1

using namespace std;

int writeLog(const char * message){
	ofstream log("log.txt", ofstream::app);
	char time_str[32];
	time_t now = time(0);
	strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&now));
	log << time_str << ' ' << message << std::endl;
	log.close();
	return 0;
}

int regist(int sockfd){
	unsigned char NLen, PLen;
	char name[NAMESIZE], password[PSSWSIZE];
	long L;
	L = recv(sockfd, &NLen, sizeof(NLen), MSG_WAITALL);
	if (L != sizeof(NLen)) return FAILURE;
	L = recv(sockfd, name, NLen, MSG_WAITALL);
	if (L != NLen) return FAILURE;
	L = recv(sockfd, &PLen, sizeof(PLen), MSG_WAITALL);
	if (L != sizeof(PLen)) return FAILURE;
	L = recv(sockfd, password, PLen, MSG_WAITALL);
	if (L != PLen) return FAILURE;

	char flag;
	//flag = sql();
	
	L = send(sockfd, &flag, sizeof(flag), 0);
	if (L != sizeof(flag)) return FAILURE;
	return 0;
}

int logIn(int sockfd){
	unsigned char NLen, PLen;
	char name[NAMESIZE], password[PSSWSIZE];
	long UID, L;
	L = recv(sockfd, &NLen, sizeof(NLen), MSG_WAITALL);
	if (L != sizeof(NLen)) return FAILURE;
	L = recv(sockfd, name, NLen, MSG_WAITALL);
	if (L != NLen) return FAILURE;
	L = recv(sockfd, &PLen, sizeof(PLen), MSG_WAITALL);
	if (L != sizeof(PLen)) return FAILURE;
	L = recv(sockfd, password, PLen, MSG_WAITALL);
	if (L != PLen) return FAILURE;
	
	//UID = sql();
	
	UID = htonl(UID);
	L = send(sockfd, &UID, sizeof(UID), 0);
	if (L != sizeof(UID)) return FAILURE;
	return 0;
}

int getFileList(int sockfd){
	long Len, UID, L;
	char path[PATHSIZE];
	L = recv(sockfd, &UID, sizeof(Len), MSG_WAITALL);
	if (L != sizeof(Len)) return FAILURE;
	UID = ntohl(UID);
	L = recv(sockfd, &Len, sizeof(UID), MSG_WAITALL);
	if (L != sizeof(UID)) return FAILURE;
	Len = ntohl(Len);
	L = recv(sockfd, path, Len, MSG_WAITALL);
	if (L != Len) return FAILURE;

	vector<string> fileList;

	
	//fileList = sql();
	
	
	unsigned int num = htonl(fileList.size());
	L = send(sockfd, &num, sizeof(num), 0);
	if (L != sizeof(num)) return FAILURE;

	for (int i = 0; i < fileList.size(); ++i) {
		int size = htonl(fileList[i].length());
		L = send(sockfd, &size, sizeof(size), 0);
		if (L != sizeof(size)) return FAILURE;
		L = send(sockfd, fileList[i].c_str(), fileList[i].length(), 0);
		if (L != fileList[i].length()) return FAILURE;
	}

	return 0;
}

int addFile(int sockfd){
	return 0;
}

int delFile(int sockfd){
	long Len, UID, L;
	char path[PATHSIZE];
	L = recv(sockfd, &UID, sizeof(Len), MSG_WAITALL);
	if (L != sizeof(Len)) return FAILURE;
	UID = ntohl(UID);
	L = recv(sockfd, &Len, sizeof(UID), MSG_WAITALL);
	if (L != sizeof(UID)) return FAILURE;
	Len = ntohl(Len);
	L = recv(sockfd, path, Len, MSG_WAITALL);
	if (L != Len) return FAILURE;

	char flag;
	//flag = sql();
	
	L = send(sockfd, &flag, sizeof(flag), 0);
	if (L != sizeof(flag)) return FAILURE;

	return 0;
}

int moveFile(int sockfd){
	int SLen, DLen, UID, L;
	char SPath[PATHSIZE], DPath[PATHSIZE];
	L = recv(sockfd, &UID, sizeof(UID), MSG_WAITALL);
	if (L != sizeof(UID)) return FAILURE;
	UID = ntohl(UID);

	L = recv(sockfd, &SLen, sizeof(SLen), MSG_WAITALL);
	if (L != sizeof(SLen)) return FAILURE;
	SLen = ntohl(SLen);
	
	L = recv(sockfd, SPath, SLen, MSG_WAITALL);
	if (L != SLen) return FAILURE;
	
	L = recv(sockfd, &DLen, sizeof(DLen), MSG_WAITALL);
	if (L != sizeof(DLen)) return FAILURE;
	DLen = ntohl(DLen);
	
	L = recv(sockfd, DPath, DLen, MSG_WAITALL);
	if (L != DLen) return FAILURE;
	
	char flag;
	//flag = sql();
	
	
	L = write(sockfd, &flag, sizeof(flag));
	if (L != sizeof(flag)) return FAILURE;
	return 0;
}

int copyFile(int sockfd){
	int SLen, DLen, UID, L;
	char SPath[PATHSIZE], DPath[PATHSIZE];
	L = recv(sockfd, &UID, sizeof(UID), MSG_WAITALL);
	if (L != sizeof(UID)) return FAILURE;
	UID = ntohl(UID);

	L = recv(sockfd, &SLen, sizeof(SLen), MSG_WAITALL);
	if (L != sizeof(SLen)) return FAILURE;
	SLen = ntohl(SLen);
	
	L = recv(sockfd, SPath, SLen, MSG_WAITALL);
	if (L != SLen) return FAILURE;
	
	L = recv(sockfd, &DLen, sizeof(DLen), MSG_WAITALL);
	if (L != sizeof(DLen)) return FAILURE;
	DLen = ntohl(DLen);
	
	L = recv(sockfd, DPath, DLen, MSG_WAITALL);
	if (L != DLen) return FAILURE;
	
	char flag;
	//flag = sql();
	
	
	L = write(sockfd, &flag, sizeof(flag));
	if (L != sizeof(flag)) return FAILURE;
	return 0;
}

int downLoad(int sockfd){

}

int exec(int sockfd){
	char mark;
	int L = recv(sockfd, &mark, sizeof(mark), MSG_WAITALL);
	if (L <= 0) {
		writeLog(strerror(errno));
		return FAILURE;
	}

	switch (mark) {
		case 0:
			return regist(sockfd);
			break;
		case 1:
			return logIn(sockfd);
			break;
		case 2:
			return getFileList(sockfd);
			break;
		case 3:
			return addFile(sockfd);
			break;
		case 4:
			return delFile(sockfd);
			break;
		case 5:
			return moveFile(sockfd);
			break;
		case 6:
			return copyFile(sockfd);
			break;
		case 7:
			return downLoad(sockfd);
			break;
		default:
			writeLog("undefine opt");
	}
	return 0;
}

void createServer(int portNumber){
    struct timeval timeout;
    timeout.tv_sec = 3;                
    timeout.tv_usec = 0;
	

    int recvLen = 0;
    int listenfd, sockSvr, sockMax;
    struct sockaddr_in client_addr;
    sockaddr_in servaddr;

    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == FAILURE){
        writeLog(strerror(errno)); 
        exit(0);
    }
    
	int opt = 1;
	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0) {
        writeLog(strerror(errno)); 
		close(listenfd);
		exit(0);	
	}
	
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(uint16_t(portNumber));
    
    if(bind(listenfd, (sockaddr *)&servaddr, sizeof(servaddr)) == FAILURE){
        writeLog(strerror(errno)); 
        exit(0);
    }

    if(listen(listenfd, 500) == FAILURE){
        writeLog(strerror(errno)); 
        exit(0);
    }
    
	for(;;) {
		struct sockaddr_in client_addr;
		unsigned int sin_size = sizeof(client_addr);
		int sockCli = accept(listenfd, ( struct sockaddr * ) & client_addr, & sin_size);
		if (sockCli < 0) {
			writeLog(strerror(errno));
			continue;
		}
		int pid = fork();
		if (pid == 0) {
			exec(sockCli);
			exit(0);
		} else {
			close(sockCli);
		}
    } 
	
}

int main(){
	createServer(8192);
}
