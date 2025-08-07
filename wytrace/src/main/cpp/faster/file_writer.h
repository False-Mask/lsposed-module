//
// Created by rose on 2025/8/6.
//

#ifndef LSPOSED_TOOLS_FILE_WRITER_H
#define LSPOSED_TOOLS_FILE_WRITER_H

#include <string>
class Writer {
private:
    int fd = -1;
    void* mmapPointer = nullptr;
    int fileLen = -1;

public:
    bool Init(std::string& fileName, int len);
    bool Destroy();
    void Write();
};


#endif //LSPOSED_TOOLS_FILE_WRITER_H
