#pragma once

#include <cstdint>
#include <string>
#include <memory>

struct FileMeta
{
    std::string name;     // filename only
    std::string path;     // full absolute path
    std::uint64_t size = 0;   // file size in bytes
    std::uint64_t itemCount = 0; // number of items if directory
    std::uint64_t mtime = 0;  // last modified time (epoch seconds)
    std::uint64_t ctime = 0;  // creation time (epoch seconds)
    std::uint64_t atime = 0;  // last access time (epoch seconds)
    bool isDir = false;       // is directory
    std::string thumbnailPath;
    
    // Advanced - Shared pointers to avoid duplicating identical strings (e.g. "rwxr-xr-x", "root")
    std::uint32_t mode = 0;
    std::shared_ptr<const std::string> permissions;
    std::shared_ptr<const std::string> owner;
    std::shared_ptr<const std::string> group;
    std::shared_ptr<const std::string> mimeType;
};
