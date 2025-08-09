//
// Created by rose on 2025/8/4.
//

#pragma once
#include <thread>
#include "faster/lock_free_queue.h"
#include "faster/logger_entry.h"

void InitTrace();

void StartTrace();

void StopTrace();

static std::thread threadInstance;

