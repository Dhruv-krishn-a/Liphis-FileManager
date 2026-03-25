#include "FileSystemEngine.hpp"
#include "FileMeta.hpp"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <system_error>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <unordered_map>
#include <mutex>

namespace fs = std::filesystem;

namespace {
    std::unordered_map<std::string, std::shared_ptr<const std::string>> g_stringPool;
    std::mutex g_poolMutex;

    std::shared_ptr<const std::string> internString(const std::string& s) {
        std::lock_guard<std::mutex> lock(g_poolMutex);
        auto it = g_stringPool.find(s);
        if (it != g_stringPool.end()) return it->second;
        auto shared = std::make_shared<const std::string>(s);
        g_stringPool[s] = shared;
        return shared;
    }
}

static std::shared_ptr<const std::string> get_perms_shared(mode_t mode) {
    std::string s;
    s += (mode & S_IRUSR) ? 'r' : '-';
    s += (mode & S_IWUSR) ? 'w' : '-';
    s += (mode & S_IXUSR) ? 'x' : '-';
    s += (mode & S_IRGRP) ? 'r' : '-';
    s += (mode & S_IWGRP) ? 'w' : '-';
    s += (mode & S_IXGRP) ? 'x' : '-';
    s += (mode & S_IROTH) ? 'r' : '-';
    s += (mode & S_IWOTH) ? 'w' : '-';
    s += (mode & S_IXOTH) ? 'x' : '-';
    return internString(s);
}

static std::uint64_t file_time_to_epoch_seconds(const fs::file_time_type& ftime) {
    using namespace std::chrono;
    try {
        auto sctp = time_point_cast<system_clock::duration>(
            ftime - fs::file_time_type::clock::now() + system_clock::now()
        );
        return static_cast<std::uint64_t>(system_clock::to_time_t(sctp));
    } catch (...) {
        return 0;
    }
}

std::vector<FileMeta> FileSystemEngine::listDirectorySync(const std::string& path, std::size_t limit) {
    std::vector<FileMeta> results;
    std::error_code ec;
    fs::path dir(path);

    if (!fs::exists(dir, ec) || !fs::is_directory(dir, ec)) return results;

    for (auto const& entry : fs::directory_iterator(dir, fs::directory_options::skip_permission_denied, ec)) {
        if (ec) break;
        
        std::error_code st_ec;
        auto st = entry.symlink_status(st_ec);
        if (st_ec) continue;

        FileMeta m;
        m.name = entry.path().filename().string();
        m.path = fs::absolute(entry.path()).string();
        m.isDir = fs::is_directory(st);
        
        if (fs::is_regular_file(st)) {
            std::error_code sz_ec;
            m.size = fs::file_size(entry.path(), sz_ec);
        }

        struct stat info;
        if (stat(m.path.c_str(), &info) == 0) {
            m.mode = static_cast<std::uint32_t>(info.st_mode);
            m.permissions = get_perms_shared(info.st_mode);
            m.owner = internString(std::to_string(info.st_uid));
            m.group = internString(std::to_string(info.st_gid));
            m.mtime = static_cast<std::uint64_t>(info.st_mtime);
        }
        
        results.push_back(std::move(m));
        if (limit > 0 && results.size() >= limit) break;
    }
    return results;
}

void FileSystemEngine::listDirectoryStream(
    const std::string &path,
    std::size_t batchSize,
    const std::function<void(std::vector<FileMeta>&&)>& onBatch,
    const std::function<bool()>& shouldCancel
)
{
    std::error_code ec;
    fs::path dir(path);

    if (!fs::exists(dir, ec) || ec) return;
    if (!fs::is_directory(dir, ec) || ec) return;

    std::vector<FileMeta> batch;
    batch.reserve(batchSize);

    for (auto it = fs::directory_iterator(dir, fs::directory_options::skip_permission_denied, ec);
         it != fs::directory_iterator();
         it.increment(ec))
    {
        if (ec) break;
        if (shouldCancel()) return;

        const auto &entry = *it;
        std::error_code st_ec;
        auto st = entry.symlink_status(st_ec);
        if (st_ec) continue;

        FileMeta m;
        m.name = entry.path().filename().string();
        std::error_code abs_ec;
        m.path = fs::absolute(entry.path(), abs_ec).string();

        if (fs::is_symlink(st)) {
             std::error_code target_ec;
             auto target_st = fs::status(entry.path(), target_ec);
             m.isDir = !target_ec && fs::is_directory(target_st);
        } else {
             m.isDir = fs::is_directory(st);
        }

        if (fs::is_regular_file(st)) {
            std::error_code size_ec;
            m.size = fs::file_size(entry.path(), size_ec);
            if (size_ec) m.size = 0;
        } else {
            m.size = 0;
        }

        struct stat info;
        if (stat(m.path.c_str(), &info) == 0) {
            m.mode = static_cast<std::uint32_t>(info.st_mode);
            m.permissions = get_perms_shared(info.st_mode);
            m.owner = internString(std::to_string(info.st_uid));
            m.group = internString(std::to_string(info.st_gid));
            m.mtime = static_cast<std::uint64_t>(info.st_mtime);
            m.ctime = static_cast<std::uint64_t>(info.st_ctime);
            m.atime = static_cast<std::uint64_t>(info.st_atime);
        } else {
            std::error_code time_ec;
            auto ftime = fs::last_write_time(entry.path(), time_ec);
            m.mtime = time_ec ? 0 : file_time_to_epoch_seconds(ftime);
            m.ctime = m.mtime;
            m.atime = m.mtime;
        }

        batch.push_back(std::move(m));

        if (batch.size() >= batchSize) {
            onBatch(std::move(batch));
            batch.clear();
            batch.reserve(batchSize);
        }
    }

    if (!batch.empty() && !shouldCancel()) {
        onBatch(std::move(batch));
    }
}

void FileSystemEngine::searchRecursive(
    const std::string &rootPath,
    const std::string &pattern,
    const std::function<void(std::vector<FileMeta>&&)>& onBatch,
    const std::function<bool()>& shouldCancel
)
{
    std::error_code ec;
    fs::path dir(rootPath);
    if (!fs::exists(dir, ec) || !fs::is_directory(dir, ec)) return;

    std::vector<FileMeta> batch;
    const std::size_t batchSize = 50;
    
    std::string lowerPattern = pattern;
    std::transform(lowerPattern.begin(), lowerPattern.end(), lowerPattern.begin(), ::tolower);

    for (auto it = fs::recursive_directory_iterator(dir, fs::directory_options::skip_permission_denied, ec);
         it != fs::recursive_directory_iterator();
         it.increment(ec))
    {
        if (ec) { ec.clear(); continue; }
        if (shouldCancel()) return;

        const auto &entry = *it;
        std::string name = entry.path().filename().string();
        std::string lowerName = name;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

        if (lowerName.find(lowerPattern) != std::string::npos) {
            FileMeta m;
            m.name = name;
            m.path = fs::absolute(entry.path()).string();
            
            std::error_code st_ec;
            auto st = entry.symlink_status(st_ec);
            if (st_ec) continue;

            m.isDir = fs::is_directory(st);
            if (fs::is_regular_file(st)) {
                std::error_code sz_ec;
                m.size = fs::file_size(entry.path(), sz_ec);
            }
            
            std::error_code t_ec;
            auto ftime = fs::last_write_time(entry.path(), t_ec);
            m.mtime = t_ec ? 0 : file_time_to_epoch_seconds(ftime);

            batch.push_back(std::move(m));
            if (batch.size() >= batchSize) {
                onBatch(std::move(batch));
                batch.clear();
            }
        }
    }
    if (!batch.empty() && !shouldCancel()) onBatch(std::move(batch));
}

// --- FILE OPERATIONS ---

FileSystemEngine::OperationResult FileSystemEngine::renamePath(const std::string &oldPath, const std::string &newPath)
{
    std::error_code ec;
    fs::rename(oldPath, newPath, ec);
    if (ec) return {false, ec.message()};
    return {true, ""};
}

FileSystemEngine::OperationResult FileSystemEngine::deletePath(const std::string &path)
{
    std::error_code ec;
    fs::remove_all(path, ec);
    if (ec) return {false, ec.message()};
    return {true, ""};
}

FileSystemEngine::OperationResult FileSystemEngine::createDirectory(const std::string &path)
{
    std::error_code ec;
    fs::create_directories(path, ec);
    if (ec) return {false, ec.message()};
    return {true, ""};
}

FileSystemEngine::OperationResult FileSystemEngine::copyPath(
    const std::string &src, 
    const std::string &dest, 
    const std::function<void(float)>& onProgress
)
{
    std::error_code ec;
    try {
        fs::copy(src, dest, fs::copy_options::recursive | fs::copy_options::overwrite_existing, ec);
        if (ec) return {false, ec.message()};
        return {true, ""};
    } catch (const fs::filesystem_error& e) {
        return {false, e.what()};
    }
}

FileSystemEngine::OperationResult FileSystemEngine::movePath(const std::string &src, const std::string &dest)
{
    std::error_code ec;
    fs::rename(src, dest, ec);
    if (ec) return {false, ec.message()};
    return {true, ""};
}

FileSystemEngine::OperationResult FileSystemEngine::setPermissions(const std::string &path, int perms)
{
    if (chmod(path.c_str(), static_cast<mode_t>(perms)) == 0) return {true, ""};
    return {false, std::strerror(errno)};
}

FileSystemEngine::OperationResult FileSystemEngine::createSymlink(const std::string &target, const std::string &link)
{
    std::error_code ec;
    fs::create_symlink(target, link, ec);
    if (ec) return {false, ec.message()};
    return {true, ""};
}
