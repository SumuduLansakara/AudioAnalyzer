#pragma once

#include <string>

class runtime
{
public:
    static void init(const std::string& exePath);
    static runtime* get_instance();

    runtime(const runtime& orig) = delete;
    void operator=(const runtime& orig) = delete;

    const std::string& get_instance_ID() const;
    std::string get_path_from_root(const std::string& fileName) const;

private:
    runtime(const std::string& exePath);
    ~runtime();

    std::string mParentDir;
    std::string mInstanceID;

    static runtime* sInstance;
};