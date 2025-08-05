//
// Created by rose on 2025/8/6.
//

#pragma once

#include "logger_entry.h"
#include <string>

class Writer {
private:
    int fd = -1;
    void* mmapPointer = nullptr;
    int fileLen = -1;
    int writeLen = 0;

public:
    bool Init(std::string& fileName, int len);
    bool Destroy();
    void Write(LogEntry& logEntry);

    void writeString(char *&begin, std::string& pname);

    void writeLong(char *&begin, long v);

    void writeInt8(char *&begin, int8_t v);
};

