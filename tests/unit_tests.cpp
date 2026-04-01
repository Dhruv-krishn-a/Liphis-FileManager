#include <iostream>
#include <cassert>
#include <filesystem>
#include <fstream>
#include "AnalysisEngine.hpp"
#include "ThreadPool.hpp"

namespace fs = std::filesystem;

void testAnalysis() {
    std::cout << "Testing AnalysisEngine..." << std::endl;
    
    fs::path testDir = "test_analysis_dir";
    fs::create_directories(testDir / "subdir1");
    fs::create_directories(testDir / "subdir2");
    
    std::ofstream(testDir / "file1.txt") << "Hello"; // 5 bytes
    std::ofstream(testDir / "subdir1" / "file2.txt") << "World!"; // 6 bytes
    
    AnalysisEngine engine;
    auto root = engine.analyseDirectoryFast(testDir.string(), [](){return false;}, nullptr);
    
    assert(root.name == "test_analysis_dir");
    assert(root.size == 11);
    assert(root.fileCount == 2);
    assert(root.children.size() == 3); // subdir1, subdir2, file1.txt
    
    std::cout << "AnalysisEngine tests passed!" << std::endl;
    
    fs::remove_all(testDir);
}

int main() {
    try {
        testAnalysis();
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
