//
// Created by rose on 2025/8/6.
//

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include "file_writer.h"
#include "utils.h"

bool Writer::Init(std::string& fileName, int len) {
    auto lfd = open(fileName.c_str(), O_RDWR | O_CREAT, 0644);
    if (fd == 0) {
        LOGE("mmap open path error %s", strerror(errno));
        return false;
    }
    fd = lfd;
    if (ftruncate(fd, len) == -1) {
        LOGE("ftruncated error size %s", strerror(errno));
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
    if (fd != -1) {
        int res = close(fd);
        if (res == -1) {
            LOGE("close failed %s", strerror(errno));
        }
    }
    if (mmapPointer == nullptr) {
        int r = munmap(mmapPointer, fileLen);
        if (r == -1) {
            LOGE("munmap failed %s", strerror(errno));
        }
    }
    LOGD("destroy successfully");
    return true;
}

void Writer::Write() {

}