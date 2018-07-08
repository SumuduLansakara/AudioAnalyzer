#include "logger.h"

#include <cassert>
#include <iostream>
#include <map>

using std::cout;
using std::cerr;
using std::endl;
using std::map;
using std::string;

const int logger::LEVEL_CRITICAL = 50;
const int logger::LEVEL_ERROR = 40;
const int logger::LEVEL_WARNING = 30;
const int logger::LEVEL_INFO = 20;
const int logger::LEVEL_DEBUG = 10;
const int logger::LEVEL_NOTSET = 0;

const map<int, string> LEVEL_TAGS = {
    {logger::LEVEL_CRITICAL, "[CRITICAL"},
    {logger::LEVEL_ERROR, "[ERROR]"},
    {logger::LEVEL_WARNING, "[WARNING]"},
    {logger::LEVEL_INFO, "[INFO]"},
    {logger::LEVEL_DEBUG, "[DEBUG]"},
    {logger::LEVEL_NOTSET, ""}
};

logger* logger::sInstance = nullptr;

logger::logger(const string& logpath, int logLevel) : mLogfile{}, mLogLevel{logLevel}
{
    mLogfile.open(logpath, std::ios_base::app);
}

string logger::get_timestamp() const
{
    char buff[20];
    struct tm *sTm;

    time_t now = time(0);
    sTm = gmtime(&now);

    strftime(buff, sizeof (buff), "%Y-%m-%d %H:%M:%S", sTm);
    return string(buff);
}

logger::~logger()
{
    mLogfile.close();
}

int logger::get_log_level() const
{
    return mLogLevel;
}

void logger::set_log_level(int logLevel)
{
    mLogLevel = logLevel;
}

void logger::write(int level, const string& txt)
{
    if (mLogLevel <= level) {
        const string timestamp = get_timestamp();
        cout << timestamp << " " << LEVEL_TAGS.at(level) << " " << txt << endl;
        mLogfile << timestamp;
        mLogfile << " ";
        mLogfile << LEVEL_TAGS.at(level);
        mLogfile << " ";
        mLogfile << txt;
        mLogfile << endl;
    }
}

// static method definitions

void logger::init(const std::string& logpath, int logLevel)
{
    assert(sInstance == nullptr);
    sInstance = new logger(logpath, logLevel);
    sInstance->mLogfile << endl;
}

logger* logger::get_instance()
{
    if (not sInstance) {
        throw std::runtime_error("logger not initialized!");
    }
    return sInstance;
}

void logger::debug(const string& msg)
{
    sInstance->write(LEVEL_DEBUG, msg);
}

void logger::info(const string& msg)
{
    sInstance->write(LEVEL_INFO, msg);
}

void logger::warning(const string& msg)
{
    sInstance->write(LEVEL_WARNING, msg);
}

void logger::error(const string& msg)
{
    sInstance->write(LEVEL_ERROR, msg);
}

void logger::critical(const string& msg)
{
    sInstance->write(LEVEL_CRITICAL, msg);
}
