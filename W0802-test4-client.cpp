#include <iostream>
#include <fcntl.h>
#include <cstdlib>
#include <zconf.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <cstring>
#include <arpa/inet.h>
#include <fstream>
#include <sstream>

using namespace std;

void initDaemon();

void createClient(char* ipaddr, int PortNumber);

int main(int argc, char * argv[]){
    if(argc < 3){
        cout << "Usage: test1-client <IP_ADDR> <PORT_NUMBER>" << endl;
        exit(0);
    }

    const int process = 500;

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
        initDaemon();

        for(int i = 0; i < process; i++) {
            pid = fork();
            if (pid < 0) {
                perror("Fork Error");
                exit(0);
            } else if (pid > 0) {
                continue;
            } else if (pid == 0) {
                createClient(ipaddr, portNumber);
            }
        }
    }
}

void initDaemon(){
    setsid();
    umask(0);
    for(int i = 0; i < NOFILE; i++){
        close(i);
    }
}

void createClient(char* ipaddr, int PortNumber) {
    int socketfd;
    uint32_t mypid = getpid();
    uint32_t mypid_network = htonl(mypid);
    stringstream ss("");
    ss << mypid << ".pid.txt";
    ofstream fout(ss.str().c_str());
    if ((socketfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        fout << "Cannot create socket" << endl;
        exit(0);
    }

    fcntl(socketfd, O_NONBLOCK);


    sockaddr_in myaddr;
    sockaddr_in remaddr;
    socklen_t slen = sizeof(myaddr);
    memset(&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(0);
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(socketfd, (sockaddr *) &myaddr, sizeof(myaddr)) < 0) {
        fout << "bind failed" << endl;
        exit(0);
    }

    memset(&remaddr, 0, sizeof(remaddr));
    remaddr.sin_family = AF_INET;
    remaddr.sin_port = htons(PortNumber);
    if(inet_aton(ipaddr, &remaddr.sin_addr) == 0){
        fout << "inet_aton() failed" << endl;
        exit(1);
    }

    const int BUFSIZE = 10240;
    char buf[BUFSIZE];

    //Send PID
    if(sendto(socketfd, &mypid_network, 4, 0, (sockaddr*)&remaddr, slen) == -1){
        fout << "sendto failed" << endl;
        exit(1);
    }

    //Get time & Send time
    time_t now = time(0);
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
    string time_str(buf);
    if(sendto(socketfd, buf, 19, 0, (sockaddr*)&remaddr, slen) == -1){
        fout << "sendto failed" << endl;
        exit(1);
    }

    int recvlen;
    int serverRand;

    for(;;){
        recvlen = recvfrom(socketfd, buf, BUFSIZE, 0, (sockaddr*)&remaddr, &slen);
        if(recvlen <= 0) continue;
        char serverRandom[5];
        serverRandom[0] = buf[3];
        serverRandom[1] = buf[4];
        serverRandom[2] = buf[5];
        serverRandom[3] = buf[6];
        serverRandom[4] = '\0';
        serverRand = atoi(serverRandom);
        break;
    }

    const char * charPool = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const int charNum = 52;
    srand(time(NULL) + mypid);
    for(int i = 0; i < serverRand; i++){ buf[i] = charPool[rand()%charNum]; }
    buf[serverRand] = '\0';

    string randomStr(buf);

    if(sendto(socketfd, buf, strlen(buf), 0, (sockaddr*)&remaddr, slen) == -1){
        fout << "sendto failed" << endl;
        exit(1);
    }

    for(;;){
        recvlen = recvfrom(socketfd, buf, BUFSIZE, 0, (sockaddr*)&remaddr, &slen);
        if(recvlen <= 0) continue;
        break;
    }

    fout << mypid << endl << time_str << endl << randomStr << flush;
    close(socketfd);

    exit(0);
}
