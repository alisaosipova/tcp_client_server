#pragma once
#include <fstream>
#include <mutex>

class messageLogger
{
private:
    std::ofstream file;
    std::mutex file_mutex;

public:
    messageLogger(const char* file_name);
    ~messageLogger();
    void saveMessage(const char* client_message);
};