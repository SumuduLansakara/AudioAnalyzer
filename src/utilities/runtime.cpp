#include "runtime.h"

#include <cassert>
#include <experimental/filesystem>

using std::string;
namespace fs = std::experimental::filesystem;

runtime* runtime::sInstance = nullptr;

runtime::runtime(const string& exePath) : mParentDir{fs::canonical(fs::absolute(exePath)).parent_path()}, mInstanceID{}
{
    time_t rawtime;
    time(&rawtime);
    struct tm * timeinfo = localtime(&rawtime);
    char buffer[80];
    strftime(buffer, sizeof (buffer), "%d%m%Y%I%M%S", timeinfo);
    mInstanceID = string(buffer);
}

runtime::~runtime()
{
}

const std::string& runtime::get_instance_ID() const
{
    return mInstanceID;
}

string runtime::get_path_from_root(const std::string& fileName) const
{
    fs::path p{mParentDir};
    p /= fileName;
    return p.string();
}


// static method definitions

void runtime::init(const std::string& exePath)
{
    assert(sInstance == nullptr);
    sInstance = new runtime(exePath);
}

runtime* runtime::get_instance()
{
    if (not sInstance) {
        throw std::runtime_error("runtime not initialized!");
    }
    return sInstance;
}