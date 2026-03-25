#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <functional>
#include <filesystem>
#include <atomic>

namespace fs = std::filesystem;

class AnalysisEngine {
public:
    struct DirNode {
        std::string name;
        std::uint64_t size = 0;
        bool isDir = false;
        std::uint64_t fileCount = 0;
        std::vector<DirNode> children;
    };

    struct Stats {
        std::atomic<std::uint64_t> totalSize{0};
        std::atomic<std::uint64_t> totalFiles{0};
        std::atomic<std::uint64_t> totalDirs{0};
    };

    // Fast parallel analysis
    static DirNode analyseDirectoryFast(
        const std::string &path,
        const std::function<bool()>& shouldCancel,
        const std::function<void(std::uint64_t currentSize, std::uint64_t files, std::uint64_t dirs, const std::string& currentItem)>& onProgress
    );

    static std::uint64_t calculateFolderSize(
        const std::string &path, 
        const std::function<bool()>& shouldCancel
    );

    // Tree string generation for clipboard
    static std::string generateTreeString(const DirNode& node, const std::string& indent = "", bool isLast = true);
};
