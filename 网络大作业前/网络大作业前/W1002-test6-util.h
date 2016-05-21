//
// Created by liuyang on 16-5-7.
//

#ifndef TEST6_UTIL_H
#define TEST6_UTIL_H

#include <iostream>
#include <cstring>
using namespace std;

const std::string getCurrentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);

    return buf;
}

const string Log(const char * sentence){
    stringstream ss("");
    ss << "[" << getCurrentDateTime() << "] " << sentence;
    return ss.str();
}

#endif
