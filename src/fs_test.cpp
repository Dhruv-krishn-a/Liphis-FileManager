#include <iostream>
#include <string>
#include "FileSystemEngine.hpp"
#include "FileMeta.hpp"

int main(int argc, char** argv) {
    std::string path = (argc > 1) ? argv[1] : ".";

    auto entries = FileSystemEngine::listDirectorySync(path, 0);

    std::cout << "Found " << entries.size() << " entries in: " << path << "\n\n";
    std::size_t shown = 0;
    for (const auto &e : entries) {
        std::cout << (e.isDir ? "D " : "F ") << e.name << " | " << e.path
                  << " | size=" << e.size << " | mtime=" << e.mtime << "\n";
        if (++shown >= 50) break;
    }

    return 0;
}
