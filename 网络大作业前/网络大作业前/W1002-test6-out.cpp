//
// Created by liuyang on 16-5-6.
//

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <zconf.h>
#include <sys/stat.h>
#include <sys/param.h>

using namespace std;

struct Config{
    string principle;
    string direction;
    int listenPorts;
    string inAddress;
    int inPorts;
};

void ReadFile(vector<Config> & configs);
void initDaemon();
int CheckPort(string Port);
bool GetPorts(string Port, vector<int> &Ports);

int main(int argc, char * argv[]){
    //pid_t pid = fork();
    //if(pid == -1){cout << "Fork Error" << endl; exit(EXIT_FAILURE);}
    //else if (pid > 0){
        //Parent
    //    exit(0);
    //} else if (pid == 0){
    //    initDaemon();
        vector<Config> configs;
        ReadFile(configs);
        for(vector<Config>::iterator it = configs.begin(); it != configs.end(); it++){
            cout << it->principle << ' ' << it->direction << ' ' << it->listenPorts << ' ' << it->inAddress << ' ' << it->inPorts << endl;
        }
    //}
}

void ReadFile(vector<Config> & configs){
    ifstream fin("/etc/ipproxy.conf");
    if(fin.peek() == ifstream::traits_type::eof()){
        cout << "Empty Files" << endl;
        return;
    }
    stringstream ss("");
    while(!fin.eof()) {
        ss.str("");
        char line[1024];
        fin.getline(line, 1024);
        for(int i = 0; line[i] != '\0'; i++)
            if(line[i] == '#') { line[i] = '\0'; break; }
        ss << line;
        if(ss.str().size() == 0) continue;
        Config config;
        string listenPorts;
        string inPorts;
        ss >> config.principle;
        ss >> config.direction;
        ss >> listenPorts;
        ss >> config.inAddress;
        ss >> inPorts;
        if(CheckPort(listenPorts) == 1 && CheckPort(inPorts) == 1){
            config.listenPorts = atoi(listenPorts.c_str());
            config.inPorts = atoi(inPorts.c_str());
            configs.push_back(config);
        } else if(CheckPort(listenPorts) == 0 && CheckPort(inPorts) == 0){
            vector<int> listenPortsVector;
            vector<int> inPortsVector;
            if(!GetPorts(listenPorts, listenPortsVector)|| !GetPorts(inPorts, inPortsVector)) continue;
            if(listenPortsVector.size() != inPortsVector.size()) continue;
            Config tempConfig;
            tempConfig.principle = config.principle;
            tempConfig.direction = config.direction;
            tempConfig.inAddress = config.inAddress;
            for(int i = 0; i < listenPortsVector.size(); i++){
                tempConfig.listenPorts = listenPortsVector[i];
                tempConfig.inPorts = inPortsVector[i];
                configs.push_back(tempConfig);
            }
        } else continue;
    }
    for(vector<Config>::iterator it = configs.begin(); it != configs.end(); it++){
        transform(it->principle.begin(), it->principle.end(), it->principle.begin(), ::toupper);
        transform(it->direction.begin(), it->direction.end(), it->direction.begin(), ::toupper);
    }
}

void initDaemon(){
    setsid();
    umask(0);
    for(int i = 0; i < NOFILE; i++){
        close(i);
    }
}

int CheckPort(string in){
    //if 'in' is a port, return 1
    //if 'in' is a port-port return 0
    //if 'in' is invalid return -1
    for(int i = 0; i < in.size(); i++){
        if(in[i] == '-') return 0;
        if(in[i] <= '9' && in[i] >= '0') continue;
        return -1;
    }
    return true;
}

bool GetPorts(string Port, vector<int> &Ports){
    // From port-port -> [port, port, port]
    // Example: GetPorts("1234-1236", Ports); You will get [1234, 1235, 1236] in Ports
    // @return: if Okey, return true. if error occurs, return false. example:"1234-1232"
    char port1[6];
    char port2[6];
    int i = 0;
    for(i = 0; Port[i] != '-'; i++)
        port1[i] = Port[i];
    port1[i] = '\0';
    int j;
    for(j = 0; Port[j + i + 1] != '\0'; j++) port2[j] = Port[j + i + 1];
    port2[j] = '\0';
    int port_min = atoi(port1);
    int port_max = atoi(port2);
    if(port_max < port_min) return false;
    Ports.clear();
    for(i = port_min; i <= port_max; i++)
        Ports.push_back(i);
    return true;
}
