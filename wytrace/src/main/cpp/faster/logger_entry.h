//
// Created by rose on 2025/8/4.
//

#pragma once
#include <cstdint>
#include "string"

enum Type : int8_t {
    TRACE_BEGIN,
    TRACE_END
};

struct LogEntry {
    std::string pname;
    long timestamp;
    std::string methodName;
    Type type;
};
