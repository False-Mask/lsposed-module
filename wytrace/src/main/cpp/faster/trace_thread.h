//
// Created by rose on 2025/8/20.
//

#ifndef LSPOSED_TOOLS_TRACE_THREAD_H
#define LSPOSED_TOOLS_TRACE_THREAD_H


#include "logger_entry.h"
#include "lock_free_queue.h"
#include <barrier>

class TraceThread {
public:
    void traceLoop(int len, int threadKey, std::barrier<>* barrier);

private:
    void consumerLoop(int len,int threadKey, std::barrier<>* barrier);

    void process(int len, int key, Writer& w, LogEntry& entry);

    void mergeAllFiles(int len, int key);

    void makeFileName(int key, std::string& std);
};


#endif //LSPOSED_TOOLS_TRACE_THREAD_H
