//
// Created by liuyang on 16-5-6.
//

#include <fstream>
#include <iostream>
#include <sstream>
#include <cstring>
#include <vector>
#include <algorithm>

using namespace std;

struct Config{
    string principle;
    string direction;
    string listenPorts;
    string inAddress;
    string inPorts;
};

void ReadFile(vector<Config> & configs);

int main(int argc, char * argv[]){
    vector<Config> configs;
    ReadFile(configs);
    for(vector<Config>::iterator it = configs.begin(); it != configs.end(); it++){
        transform(it->principle.begin(), it->principle.end(), it->principle.begin(), ::toupper);
        transform(it->direction.begin(), it->direction.end(), it->direction.begin(), ::toupper);
        cout << it->principle << ' ' << it->direction << endl;
    }
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
        ss >> config.principle;
        ss >> config.direction;
        ss >> config.listenPorts;
        ss >> config.inAddress;
        ss >> config.inPorts;
        configs.push_back(config);
    }
}