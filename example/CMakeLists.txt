# CMakeList.txt: example 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

project ("example0")
project ("example1")
project ("example2")
project ("example3")
project ("example4")
project ("example5")
project ("example6")

include_directories (../include)

# 将源代码添加到此项目的可执行文件。
add_executable (example0 "example0.cpp" "example.h")
add_executable (example1 "example1.cpp" "example.h")
add_executable (example2 "example2.cpp" "example.h")
add_executable (example3 "example3.cpp" "example.h")
add_executable (example4 "example4.cpp" "example.h")
add_executable (example5 "example5.cpp" "example.h")
add_executable (example6 "example6.cpp" "example.h")

# TODO: 如有需要，请添加测试并安装目标。
