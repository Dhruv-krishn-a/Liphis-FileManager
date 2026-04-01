#pragma once

#include "FileMeta.hpp"
#include <string>
#include <vector>
#include <cstddef>
#include <functional>


class FileSystemEngine {
public:
    // Synchronously list a directory.
    // - `path`: directory path (absolute or relative)
    // - `limit`: 0 => no limit, otherwise stop after 'limit' entries
    // Returns vector<FileMeta> (empty on errors or if not a directory).
    static void listDirectoryStream(
        const std::string &path,
        std::size_t batchSize,
        const std::function<void(std::vector<FileMeta>&&)>& onBatch,
        const std::function<bool()>& shouldCancel
    );

    static std::vector<FileMeta> listDirectorySync(const std::string& path, std::size_t limit = 0);
    static FileMeta getFileMeta(const std::string& path);

    // Search
    static void searchRecursive(
        const std::string &rootPath,
        const std::string &pattern,
        const std::function<void(std::vector<FileMeta>&&)>& onBatch,
        const std::function<bool()>& shouldCancel
    );

    struct OperationResult {
        bool success;
        std::string message;
    };

    // File Operations
    static OperationResult renamePath(const std::string &oldPath, const std::string &newPath);
    static OperationResult deletePath(const std::string &path);
    static OperationResult createDirectory(const std::string &path);
    static OperationResult createFile(const std::string &path);
    static OperationResult copyPath(const std::string &src, const std::string &dest, const std::function<void(float)>& onProgress = nullptr);
    static OperationResult movePath(const std::string &src, const std::string &dest);
    static OperationResult setPermissions(const std::string &path, int perms);
    static OperationResult createSymlink(const std::string &target, const std::string &link);
    static OperationResult moveToTrash(const std::string &path);
};
