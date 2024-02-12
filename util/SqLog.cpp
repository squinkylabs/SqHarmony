
#include <cstdarg>
#include <cstdio>

#include "SqLog.h"

namespace SqLog {

#ifdef __PLUGIN

#if 0
#include "logger.hpp"

    void
    log(Level level, const char* filename, int line, const char* format, ...) {
    rack::logger::log(level, filename, line, format, ...);
}
#endif

#else

static const int levelColors[] = {
    35,
    34,
    33,
    31};

static const char* const levelLabels[] = {
    "debug",
    "info",
    "warn",
    "fatal"};

//#define _TO_LOG_FILE

#ifdef _TO_LOG_FILE
static FILE* fp = nullptr;
#endif

static void logVa(Level level, const char* filename, int line, const char* format, va_list args) {
#ifdef _TO_LOG_FILE
    if (fp == nullptr) {
        fopen_s(&fp, "d:\\Rack\\plugins\\SqHarmony\\log.log", "w");
        fprintf(stderr, "log open = %p\n", fp);
    }
    auto outputFile = fp;
#else
    FILE* outputFile = stderr;

    fprintf(outputFile, "\x1B[%dm", levelColors[level]);
    fprintf(outputFile, "[%s %s:%d] ", levelLabels[level], filename, line);
    fprintf(outputFile, "\x1B[0m");
#endif
    vfprintf(outputFile, format, args);
    fprintf(outputFile, "\n");
    fflush(outputFile);
}

int errorCount = 0;

void log(Level level, const char* filename, int line, const char* format, ...) {
    if (level > INFO_LEVEL) {
        errorCount++;
    }
    va_list args;
    va_start(args, format);
    logVa(level, filename, line, format, args);
    va_end(args);
}

#endif

}  // namespace SqLog