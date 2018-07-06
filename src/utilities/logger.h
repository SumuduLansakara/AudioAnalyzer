#pragma once
#include <fstream>
#include <string>
#include <map>

class logger
{
public:

    static logger* get_instance();
    static void init(const std::string& logpath, int logLevel);

    static void debug(const std::string& msg);
    static void info(const std::string& msg);
    static void warning(const std::string& msg);
    static void error(const std::string& msg);
    static void critical(const std::string& msg);

    logger(const logger& orig) = delete;
    void operator=(const logger& orig) = delete;

    int get_log_level() const;
    void set_log_level(int logLevel);

    std::string get_timestamp() const;

private:
    logger(const std::string& logpath, int logLevel);
    ~logger();

    void write(int level, const std::string& txt);

    std::ofstream mLogfile;
    int mLogLevel;
    static logger* sInstance;

public:
    static const int LEVEL_CRITICAL;
    static const int LEVEL_ERROR;
    static const int LEVEL_WARNING;
    static const int LEVEL_INFO;
    static const int LEVEL_DEBUG;
    static const int LEVEL_NOTSET;
};
