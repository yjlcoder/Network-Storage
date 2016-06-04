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
#include <openssl/sha.h>
#include <sys/sem.h>
#include <unistd.h>
#include "database/DB_Operate.h"

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

string md5S(const unsigned char * md5){
	char res[33];
	for (int i = 0; i < 16; ++i) {
		sprintf(res + (i*2), "%02x", md5[i]);
	}
	return res;
}

int getSmallStr(int sockfd, char * buff){
	int L;
	unsigned char Len = 0;
	L = recv(sockfd, &Len, sizeof(Len), MSG_WAITALL);
	cout << Len << endl;
	if (L != sizeof(Len)) return -1;
	L = recv(sockfd, buff, Len, MSG_WAITALL);
	cout << buff << endl;
	if (L != Len) return -1;
	buff[Len] = '\0';
	return Len;
}

int getStr(int sockfd, char * buff){
	int L;
	int Len = 0;
	cout << __LINE__ << ' ' << sizeof(Len) << endl;
	L = recv(sockfd, &Len, sizeof(Len), MSG_WAITALL);
	cout << __LINE__ << ' ' << L << endl;
	if (L != sizeof(Len)) return -1;
	Len = ntohl(Len);
	cout << __LINE__ << ' ' << Len << endl;
	if (Len == 0) return 0;
	cout << __LINE__ << endl;
	L = recv(sockfd, buff, Len, MSG_WAITALL);
	
	cout << __LINE__ << endl;
	if (L != Len) return -1;

	cout << __LINE__ << endl;
	buff[Len] = '\0';
	return Len;
}

int sendStr(int sockfd, const int Len, const char * buff){
	int nLen = htonl(Len);
	char sendBuff[1024];
	memcpy(sendBuff, &nLen, sizeof(nLen));
	memcpy(sendBuff + sizeof(nLen), buff, Len);
	int L = Len + sizeof(nLen);
	if (send(sockfd, sendBuff, L, 0) != L) return -1;
	return 0;
}


int regist(int sockfd){
	char name[NAMESIZE], password[PSSWSIZE];
	int L;
	DB_Operate DB;
	if (getSmallStr(sockfd, name) == -1) return -1;
	if ((L = getSmallStr(sockfd, password)) == -1) return -1;

	char flag;
	unsigned char sha1[40];
	char passwordS[41];
	cout << password << endl;
	SHA1((unsigned char *) password, L, sha1);
	
	for (int i = 0; i < 20; ++i) {
		sprintf(passwordS + 2*i, "%02x", (int)sha1[i]);
	}

	flag = DB.Insert_User(name, passwordS);
	flag = flag ? 0x00 : -1;
	if (send(sockfd, &flag, sizeof(flag), 0) != sizeof(flag)) return -1;
	
	int UID = DB.Check_User(name, passwordS);
	char UIDS[13];
	sprintf(UIDS, "%d", UID);
	DB.Insert_File_Info(UIDS, "/", "NULL");
	return 0;
}

int logIn(int sockfd){
	char name[NAMESIZE], password[PSSWSIZE];
	int L;
	DB_Operate DB;
	if (getSmallStr(sockfd, name) == -1) return -1;
	if ((L = getSmallStr(sockfd, password)) == -1) return -1;

	unsigned char sha1[40];
	char passwordS[41];
	cout << password << endl;
	SHA1((unsigned char *) password, L, sha1);
	for (int i = 0; i < 20; ++i) {
		sprintf(passwordS + 2*i, "%02x", (int)sha1[i]);
	}

	int UID;
	UID = DB.Check_User(name, passwordS);
	if (UID == -1) UID == 0;

	UID = htonl(UID);
	if (send(sockfd, &UID, sizeof(UID), 0) != sizeof(UID)) return -1;
	return 0;
}

int getFileList(int sockfd){
	DB_Operate DB;
	int Len, UID, L;
	char path[PATHSIZE];
	L = recv(sockfd, &UID, sizeof(UID), MSG_WAITALL);
	cout << L << endl;
	if (L != sizeof(UID)) return FAILURE;
	UID = ntohl(UID);
	if (getStr(sockfd, path) == -1) {
		writeLog("path error");
		return -1;
	}

	char UIDS[13];
	sprintf(UIDS, "%d", UID);
	vector<string> fileList = DB.Query_File_List(UIDS, path);
	
	unsigned int num = 0;//htonl(fileList.size());
	cout << "size : " << fileList.size() << endl;
	for (int i = 0; i < fileList.size(); ++i) {
		string msg = path + fileList[i];
cout << msg << endl;
		num += strlen(msg.c_str()) + 4;
	}
	num = htonl(num);
	L = send(sockfd, &num, sizeof(num), 0);
	if (L != sizeof(num)) return FAILURE;

	for (int i = 0; i < fileList.size(); ++i) {
		string msg = path + fileList[i];
		if (sendStr(sockfd, msg.length(), msg.c_str()) == -1) return -1;
	}

	return 0;
}

int fileExist(int sockfd, const int UID, const char * fileName, const unsigned char * md5_str){
	char md5_32[33];
	DB_Operate DB;

	for (int i = 0; i < 16; ++i) {
		sprintf(md5_32 + i * 2, "%02x", md5_str[i]);
	}

	char UIDS[13];
	sprintf(UIDS, "%d", UID);
	char flag = DB.Insert_File_Info(UIDS, fileName, md5_32);	
	int mark = -2;
	if (flag == false) mark = -3;
	mark = htonl(mark);
	int L = send(sockfd, &mark, sizeof(mark), 0);
	if (L != sizeof(mark)) return -1;
	return 0;
}

int semCreateFile = sem_init(CREATEFILEKEY, 1);
int semFileMerge = sem_init(FILEMERGEKEY, 1);

int createConfig(const unsigned char * md5_str, const long long fileSize){
	
	char cmd[1024];
	char md5_file[1024];
	sprintf(cmd, "mkdir upload");
	system(cmd);

	sprintf(cmd, "mkdir upload/%02x", md5_str[0]);
	system(cmd);

	sprintf(md5_file, "upload/%02x/", md5_str[0]);
	for (int i  = 0; i < 16; ++i) {
		int l = strlen(md5_file);
		sprintf(md5_file + l, "%02x", md5_str[i]);
	}
	int l = strlen(md5_file);
	sprintf(cmd, "mkdir %s", md5_file);
	system(cmd);

	sprintf(cmd, "upload/%02x/%s/cfg", md5_str[0], md5S(md5_str).c_str());
	P(semCreateFile);
	
	if (access(cmd, 0) == -1) {
		cout << cmd << endl;
		ofstream file(cmd, ios::binary);
		if (file.is_open()) {
			cout << "file ok" << endl;
		} else {
			cout << "file error" << endl;
		}
		int blockNum = (fileSize - 1) / (1024*1024) + 1;
		cout << blockNum << endl;
		char flag = 0;
		for (int i = 0; i < blockNum; ++i)
			file.write(&flag, 1);
		
		DB_Operate DB;
		cout << md5S(md5_str) << endl;
		DB.Insert_Md5_Statu(md5S(md5_str), "2");
		file.close();
	}
	V(semCreateFile);
	return 0;
}



int recvFile(int sockfd, const unsigned char * md5_str, int blockNum){
	int size, L;
	char buff[2048];
	unsigned char md5[16];
	unsigned char md5_chk[16];
	char md5_32[33];
	char path[1024];

	for (int i = 0; i < 16; ++i) {
		sprintf(md5_32 + i*2, "%02x", md5_str[i]);
	}
	sprintf(path, "upload/%02x/%s/%d", md5_str[0], md5_32, blockNum);

	//cout << __LINE__ << endl;
	L = recv(sockfd, &size, sizeof(size), MSG_WAITALL);
	if (L != sizeof(size)) return -1;
	//cout << size << endl;
	size = ntohl(size);
	//cout << size << endl;
	MD5_CTX c;
	MD5_Init(&c);
	ofstream file(path);
	while (size > 0) {
		//cout << size << endl;
		int s = min(size, 1024);
		size -= s;
		L = recv(sockfd, buff, s, MSG_WAITALL);
		buff[L] = 0;
		//puts(buff);
		//cout << L << endl;
		if (L != s) return -1;
		MD5_Update(&c, buff, L);
		file.write(buff, L);
	}

	L = recv(sockfd, md5, 16, MSG_WAITALL);
	if (L != 16) return -1;
	MD5_Final(md5_chk,&c);
	for (int i = 0; i < 16; ++i) {
		//cout << (int)md5_chk[i] << ' ' << (int)md5[i] << endl;
		if (md5_chk[i] != md5[i]) return -1;
	}
	return 0;
}

int mergeFile(const unsigned char * md5_str, const int fileL, const long long fileSize){
	char cmd[1024];
	char md5_file[1024];
	sprintf(cmd, "mkdir /files");
	system(cmd);

	sprintf(cmd, "mkdir /files/%02x", md5_str[0]);
	system(cmd);

	char compPath[1024];
	char md5_32[33];
	for (int i = 0; i < 16; ++i) {
		sprintf(md5_32 + i*2, "%02x", md5_str[i]);
	}
	sprintf(compPath, "/files/%02x/%s", md5_str[0], md5_32);

	P(semFileMerge);
	if (access(compPath, 0) == -1) {
		ofstream compF(compPath, ios::binary);
		for (int i = 0; i < fileL; ++i) {
			char sourPath[1024];
			sprintf(sourPath, "upload/%02x/%s/%d", md5_str[0], md5_32, i);
			ifstream sourF(sourPath, ios::binary);
			compF << sourF.rdbuf();
		}
	}

	sprintf(cmd, "rm -rf upload/%02x/%s", md5_str[0], md5_32);
	system(cmd);
	V(semFileMerge);

	DB_Operate DB;
	int flag = DB.Update_Md5_Statu(md5_32, "1"); 

	return 0;
}



int semFileOp = sem_init(FILEOPKEY, 1);
int selectBlock(int sockfd, const char * path, const unsigned char * md5_str, const long long fileSize){
	char cfg_file[1024];
	sprintf(cfg_file, "upload/%02x/", md5_str[0]);
	for (int i  = 0; i < 16; ++i) {
		int l = strlen(cfg_file);
		sprintf(cfg_file + l, "%02x", md5_str[i]);
	}
	int l = strlen(cfg_file);
	sprintf(cfg_file + l, "/cfg");
	
	P(semFileOp);
	if (access(cfg_file, 0) == -1) {
		createConfig(md5_str, fileSize);
	}
	int fileL;
	fstream file(cfg_file, ios::in|ios::out|ios::binary);
	file.seekg(0, ios::end);
	fileL = file.tellg();
	char flag = 0;
	char overmark = 1;
	int blockNum = -1;
	int overNum = 0;
	file.seekg(0, ios::beg);
	cout << cfg_file << endl;
	for (int i = 0; i < fileL; ++i) {
		file.read(&flag, 1);
		//cout << "i : " << i << ' ' << (int)flag << endl;
		if (flag != -1) overmark = 0;
		if (flag == 0 && blockNum == -1) {
			//cout << (int)flag << endl;
			blockNum = i;
			file.seekg(i, ios::beg);
			flag = 1;
			//cout << (int)flag << endl;
			file.write(&flag, 1);
			break;
		}
	}
	file.close();
	V(semFileOp);
	
	if (overmark == 1) {
		blockNum = -1;
	} else if (overmark == 0 && blockNum == -1){
		blockNum = -2;
	}
	cout << "blockNum : " << blockNum << endl;
	int tblockNum = htonl(blockNum);
	send(sockfd, &tblockNum, sizeof(tblockNum), 0);
	if (overmark == 1 || blockNum == -1) {
		if (overmark == 1) mergeFile(md5_str, fileL, fileSize);
		return 0;
	}

	cout << "recv" << blockNum << endl;
	if (blockNum != -1) flag = recvFile(sockfd, md5_str, blockNum);
	cout << __FILE__ << __LINE__ << endl;
	
	//if (flag == -1) return -1;
	cout << "xxx" << endl;

	P(semFileOp);
	fstream file_(cfg_file, ios::in|ios::out|ios::binary);
	file_.seekg(blockNum, ios::beg);
	if (flag == 0) {
		++overNum;
		char buf = -1;
		file_.write(&buf, 1);
	} else {
		char buf = 0;
		file_.write(&buf, 1);
	}

	file_.seekg(0, ios::beg);
	overNum = 0;
	for (int i = 0; i < fileL; ++i) {
		file_.read(&flag, 1);
		//cout << (int)flag << endl;
		if (flag == -1) ++overNum;
	}
	file_.close();
	V(semFileOp);
	//if (flag == -1) return -1;

	cout << overNum << ' ' << flag << ' ' << fileL << endl;
	if (overNum == fileL) mergeFile(md5_str, fileL, fileSize);

	overNum = htonl(overNum);
	int L = send(sockfd, &overNum, sizeof(overNum), 0);
	if (L != sizeof(overNum)) return -1;
	return 0;
}


int addFile(int sockfd){
	int UID, Len;
	long long fileSize;
	char path[PATHSIZE];
	unsigned char md5_str[16];
	int L;
	DB_Operate DB;

	if (recv(sockfd, &UID, sizeof(UID), MSG_WAITALL) != 4) return -1;
	UID = ntohl(UID);

	char UIDS[13];
	sprintf(UIDS, "%d", UID);
	
	cout << __FILE__ << endl;
	if ((L = getStr(sockfd, path)) == -1) return -1;

	cout << __FILE__ << endl;
	if (path[L-1] == '/') {
		char flag = DB.Insert_File_Info(UIDS, path, "NULL");
		int msg = flag ? 0 : -1;
		msg = htonl(msg);
		send(sockfd, &msg, 4, 0);
		return 0;
	}

	cout << __FILE__ << endl;
	L = recv(sockfd, md5_str, 16, MSG_WAITALL);
	if (L != 16) return -1;

	cout << __FILE__ << endl;
	L = recv(sockfd, &fileSize, 8, MSG_WAITALL);
	if (L != 8) return -1;
	fileSize = ntohll(fileSize);
	cout << fileSize << endl;

	char flag;
	flag = DB.Query_Md5_Statu(md5S(md5_str));
	

	cout << flag << endl;

	int msg = -1;
	switch (flag) {
		case -1:
			createConfig(md5_str, fileSize);
			break;
		case 1:
			return fileExist(sockfd, UID, path, md5_str);
			break;
		case 2:
			break;
		case 3:
			return 0;
		default:
			return -1;
	}

	cout << "selectBlock" << endl;
	return selectBlock(sockfd, path, md5_str, fileSize);
}



int delFile(int sockfd){
	int Len, UID, L;
	char path[PATHSIZE];
	DB_Operate DB;

	L = recv(sockfd, &UID, sizeof(UID), MSG_WAITALL);
	if (L != sizeof(UID)) return FAILURE;
	UID = ntohl(UID);

	if (getStr(sockfd, path) == -1) return -1;

	char flag;
	char UIDS[13];
	sprintf(UIDS, "%d", UID);
	flag = DB.Delete_File_Info(UIDS, path);
	
	flag = flag ? 0 : -1;
	L = send(sockfd, &flag, sizeof(flag), 0);
	if (L != sizeof(flag)) return FAILURE;

	return 0;
}

int moveFile(int sockfd){
	int SLen, DLen, UID, L;
	char SPath[PATHSIZE], DPath[PATHSIZE];
	DB_Operate DB;
	L = recv(sockfd, &UID, sizeof(UID), MSG_WAITALL);
	if (L != sizeof(UID)) return FAILURE;
	UID = ntohl(UID);

	if (getStr(sockfd, SPath) == -1) return -1;
	if (getStr(sockfd, DPath) == -1) return -1;

	char flag;
	char UIDS[13];
	sprintf(UIDS, "%d", UID);
	flag = DB.Update_File_Info(UIDS, SPath, DPath);
	
	flag = flag ? 0 : -1;
	L = send(sockfd, &flag, sizeof(flag), 0);
	if (L != sizeof(flag)) return FAILURE;
	return 0;
}

int copyFile(int sockfd){
	DB_Operate DB;
	int SLen, DLen, UID, L;
	char SPath[PATHSIZE], DPath[PATHSIZE];
	L = recv(sockfd, &UID, sizeof(UID), MSG_WAITALL);
	if (L != sizeof(UID)) return FAILURE;
	UID = ntohl(UID);

	if (getStr(sockfd, SPath) == -1) return -1;
	if (getStr(sockfd, DPath) == -1) return -1;

	char flag;
	char UIDS[13];
	sprintf(UIDS, "%d", UID);
	flag = DB.Copy_File_Info(UIDS, SPath, DPath);
	flag = flag ? 0 : -1;

	L = send(sockfd, &flag, sizeof(flag), 0);
	if (L != sizeof(flag)) return FAILURE;
	return 0;
}

int downLoad(int sockfd){
	int UID, Len, L;
	char path[PATHSIZE];
	L = recv(sockfd, &UID, sizeof(UID), MSG_WAITALL);
	if (L != sizeof(UID)) return -1;
	UID = ntohl(UID);

	if (getStr(sockfd, path) == -1) return -1;
	char flag;
	char UIDS[20];
	sprintf(UIDS, "%d", UID);
	
	DB_Operate DB;
	string MD5 = DB.Query_Md5(UIDS, path);
	flag = DB.Query_Md5_Statu(MD5);
	cout << (int)flag << endl;
	long long res;
	if (flag != 1) {
		int msg = -1;
		send(sockfd, &msg, 4, 0);
		return -1;
	}
	
	char md5_32[33], compPath[PATHSIZE];
	cout << MD5 << endl;
	sprintf(md5_32, "%s", MD5.c_str());
	sprintf(compPath, "/files/%c%c/%s", md5_32[0], md5_32[1], md5_32);
	cout << compPath << endl;
	ifstream file(compPath, ios::binary);

	if (!file.is_open()) {
		cout << "file error" << endl;
		res = -1;
	}
	if (res == -1) {
		send(sockfd, &res, sizeof(res), 0);
		return -1;
	}
	file.seekg(0, ios::end);
	res = file.tellg();
	long long fileSize = res;
	cout << "file size : " << res << endl;
	res = htonll(res);
	L = send(sockfd, &res, sizeof(res), 0);
	if (L != sizeof(res)) return -1;
	for (;;) {
		int num;
		L = recv(sockfd, &num, sizeof(num), MSG_WAITALL);
		if (L != sizeof(num)) return -1;
		num = ntohl(num);
		
		MD5_CTX c;
		MD5_Init(&c);
		int size = BLOCKSIZE;
		unsigned long long offset = 1LL * num * BLOCKSIZE;
		file.seekg(offset, ios::beg);
		if (offset + BLOCKSIZE >= fileSize) size = fileSize - offset;

		int tsize = htonl(size);
		L = send(sockfd, &tsize, sizeof(tsize), 0);
		if (L != sizeof(tsize)) return -1;
		
		cout << path << endl;
		cout << offset << ' ' << size << ' ' << tsize << endl;
		char buff[2048];
		while (size > 0) {
			int l = min(1024, size);
			//cout << l << endl;
			size -= l;
			//cout << size << endl;
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
	signal(SIGCHLD, SIG_IGN);
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
			int flag = exec(sockCli);
			if (flag == -1) 
				send(sockCli, &flag, 4, 0);
			exit(flag);
		} else {
			close(sockCli);
		}
    } 
	
}

int main(){
	createServer(8192);
}

