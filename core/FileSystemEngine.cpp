#include "FileSystemEngine.hpp"
#include "FileMeta.hpp"

#include <QMimeDatabase>
#include <QMimeType>
#include <QString>

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
        
        std::error_code entry_st_ec;
        auto st = entry.status(entry_st_ec); // follow symlink to get target status
        if (entry_st_ec) {
            st = entry.symlink_status(entry_st_ec); // fallback to symlink status
        }
        if (entry_st_ec) continue;

        FileMeta m;
        m.name = entry.path().filename().string();
        m.path = fs::absolute(entry.path()).string();
        
        m.isDir = fs::is_directory(st);
        
        if (m.isDir) {
            m.itemCount = 0; // Removed expensive counting for performance
            m.size = 0;
        } else if (fs::is_regular_file(st)) {
            std::error_code sz_ec;
            m.size = fs::file_size(entry.path(), sz_ec);
            m.itemCount = 0;
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
        }
        
        results.push_back(std::move(m));
        if (limit > 0 && results.size() >= limit) break;
    }
    return results;
}

FileMeta FileSystemEngine::getFileMeta(const std::string& path, bool includeMime)
{
    FileMeta m;
    fs::path p(path);
    m.name = p.filename().string();
    m.path = path;

    std::error_code ec;
    auto st = fs::symlink_status(p, ec);
    if (ec) return m;

    m.isDir = fs::is_directory(st);
    if (m.isDir) {
        m.size = 0;
        m.itemCount = 0;
        static const std::string inodeDir = "inode/directory";
        m.mimeType = std::make_shared<const std::string>(inodeDir);
    } else {
        std::error_code sz_ec;
        m.size = fs::file_size(p, sz_ec);
        m.itemCount = 0;
        if (includeMime) {
            static QMimeDatabase db;
            std::string mimeStr = db.mimeTypeForFile(QString::fromStdString(path)).name().toStdString();
            m.mimeType = std::make_shared<const std::string>(std::move(mimeStr));
        }
    }

    struct stat info;
    if (stat(path.c_str(), &info) == 0) {
        m.mode = static_cast<std::uint32_t>(info.st_mode);
        m.permissions = get_perms_shared(info.st_mode);
        m.owner = internString(std::to_string(info.st_uid));
        m.group = internString(std::to_string(info.st_gid));
        m.mtime = static_cast<std::uint64_t>(info.st_mtime);
        m.atime = static_cast<std::uint64_t>(info.st_atime);
        m.ctime = static_cast<std::uint64_t>(info.st_ctime);
    }
    return m;
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
        std::error_code entry_st_ec;
        auto st = entry.status(entry_st_ec); // follow symlink to get target status
        if (entry_st_ec) {
            st = entry.symlink_status(entry_st_ec); // fallback to symlink status
        }
        if (entry_st_ec) continue;

        FileMeta m;
        m.name = entry.path().filename().string();
        // Path optimization: use the already known parent directory path
        m.path = (fs::path(path) / m.name).string();

        m.isDir = fs::is_directory(st);

        if (m.isDir) {
            m.itemCount = 0; // Removed expensive counting for performance
            m.size = 0;
            static const std::string inodeDir = "inode/directory";
            m.mimeType = std::make_shared<const std::string>(inodeDir);
        } else if (fs::is_regular_file(st)) {
            std::error_code sz_ec;
            m.size = fs::file_size(entry.path(), sz_ec);
            if (sz_ec) m.size = 0;
            
            static QMimeDatabase db;
            std::string mimeStr = db.mimeTypeForFile(QString::fromStdString(m.path)).name().toStdString();
            m.mimeType = std::make_shared<const std::string>(std::move(mimeStr));
            m.itemCount = 0;
        } else {
            m.size = 0;
            m.itemCount = 0;
        }

        struct stat info;
        if (stat(m.path.c_str(), &info) == 0) {
            m.mode = static_cast<std::uint32_t>(info.st_mode);
            m.permissions = get_perms_shared(info.st_mode);
            m.owner = internString(std::to_string(info.st_uid));
            m.group = internString(std::to_string(info.st_gid));
            m.mtime = static_cast<std::uint64_t>(info.st_mtime);
            m.atime = static_cast<std::uint64_t>(info.st_atime);
            m.ctime = static_cast<std::uint64_t>(info.st_ctime);
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

// Search
void FileSystemEngine::searchRecursive(
    const std::string &rootPath,
    const std::string &pattern,
    const std::function<void(std::vector<FileMeta>&&)>& onBatch,
    const std::function<bool()>& shouldCancel
) {
    // Basic search implementation
    std::error_code ec;
    fs::path root(rootPath);
    if (!fs::exists(root, ec) || !fs::is_directory(root, ec)) return;

    std::vector<FileMeta> batch;
    batch.reserve(50);

    for (auto it = fs::recursive_directory_iterator(root, fs::directory_options::skip_permission_denied, ec);
         it != fs::recursive_directory_iterator();
         it.increment(ec)) 
    {
        if (ec) { ec.clear(); continue; }
        if (shouldCancel()) return;

        const auto& entry = *it;
        std::string name = entry.path().filename().string();
        
        // Simple case-insensitive search
        auto it_p = std::search(
            name.begin(), name.end(),
            pattern.begin(), pattern.end(),
            [](char ch1, char ch2) { return std::tolower(ch1) == std::tolower(ch2); }
        );

        if (it_p != name.end()) {
            FileMeta m;
            m.name = name;
            m.path = fs::absolute(entry.path()).string();
            m.isDir = entry.is_directory();
            
            struct stat info;
            if (stat(m.path.c_str(), &info) == 0) {
                m.size = static_cast<std::uint64_t>(info.st_size);
                m.mtime = static_cast<std::uint64_t>(info.st_mtime);
            }

            batch.push_back(std::move(m));
            if (batch.size() >= 50) {
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

FileSystemEngine::OperationResult FileSystemEngine::createFile(const std::string &path)
{
    FILE* f = fopen(path.c_str(), "w");
    if (f) {
        fclose(f);
        return {true, ""};
    }
    return {false, std::strerror(errno)};
}

static void copyRecursive(const fs::path& src, const fs::path& dest, std::uint64_t totalSize, std::uint64_t& copiedSize, const std::function<void(float)>& onProgress, std::error_code& ec) {
    if (fs::is_directory(src, ec)) {
        fs::create_directories(dest, ec);
        if (ec) return;
        for (const auto& entry : fs::directory_iterator(src, fs::directory_options::skip_permission_denied, ec)) {
            if (ec) break;
            copyRecursive(entry.path(), dest / entry.path().filename(), totalSize, copiedSize, onProgress, ec);
            if (ec) return;
        }
    } else if (fs::is_regular_file(src, ec)) {
        FILE* in = fopen(src.string().c_str(), "rb");
        if (!in) { ec = std::make_error_code(std::errc::no_such_file_or_directory); return; }
        
        FILE* out = fopen(dest.string().c_str(), "wb");
        if (!out) { fclose(in); ec = std::make_error_code(std::errc::permission_denied); return; }
        
        char buffer[8192];
        size_t bytesRead;
        while ((bytesRead = fread(buffer, 1, sizeof(buffer), in)) > 0) {
            fwrite(buffer, 1, bytesRead, out);
            copiedSize += bytesRead;
            if (onProgress && totalSize > 0) {
                onProgress(static_cast<float>(copiedSize) / static_cast<float>(totalSize));
            }
        }
        
        fclose(in);
        fclose(out);
        
        // Copy permissions
        std::error_code p_ec;
        fs::permissions(dest, fs::status(src).permissions(), fs::perm_options::replace, p_ec);
    } else {
        fs::copy(src, dest, fs::copy_options::overwrite_existing, ec);
    }
}

FileSystemEngine::OperationResult FileSystemEngine::copyPath(
    const std::string &src, 
    const std::string &dest, 
    const std::function<void(float)>& onProgress
)
{
    std::error_code ec;
    fs::path srcPath(src);
    fs::path destPath(dest);
    
    if (!fs::exists(srcPath, ec)) return {false, "Source does not exist"};
    
    std::uint64_t totalSize = 0;
    if (fs::is_regular_file(srcPath, ec)) {
        totalSize = fs::file_size(srcPath, ec);
    } else if (fs::is_directory(srcPath, ec)) {
        for (auto const& entry : fs::recursive_directory_iterator(srcPath, fs::directory_options::skip_permission_denied, ec)) {
            if (ec) { ec.clear(); continue; }
            if (fs::is_regular_file(entry, ec)) totalSize += fs::file_size(entry, ec);
        }
    }
    
    ec.clear();
    std::uint64_t copiedSize = 0;
    copyRecursive(srcPath, destPath, totalSize, copiedSize, onProgress, ec);
    
    if (ec) return {false, ec.message()};
    if (onProgress) onProgress(1.0f);
    return {true, ""};
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

#include <QProcess>
#include <QStringList>

FileSystemEngine::OperationResult FileSystemEngine::moveToTrash(const std::string &path)
{
    // Use gio trash which handles files safely on Linux desktops
    int ret = QProcess::execute("gio", {"trash", QString::fromStdString(path)});
    if (ret == 0) return {true, ""};
    return {false, "Failed to move to trash (gio error " + std::to_string(ret) + ")"};
}
