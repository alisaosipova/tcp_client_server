#include "messageLogger.h"

messageLogger::messageLogger(const char* file_name)
{
    file.open(file_name, std::ios::out | std::ios::app);
    if (!file.is_open()) {
        throw std::runtime_error("The file could not be opened");
    }
}

messageLogger::~messageLogger()
{
    file.close();
}


void messageLogger::saveMessage(const char* client_message)
{
    std::lock_guard<std::mutex> lock(file_mutex);

    file << client_message << '\n';
}