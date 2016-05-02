#include <zconf.h>
#include <fcntl.h>
#include <cstdlib>
#include <sys/param.h>
#include <sys/stat.h>
#include <cstdio>
#include <iostream>
#include <netinet/in.h>
#include <cstring>
#include <fstream>
#include <sstream>
#include <mysql/mysql.h>

using namespace std;

const int BUFSIZE = 10240;
const int process = 500;

void initDaemon(){
    setsid();
    umask(0);
    for(int i = 0; i < NOFILE; i++){
        close(i);
    }
}

int find(uint16_t vessel[process], uint16_t element){
    int ans = -1;
    for(int i = 0; i < process; i++){
        if(element == vessel[i]) {
            ans = i;
            break;
        }
    }
    return ans;
}

void createServer(int portNumber) {
    int socketfd;
    ofstream fout("main.pid.txt");
    if ((socketfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        fout << "Cannot create socket" << endl;
        exit(0);
    }

    fcntl(socketfd, O_NONBLOCK);

    sockaddr_in servaddr;
    sockaddr_in remaddr;
    socklen_t addrlen = sizeof(servaddr);
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(uint16_t(portNumber));

    if (bind(socketfd, (sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        fout << "Bind Failed" << endl;
        exit(0);
    }

    int recvlen;
    char buf[BUFSIZE];

    uint16_t socketfdSets[process];
    in_addr_t addrSets[process];
    int getSocket = 0;
    int finishSocket = 0;
    memset(&socketfdSets, 0, sizeof(socketfdSets));
    int socketProgress[process];
    memset(&socketProgress, 0, sizeof(socketProgress));
    uint32_t pidSets[process];
    string timeSets[process];
    string randomStrSets[process];

    stringstream ss("");

    for(;;){
        recvlen = recvfrom(socketfd, buf, BUFSIZE, 0, (sockaddr*)&remaddr, &addrlen);
        if(recvlen < 0) continue;
        uint16_t port = remaddr.sin_port;
        int found = find(socketfdSets, port);
        if(found == -1){
            //Can't find
            socketfdSets[getSocket] = port;
            addrSets[getSocket] = remaddr.sin_addr.s_addr;
            found = getSocket;
            getSocket += 1;
        }
        if(recvlen == 4){
            uint32_t *pid = (uint32_t *)buf;
            *pid = ntohl(*pid);
            pidSets[found] = *pid;
        } else if (recvlen == 19){
            buf[recvlen] = '\0';
            timeSets[found] = buf;

            //get random number between 5000 and 9999
            srand(time(NULL) + getpid());
            ss.str("");
            ss << "str" << rand()%5000 + 5000;
            sendto(socketfd, ss.str().c_str(), 7, 0, (sockaddr*) &remaddr, addrlen);
        } else if (recvlen > 500){
            buf[recvlen] = '\0';
            randomStrSets[found] = buf;
            finishSocket += 1;
        }
        if(finishSocket == process)
            break;
    }

    for(int i = 0; i < process; i++){
        memset(&remaddr, 0, sizeof(remaddr));
        remaddr.sin_port = socketfdSets[i];
        remaddr.sin_addr.s_addr = addrSets[i];
        sendto(socketfd, "end", 3, 0, (sockaddr*) &remaddr, addrlen);
    }

    for(int i = 0; i < process; i++){
        MYSQL * mysql;
        if((mysql = mysql_init(NULL)) == NULL){
    	    fout << "mysql_init failed" << endl;
		    exit(-1);
	    }
	    mysql_set_character_set(mysql, "gbk");

	    if(mysql_real_connect(mysql, "localhost", "root", "root123", "test", 0, NULL, 0) == NULL){
		    fout << "mysql_real_connect failed(" << mysql_error(mysql) <<  ")" << endl;
		    exit(-1);
	    }

	    ss.str("");
	    ss << "insert into dbtest4 values (" << pidSets[i] << ", '" << timeSets[i] << "', '" << randomStrSets[i] << "')";
	    cout << ss.str();
	    if(mysql_query(mysql, ss.str().c_str())){
		    fout << "mysql_query failed(" << mysql_error(mysql) << ")" << endl;
	    }
	    mysql_close(mysql);
    }
    close(socketfd);
    exit(0);
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
        createServer(portNumber);
    }
}
