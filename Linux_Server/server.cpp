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
	struct timeval timeout;
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(sockfd, &fds);
	
	timeout.tv_sec = 60;                
	timeout.tv_usec = 0;
	int ret = select((int)sockfd+1, &fds, NULL, NULL, &timeout);
	if(ret < 0) {
		writeLog(strerror(errno));
		return -1;
	} else if(ret == 0) {
		writeLog("time out...");
		return -1;
	}

	unsigned char NLen, PLen, flag;
	char name[256], password[256];
	read(sockfd, &NLen, 1);
	read(sockfd, name, NLen);
	read(sockfd, &PLen, 1);
	read(sockfd, password, PLen);

	//flag = sql();
	
	for (;;) {
		timeout.tv_sec = 60;                
		timeout.tv_usec = 0;
		FD_ZERO(&fds);
		FD_SET(sockfd, &fds);
		int ret = select((int)sockfd+1, NULL, &fds, NULL, &timeout);
		if (ret < 0) {
			writeLog(strerror(errno));
			return -1;
		}
		if (ret == 0) continue;
		write(sockfd, &flag, 1);
		break;
	}
	return 0;
}

int logIn(int sockfd){
	struct timeval timeout;
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(sockfd, &fds);
	
	timeout.tv_sec = 60;                
	timeout.tv_usec = 0;
	int ret = select((int)sockfd+1, &fds, NULL, NULL, &timeout);
	if(ret < 0) {
		writeLog(strerror(errno));
		return -1;
	} else if(ret == 0) {
		writeLog("time out...");
		return -1;
	}

	unsigned char NLen, PLen;
	char name[256], password[256];
	int UID;
	read(sockfd, &NLen, 1);
	read(sockfd, name, NLen);
	read(sockfd, &PLen, 1);
	read(sockfd, password, PLen);

	//UID = sql();
	
	UID = htonl(UID);
	for (;;) {
		FD_ZERO(&fds);
		FD_SET(sockfd, &fds);
		timeout.tv_sec = 60;                
		timeout.tv_usec = 0;
		int ret = select((int)sockfd+1, NULL, &fds, NULL, &timeout);
		if (ret < 0) {
			writeLog(strerror(errno));
			return -1;
		}
		if (ret == 0) continue;
		write(sockfd, &UID, 4);
		break;
	}
	return 0;
}

int getFileList(int sockfd){
	struct timeval timeout;
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(sockfd, &fds);
	
	timeout.tv_sec = 60;                
	timeout.tv_usec = 0;
	int ret = select((int)sockfd+1, &fds, NULL, NULL, &timeout);
	if(ret < 0) {
		writeLog(strerror(errno));
		return -1;
	} else if(ret == 0) {
		writeLog("time out...");
		return -1;
	}

	int Len, UID;
	char path[1024];
	read(sockfd, &UID, 4);
	read(sockfd, &Len, 4);
	UID = ntohl(UID);
	Len = ntohl(Len);
	read(sockfd, path, Len);
	
	vector<string> fileList;

	
	//fileList = sql();
	
	
	unsigned int num = htonl(fileList.size());
	for (;;) {
		FD_ZERO(&fds);
		FD_SET(sockfd, &fds);
		timeout.tv_sec = 60;                
		timeout.tv_usec = 0;
		int ret = select((int)sockfd+1, NULL, &fds, NULL, &timeout);
		if(ret < 0) {
			writeLog(strerror(errno));
			return -1;
		} 
		if (ret == 0) continue;
		write(sockfd, &num, 4);
		break;
	}

	for (int i = 0; i < fileList.size(); ++i) {
		for (;;) {
			FD_ZERO(&fds);
			FD_SET(sockfd, &fds);
			timeout.tv_sec = 60;                
			timeout.tv_usec = 0;
			int ret = select((int)sockfd+1, NULL, &fds, NULL, &timeout);
			if(ret < 0) {
				writeLog(strerror(errno));
				return -1;
			} 
			if (ret == 0) continue;
			int size = htonl(fileList[i].length());
			write(sockfd, &size, 4);
			write(sockfd, fileList[i].c_str(), fileList[i].length());
			break;
		}
	}

	return 0;
}

int addFile(int sockfd){
	return 0;
}

int delFile(int sockfd){
	struct timeval timeout;
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(sockfd, &fds);
	
	timeout.tv_sec = 60;                
	timeout.tv_usec = 0;
	int ret = select((int)sockfd+1, &fds, NULL, NULL, &timeout);
	if(ret < 0) {
		writeLog(strerror(errno));
		return -1;
	} else if(ret == 0) {
		writeLog("time out...");
		return -1;
	}

	int Len, UID;
	char path[1024];
	read(sockfd, &UID, 4);
	read(sockfd, &Len, 4);
	UID = ntohl(UID);
	Len = ntohl(Len);
	read(sockfd, path, Len);
	
	char flag;

	//flag = sql();
	
	for (;;) {
		FD_ZERO(&fds);
		FD_SET(sockfd, &fds);
		timeout.tv_sec = 60;                
		timeout.tv_usec = 0;
		int ret = select((int)sockfd+1, NULL, &fds, NULL, &timeout);
		if(ret < 0) {
			writeLog(strerror(errno));
			return -1;
		} 
		if (ret == 0) continue;
		write(sockfd, &flag, 1);
		break;
	}

	return 0;
}

int moveFile(int sockfd){
	struct timeval timeout;
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(sockfd, &fds);
	
	timeout.tv_sec = 60;                
	timeout.tv_usec = 0;
	int ret = select((int)sockfd+1, &fds, NULL, NULL, &timeout);
	if(ret < 0) {
		writeLog(strerror(errno));
		return -1;
	} else if(ret == 0) {
		writeLog("time out...");
		return -1;
	}

	int SLen, DLen, UID;
	char SPath[1024], DPath[1024];
	read(sockfd, &UID, 4);
	UID = ntohl(UID);
	
	read(sockfd, &SLen, 4);
	SLen = ntohl(SLen);
	read(sockfd, SPath, SLen);
	
	read(sockfd, &DLen, 4);
	DLen = ntohl(DLen);
	read(sockfd, DPath, DLen);

	char flag;

	//flag = sql();
	
	for (;;) {
		FD_ZERO(&fds);
		FD_SET(sockfd, &fds);
		timeout.tv_sec = 60;                
		timeout.tv_usec = 0;
		int ret = select((int)sockfd+1, NULL, &fds, NULL, &timeout);
		if(ret < 0) {
			writeLog(strerror(errno));
			return -1;
		} 
		if (ret == 0) continue;
		write(sockfd, &flag, 1);
		break;
	}
}

int copyFile(int sockfd){
	struct timeval timeout;
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(sockfd, &fds);
	
	timeout.tv_sec = 60;                
	timeout.tv_usec = 0;
	int ret = select((int)sockfd+1, &fds, NULL, NULL, &timeout);
	if(ret < 0) {
		writeLog(strerror(errno));
		return -1;
	} else if(ret == 0) {
		writeLog("time out...");
		return -1;
	}

	int SLen, DLen, UID;
	char SPath[1024], DPath[1024];
	read(sockfd, &UID, 4);
	UID = ntohl(UID);
	
	read(sockfd, &SLen, 4);
	SLen = ntohl(SLen);
	read(sockfd, SPath, SLen);
	
	read(sockfd, &DLen, 4);
	DLen = ntohl(DLen);
	read(sockfd, DPath, DLen);

	char flag;

	//flag = sql();
	
	for (;;) {
		FD_ZERO(&fds);
		FD_SET(sockfd, &fds);
		timeout.tv_sec = 60;                
		timeout.tv_usec = 0;
		int ret = select((int)sockfd+1, NULL, &fds, NULL, &timeout);
		if(ret < 0) {
			writeLog(strerror(errno));
			return -1;
		} 
		if (ret == 0) continue;
		write(sockfd, &flag, 1);
		break;
	}
}

int downLoad(int sockfd){

}

int exec(int sockfd){
    struct timeval timeout;
	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(sockfd, &readfds);
	
	timeout.tv_sec = 60;                
	timeout.tv_usec = 0;
	int ret = select((int)sockfd+1, &readfds, NULL, NULL, &timeout);
	if(ret < 0) {
		writeLog(strerror(errno));
		return -1;
	} else if(ret == 0) {
		writeLog("time out...");
		return -1;
	}
	char mark;
	int L = read(sockfd, &mark, 1);
	if (L <= 0) {
		writeLog(strerror(errno));
		return -1;
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

    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        writeLog(strerror(errno)); 
        exit(0);
    }

    int val;
	if ((val = fcntl(listenfd, F_GETFL, 0)) < 0) {
        writeLog(strerror(errno)); 
		close(listenfd);
		exit(0);
	}
	if (fcntl(listenfd, F_SETFL, val|O_NONBLOCK) < 0) {
        writeLog(strerror(errno)); 
		close(listenfd);
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
    
    if(bind(listenfd, (sockaddr *)&servaddr, sizeof(servaddr)) == -1){
        writeLog(strerror(errno)); 
        exit(0);
    }

    if(listen(listenfd, 500) == -1){
        writeLog(strerror(errno)); 
        exit(0);
    }
    
	for(;;) {
		fd_set readfds;
		FD_ZERO(&readfds);
		FD_SET(listenfd, &readfds);
		
		timeout.tv_sec = 15;                
		timeout.tv_usec = 0;
        int ret = select((int)listenfd+1, &readfds, NULL, NULL, &timeout);
        if(ret < 0) {
            writeLog(strerror(errno));
			continue;
        } else if(ret == 0) {
            continue;
        }
		
		if(FD_ISSET(listenfd, &readfds)) {
			struct sockaddr_in client_addr;
			unsigned int sin_size = sizeof(client_addr);
			int sockCli = accept(listenfd, ( struct sockaddr * ) & client_addr, & sin_size);
			if (sockCli < 0) {
				writeLog(strerror(errno));
			}
			int pid = fork();
			if (pid == 0) {
				exec(sockCli);
			} else {
				close(sockCli);
			}
        }
    } 
	
}

int main(){
	createServer(8192);
}
