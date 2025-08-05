//
// Created by rose on 2025/8/6.
//

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include "file_writer.h"
#include "utils.h"

bool Writer::Init(std::string& fileName, int len) {
    LOGE("init begin ");
    if (access(fileName.c_str(), W_OK) != 0) {
        LOGE("No write permission: %s", strerror(errno));
    }
    auto lfd = open(fileName.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0644);
    fd = lfd;
    if (fd == -1) {
        LOGE("mmap open path error %s %s", fileName.c_str(), strerror(errno));
        return false;
    }
    if (ftruncate(fd, len) == -1) {
        LOGE("ftruncated fd %d, error file:%s, size:%d, error msg: %s", fd, fileName.c_str(), len , strerror(errno));
        close(fd);
        return false;
    }
    fileLen = len;
    void *mmLocal =  mmap(nullptr, len, PROT_READ | PROT_WRITE,
                          MAP_SHARED, fd, 0);
    if (mmLocal == MAP_FAILED) {
        LOGE("mmap failed %s", strerror(errno));
        close(fd);
        return false;
    }
    mmapPointer = mmLocal;
    LOGD("init successfully filePath=%s\nfileLen=%d\n",fileName.c_str(), len);
    return true;
}

bool Writer::Destroy() {
    LOGE("destroy invoked");
    if (writeLen > 0) {
        if (msync(mmapPointer, writeLen,MS_SYNC) == -1) {
            LOGE("destroy msync failed %s", strerror(errno));
        } else {
            LOGE("destroy msync successfully size: %d", writeLen);
        }
    }
    if (writeLen > 0 && ftruncate(fd, writeLen) == -1) {
        LOGE("destroy ftruncate to actual size failed %s", strerror(errno));
    }
    if (fd != -1) {
        int res = close(fd);
        if (res == -1) {
            LOGE("destroy close failed %s", strerror(errno));
        }
    }
    if (mmapPointer != nullptr) {
        int r = munmap(mmapPointer, fileLen);
        if (r == -1) {
            LOGE("destroy munmap failed %s", strerror(errno));
        }
    }
    mmapPointer = nullptr;
    fileLen = 0;
    fd = -1;
    writeLen = 0;
    LOGD("destroy successfully");
    return true;
}

//struct LogEntry {
//    std::string pname;
//    long timestamp;
//    std::string methodName;
//    Type type;
//};

void Writer::Write(LogEntry& logEntry) {
    char* begin = reinterpret_cast<char *>(reinterpret_cast<uint64_t>(mmapPointer) + writeLen);
    // pname
    writeString(begin, logEntry.pname);
    // timestamp
    writeLong(begin, logEntry.timestamp);
    // methodName
    writeString(begin, logEntry.methodName);
    // type
    writeInt8(begin, logEntry.type);


}

void Writer::writeString(char* &begin,std::string& pname) {
    auto pLen = pname.size();
    auto pdata = pname.data();
    *(int8_t*)(begin) = pLen;
    begin += sizeof(int8_t);
    memcpy(begin, pdata,pLen);
    begin += pLen;
    writeLen += sizeof(int8_t) + pLen;
}

void Writer::writeLong(char* &begin, long v) {
    *(long *)(begin) = v;
    begin += sizeof(long);
    writeLen += sizeof(long);
}

void Writer::writeInt8(char* &begin, int8_t v) {
   *(int8_t*)(begin) = v;
    begin += sizeof(int8_t);
    writeLen += sizeof(int8_t);
}

