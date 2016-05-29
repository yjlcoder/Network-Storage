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
#include <openssl/md5.h>
#include <sys/sem.h>
#include<unistd.h>

#define PATHSIZE 1024
#define NAMESIZE 256
#define PSSWSIZE 256
#define BUFFSIZE 2048
#define FAILURE -1
#define BLOCKSIZE (1024*1024)

#define CREATEFILEKEY (key_t)0x1021
#define FILEOPKEY (key_t)0x0907
#define FILEMERGEKEY (key_t)0x1210

using namespace std;

#define IFLAGS (IPC_CREAT|IPC_EXCL)

union semun{
    int val;
    struct semid_ds *buf;
    unsigned short * ary;
};

int min(int a, int b){
	return a < b ? a : b;
}

inline
unsigned long long htonll(unsigned long long x){
	unsigned long low = x & 0xffffffff;
	unsigned long high = x >> 32;
	return (((unsigned long long)htonl(low)<<32) 
			| (unsigned long long)htonl(high));
}

inline
unsigned long long ntohll(unsigned long long x){
	unsigned long low = x & 0xffffffff;
	unsigned long high = x >> 32;
	return (((unsigned long long)ntohl(low)<<32) 
			| (unsigned long long)ntohl(high));
}


int sem_init(key_t key, int inival)
{
  int semid; 
  union semun arg;
  semid=semget(key,1,0660|IFLAGS);
  arg.val=inival;
  semctl(semid, 0, SETVAL, arg);
  return semid;
}

void P(int semid)
{
    struct sembuf sb; 
    sb.sem_num=0;
    sb.sem_op=-1;
    sb.sem_flg=0;
    semop(semid,&sb,1);
}

void V(int semid)
{
    struct sembuf sb; 
    sb.sem_num=0;
    sb.sem_op=1;
    sb.sem_flg=0;
    semop(semid,&sb,1); 
}


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
	long L;
	L = recv(sockfd, &NLen, sizeof(NLen), MSG_WAITALL);
	if (L != sizeof(NLen)) return FAILURE;
	L = recv(sockfd, name, NLen, MSG_WAITALL);
	if (L != NLen) return FAILURE;
	L = recv(sockfd, &PLen, sizeof(PLen), MSG_WAITALL);
	if (L != sizeof(PLen)) return FAILURE;
	L = recv(sockfd, password, PLen, MSG_WAITALL);
	if (L != PLen) return FAILURE;
	
	long UID;
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

int fileExist(int sockfd, const char * fileName, const char * md5_str){
	long mark = -2;
	mark = htonl(mark);
	int L = send(sockfd, &mark, sizeof(mark), 0);
	if (L != sizeof(mark)) return -1;

	//sql();

	return 0;
}

int semCreateFile = sem_init(CREATEFILEKEY, 1);
int semFileMerge = sem_init(FILEMERGEKEY, 1);

int createConfig(const char * fileName, const char * md5_str, const long long fileSize){
	
	char cmd[1024];
	char md5_file[1024];
	sprintf(cmd, "mkdir upload");
	system(cmd);

	sprintf(cmd, "mkdir upload/%x", md5_str[0]);
	system(cmd);

	sprintf(md5_file, "upload/%x/", md5_str[0]);
	for (int i  = 0; i < 16; ++i) {
		int l = strlen(md5_file);
		sprintf(md5_file + l, "%x", md5_str[i]);
	}
	int l = strlen(md5_file);
	sprintf(cmd, "mkdir %s", md5_file);
	system(cmd);

	sprintf(cmd + l, "/cfg");
	P(semCreateFile);
	
	if (access(cmd, 0) != -1) {
		ofstream file(cmd, ios::binary);
		int blockNum = (fileSize - 1) / (1024*1024) + 1;
		char flag = 0;
		for (int i = 0; i < blockNum; ++i)
			file.write(&flag, 1);

	//sql();
	
	}
	V(semCreateFile);
	return 0;
}



int recvFile(int sockfd, const char * md5_str, int blockNum){
	int size, L;
	char buff[2048];
	unsigned char md5[16];
	unsigned char md5_chk[16];
	char md5_32[33];
	char path[1024];

	char md5_32[33];
	for (int i = 0; i < 16; ++i) {
		sprintf(md5_32 + i*2, "%x", md5_str[i]);
	}
	sprintf(path, "upload/%x/%s/%d", md5_str[0], md5_32, blockNum);

	L = recv(sockfd, &size, sizeof(size), MSG_WAITALL);
	if (L != sizeof(size)) return -1;
	size = ntohl(size);
	MD5_CTX c;
	ofstream file(path);
	while (size > 0) {
		int s = min(size, 1024);
		size -= s;
		L = recv(sockfd, buff, s, MSG_WAITALL);
		if (L != s) return -1;
		MD5_Update(&c, buff, L);
		file.write(buff, L);
	}

	L = recv(sockfd, md5, 16, MSG_WAITALL);
	if (L != 16) return -1;
	MD5_Final(md5_chk,&c);
	for (int i = 0; i < 16; ++i) 
		if (md5_chk[i] != md5[i]) return -1;
	return 0;
}

int mergeFile(const char * md5_str, const long fileL, const long long fileSize){
	char compPath[1024];
	
	char md5_32[33];
	for (int i = 0; i < 16; ++i) {
		sprintf(md5_32 + i*2, "%x", md5_str[i]);
	}
	sprintf(compPath, "file/%x/%s", md5_str[0], md5_32);

	P(semFileMerge);
	if (access(compPath, 0) == -1) {
		ofstream compF(compPath, ios::binary);
		for (int i = 0; i < fileL; ++i) {
			char sourPath[1024];
			sprintf(sourPath, "upload/%x/%s/%d", md5_str[0], md5_32, i);
			ifstream sourF(sourPath, ios::binary);
			compF << sourF.rdbuf();
		}
	}

	char cmd[1024];
	sprintf(cmd, "rm -rf upload/%x/%s", md5_str[0], md5_32);
	system(cmd);
	V(semFileMerge);

	//sql()

	return 0;
}



int semFileOp = sem_init(FILEOPKEY, 1);
int selectBlock(int sockfd, const char * path, const char * md5_str, const long long fileSize){
	char cfg_file[1024];
	sprintf(cfg_file, "upload/%x/", md5_str[0]);
	for (int i  = 0; i < 16; ++i) {
		int l = strlen(cfg_file);
		sprintf(cfg_file + l, "%x", md5_str[i]);
	}
	int l = strlen(cfg_file);
	sprintf(cfg_file + l, "/cfg");
	
	P(semFileOp);

	int fileL;
	fstream file(cfg_file, ios::in|ios::out);
	file.seekg(0, ios::end);
	fileL = file.tellg();
	char flag = 0;
	char overmark = 1;
	int blockNum = -1;
	int overNum = 0;

	for (int i = 0; i < fileL; ++i) {
		file.read(&flag, 1);
		if (flag != -1) overmark = 0;
		if (flag == 0 && blockNum == -1) {
			blockNum = i;
			file.seekg(i, ios::beg);
			flag = 1;
			file.write(&flag, 1);
		}
		if (flag == -1) ++overNum;
	}
	file.close();
	V(semFileOp);
	
	if (overmark == 1) {
		blockNum = -1;
	} else if (overmark == 0 && blockNum == -1){
		blockNum = -2;
	}
	blockNum = htonl(blockNum);
	recv(sockfd, &blockNum, sizeof(blockNum), MSG_WAITALL);
	if (overmark == 0 && blockNum == -1) return 0;

	if (overmark == 1) {
		//add_file to complete;
		mergeFile(md5_str, fileL, fileSize);
	} else {
		flag = recvFile(sockfd, md5_str, blockNum);
	}

	P(semFileOp);
	fstream file_(cfg_file, ios::in|ios::out);
	file_.seekg(blockNum, ios::beg);

	if (flag == 0) {
		++overNum;
		char buf = -1;
		file_.write(&buf, 1);
	} else {
		char buf = 0;
		file_.write(&buf, 1);
	}
	file_.close();
	V(semFileOp);

	overNum = htonl(overNum);
	int L = send(sockfd, &overNum, sizeof(overNum), 0);
	if (L != sizeof(overNum)) return -1;
	return 0;
}


int addFile(int sockfd){
	long UID, Len;
	long long fileSize;
	char path[PATHSIZE];
	char md5_str[16];
	int L;
	L = recv(sockfd, &UID, sizeof(UID), MSG_WAITALL);
	if (L != sizeof(UID)) return -1;
	UID = ntohl(UID);

	L = recv(sockfd, &Len, sizeof(Len), MSG_WAITALL);
	if (L != sizeof(Len)) return -1;
	Len = ntohl(Len);

	L = recv(sockfd, path, Len, MSG_WAITALL);
	if (L != Len) return -1;

	L = recv(sockfd, md5_str, 16, MSG_WAITALL);
	if (L != 16) return -1;

	L = recv(sockfd, &fileSize, sizeof(fileSize), MSG_WAITALL);
	if (L != sizeof(fileSize)) return -1;
	fileSize = ntohll(fileSize);

	char flag;

	//flag = sql();
	
	switch (flag) {
		case 0:
			return fileExist(sockfd, path, md5_str);
			break;
		case 1:
			createConfig(path, md5_str, fileSize);
			break;
		case 2:
			//nothing
			break;
		default:
			return -1;
	}

	return selectBlock(sockfd, path, md5_str, fileSize);
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
	int UID, Len, L;
	char path[PATHSIZE];
	L = recv(sockfd, &UID, sizeof(UID), MSG_WAITALL);
	if (L != sizeof(UID)) return -1;
	UID = ntohl(UID);

	L = recv(sockfd, &Len, sizeof(Len), MSG_WAITALL);
	if (L != sizeof(Len)) return -1;
	Len = ntohl(Len);

	L = recv(sockfd, path, Len, MSG_WAITALL);
	if (L != Len) return -1;

	string MD5;
	char flag;


	//MD5 = sql();
	//flag = sql(MD5.c_str());
	
	
	long long res;
	if (flag == -1) res = -1;
	ifstream file(MD5.c_str(), ios::binary);
	if (!file.is_open()) {
		res = -1;
	}
	res = htonll(res);
	if (res == -1) {
		send(sockfd, &res, sizeof(res), 0);
		return -1;
	}
	file.seekg(0, ios::end);
	res = file.tellg();
	L = send(sockfd, &res, sizeof(res), 0);
	if (L != sizeof(res)) return -1;
	for (;;) {
		long num;
		L = recv(sockfd, &num, sizeof(num), MSG_WAITALL);
		if (L != sizeof(num)) return -1;
		
		MD5_CTX c;

		long size = BLOCKSIZE;
		unsigned long long offset = num * BLOCKSIZE;
		file.seekg(offset, ios::beg);
		if (offset + BLOCKSIZE >= res) size = res - offset;

		long tsize = htonl(size);
		L = send(sockfd, &tsize, sizeof(tsize), 0);
		if (L != sizeof(tsize)) return -1;

		char buff[2048];
		while (size > 0) {
			int l = min(1024, size);
			size -= l;
			file.read(buff, l);
			MD5_Update(&c, buff, l);
			L = send(sockfd, buff, l, 0);
			if (L != l) return -1;
		}
		unsigned char md5_str[16];
		MD5_Final(md5_str,&c);
		L = send(sockfd, md5_str, 16, 0);
		if (L != 16) return -1;
	}

	return 0;
}

int exec(int sockfd){
	char mark;
	int L = recv(sockfd, &mark, sizeof(mark), MSG_WAITALL);
	if (L <= 0) {
		writeLog(strerror(errno));
		return FAILURE;
	}

	switch (mark) {
		case 0x00:
			return regist(sockfd);
			break;
		case 0x01:
			return logIn(sockfd);
			break;
		case 0x02:
			return getFileList(sockfd);
			break;
		case 0x03:
			return addFile(sockfd);
			break;
		case 0x04:
			return delFile(sockfd);
			break;
		case 0x05:
			return moveFile(sockfd);
			break;
		case 0x06:
			return copyFile(sockfd);
			break;
		case 0x07:
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
			exit(exec(sockCli));
		} else {
			close(sockCli);
		}
    } 
	
}

int main(){
	createServer(8192);
}


