#pragma  once
#include <string>
#include <stdio.h>
#include <atomic>

#ifdef NDEBUG
#define hlog(level, ...) \
    do { \
        if (level<=Logger::getLogger().getLogLevel()) { \
            Logger::getLogger().logv(level, __FILE__, __LINE__, __func__, __VA_ARGS__); \
        } \
    } while(0)
#else
#define hlog(level, ...) \
    do { \
        if (level<=Logger::getLogger().getLogLevel()) { \
            snprintf(0, 0, __VA_ARGS__); \
            Logger::getLogger().logv(level, __FILE__, __LINE__, __func__, __VA_ARGS__); \
        } \
    } while(0)

#endif

#define TRACE(...) hlog(Logger::LTRACE, __VA_ARGS__)
#define DEBUG(...) hlog(Logger::LDEBUG, __VA_ARGS__)
#define INFO(...) hlog(Logger::LINFO, __VA_ARGS__)
#define WARN(...) hlog(Logger::LWARN, __VA_ARGS__)
#define ERROR(...) hlog(Logger::LERROR, __VA_ARGS__)
#define FATAL(...) hlog(Logger::LFATAL, __VA_ARGS__)
#define FATALIF(b, ...) do { if((b)) { hlog(Logger::LFATAL, __VA_ARGS__); } } while (0)
#define CHECK(b, ...) do { if((b)) { hlog(Logger::LFATAL, __VA_ARGS__); } } while (0)
#define EXITIF(b, ...) do { if ((b)) { hlog(Logger::LERROR, __VA_ARGS__); _exit(1); }} while(0)

#define setloglevel(l) Logger::getLogger().setLogLevel(l)
#define setlogfile(n) Logger::getLogger().setFileName(n)

struct Logger {
    enum LogLevel{LFATAL=0, LERROR, LUERR, LWARN, LINFO, LDEBUG, LTRACE, LALL};
    Logger();
    ~Logger();
    void logv(int level, const char* file, int line, const char* func, const char* fmt ...);

    void setFileName(const std::string& filename);
    void setLogLevel(const std::string& level);
    void setLogLevel(LogLevel level) { level_ = std::min(LALL, std::max(LFATAL, level)); }

    LogLevel getLogLevel() { return level_; }
    const char* getLogLevelStr() { return levelStrs_[level_]; }
    int getFd() { return fd_; }

    void adjustLogLevel(int adjust) { setLogLevel(LogLevel(level_+adjust)); }
    void setRotateInterval(long rotateInterval) { rotateInterval_ = rotateInterval; }
    static Logger& getLogger();
private:
    void maybeRotate();
    static const char* levelStrs_[LALL+1];
    int fd_;
    LogLevel level_;
    long lastRotate_;
    std::atomic<int64_t> realRotate_;
    long rotateInterval_;
    std::string filename_;
};


