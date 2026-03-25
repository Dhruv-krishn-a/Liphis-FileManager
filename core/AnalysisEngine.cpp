#include "AnalysisEngine.hpp"
#include "ThreadPool.hpp"
#include <algorithm>
#include <system_error>
#include <future>
#include <mutex>

namespace {
    struct InternalStats {
        std::uint64_t size = 0;
        std::uint64_t fileCount = 0;
    };

    // Helper for recursive size and file count calculation
    InternalStats recursiveAnalyse(
        const fs::path& path,
        const std::function<bool()>& shouldCancel,
        const std::function<void(std::uint64_t size, std::uint64_t files, std::uint64_t dirs, const std::string&)>& onProgress,
        std::uint64_t& currentTotalSize,
        std::uint64_t& currentTotalFiles,
        std::uint64_t& currentTotalDirs
    ) {
        InternalStats stats;
        std::error_code ec;

        if (shouldCancel()) return stats;

        for (auto const& entry : fs::directory_iterator(path, fs::directory_options::skip_permission_denied, ec)) {
            if (shouldCancel()) break;

            std::error_code st_ec;
            auto st = fs::symlink_status(entry.path(), st_ec); // Use symlink_status to detect links
            if (st_ec) continue;

            if (fs::is_directory(st) && !fs::is_symlink(st)) {
                currentTotalDirs++;
                auto sub = recursiveAnalyse(entry.path(), shouldCancel, onProgress, currentTotalSize, currentTotalFiles, currentTotalDirs);
                stats.size += sub.size;
                stats.fileCount += sub.fileCount;
            } else if (fs::is_regular_file(st)) {
                std::error_code sz_ec;
                auto sz = fs::file_size(entry.path(), sz_ec);
                if (!sz_ec) {
                    stats.size += sz;
                    stats.fileCount++;
                    currentTotalSize += sz;
                    currentTotalFiles++;
                }
            }
            
            // Periodically report progress
            static thread_local int counter = 0;
            if (++counter % 200 == 0) {
                if (onProgress) onProgress(currentTotalSize, currentTotalFiles, currentTotalDirs, entry.path().filename().string());
            }
        }
        return stats;
    }
}

AnalysisEngine::DirNode AnalysisEngine::analyseDirectoryFast(
    const std::string &path,
    const std::function<bool()>& shouldCancel,
    const std::function<void(std::uint64_t currentSize, std::uint64_t files, std::uint64_t dirs, const std::string& currentItem)>& onProgress
) {
    DirNode root;
    fs::path rootPath(path);
    root.name = rootPath.filename().string();
    if (root.name.empty()) root.name = path;
    root.isDir = true;

    // Use atomic counters for shared progress updates across threads
    struct SharedStats {
        std::atomic<std::uint64_t> totalSize{0};
        std::atomic<std::uint64_t> totalFiles{0};
        std::atomic<std::uint64_t> totalDirs{0};
    };
    auto shared = std::make_shared<SharedStats>();

    std::error_code ec;
    std::vector<std::future<std::pair<DirNode, InternalStats>>> futures;

    for (auto const& entry : fs::directory_iterator(rootPath, fs::directory_options::skip_permission_denied, ec)) {
        if (shouldCancel()) break;

        // CRITICAL FIX: Capture by value to avoid dangling references
        futures.push_back(ThreadPool::instance().submit([shared, entryPath = entry.path(), shouldCancel, onProgress]() {
            DirNode node;
            node.name = entryPath.filename().string();
            std::error_code st_ec;
            auto st = fs::symlink_status(entryPath, st_ec);
            node.isDir = fs::is_directory(st) && !fs::is_symlink(st);

            InternalStats s;
            if (node.isDir) {
                // Wrapper to bridge atomic to reference if needed, or just pass atomics directly
                auto wrapperProgress = [&](std::uint64_t s, std::uint64_t f, std::uint64_t d, const std::string& item) {
                    if (onProgress) onProgress(shared->totalSize.load(), shared->totalFiles.load(), shared->totalDirs.load(), item);
                };

                // Create local counters to pass to recursive function to avoid excessive atomic contention
                std::uint64_t localSize = 0;
                std::uint64_t localFiles = 0;
                std::uint64_t localDirs = 0;

                auto innerProgress = [&](std::uint64_t s, std::uint64_t f, std::uint64_t d, const std::string& item) {
                    shared->totalSize += (s - localSize);
                    shared->totalFiles += (f - localFiles);
                    shared->totalDirs += (d - localDirs);
                    localSize = s; localFiles = f; localDirs = d;
                    if (onProgress) onProgress(shared->totalSize.load(), shared->totalFiles.load(), shared->totalDirs.load(), item);
                };

                s = recursiveAnalyse(entryPath, shouldCancel, innerProgress, localSize, localFiles, localDirs);
                node.size = s.size;
                node.fileCount = s.fileCount;
            } else {
                std::error_code sz_ec;
                node.size = fs::file_size(entryPath, sz_ec);
                node.fileCount = 1;
                shared->totalSize += node.size;
                shared->totalFiles++;
                if (onProgress) onProgress(shared->totalSize.load(), shared->totalFiles.load(), shared->totalDirs.load(), node.name);
            }
            return std::make_pair(node, s);
        }));
    }

    for (auto& f : futures) {
        auto res = f.get();
        root.size += res.first.size;
        root.fileCount += res.first.fileCount;
        root.children.push_back(std::move(res.first));
    }

    std::sort(root.children.begin(), root.children.end(), [](const DirNode& a, const DirNode& b) {
        return a.size > b.size;
    });

    return root;
}

std::uint64_t AnalysisEngine::calculateFolderSize(
    const std::string &path, 
    const std::function<bool()>& shouldCancel
) {
    std::uint64_t totalSize = 0;
    std::error_code ec;
    for (auto const& entry : fs::recursive_directory_iterator(path, fs::directory_options::skip_permission_denied, ec)) {
        if (shouldCancel()) break;
        if (ec) { ec.clear(); continue; }
        if (fs::is_regular_file(entry.status(ec))) {
            std::error_code sz_ec;
            totalSize += fs::file_size(entry.path(), sz_ec);
        }
    }
    return totalSize;
}

std::string AnalysisEngine::generateTreeString(const DirNode& node, const std::string& indent, bool isLast) {
    std::string result = indent + (isLast ? "└── " : "├── ") + node.name;
    
    // Add size to the tree output
    double size = static_cast<double>(node.size);
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unit = 0;
    while (size >= 1024 && unit < 4) { size /= 1024; unit++; }
    char sizeBuf[32];
    snprintf(sizeBuf, sizeof(sizeBuf), " (%.1f %s)", size, units[unit]);
    result += sizeBuf + std::string("\n");

    std::string newIndent = indent + (isLast ? "    " : "│   ");
    for (size_t i = 0; i < node.children.size(); ++i) {
        // Limit tree depth for clipboard to avoid massive strings
        if (indent.length() > 40) {
            result += newIndent + "...\n";
            break;
        }
        result += generateTreeString(node.children[i], newIndent, i == node.children.size() - 1);
    }
    return result;
}
