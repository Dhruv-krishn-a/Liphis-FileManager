# CMake generated Testfile for 
# Source directory: /home/dhruv/Liphis
# Build directory: /home/dhruv/Liphis/build-release
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[fs_test_smoke]=] "/home/dhruv/Liphis/build-release/fs_test" "/home/dhruv/Liphis")
set_tests_properties([=[fs_test_smoke]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/dhruv/Liphis/CMakeLists.txt;211;add_test;/home/dhruv/Liphis/CMakeLists.txt;0;")
add_test([=[unit_tests]=] "/home/dhruv/Liphis/build-release/unit_tests")
set_tests_properties([=[unit_tests]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/dhruv/Liphis/CMakeLists.txt;212;add_test;/home/dhruv/Liphis/CMakeLists.txt;0;")
