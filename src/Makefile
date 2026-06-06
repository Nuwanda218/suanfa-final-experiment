# 0/1 背包结课实验 Makefile
#
# 默认直接执行 make 或 mingw32-make 时，会先清理旧的可执行文件，再重新编译主程序。
# 如果系统安装的是 GNU make，可以执行：make -C 3024244427-3-1/src
# 当前 Windows/MinGW 环境中可用的是 mingw32-make，因此可执行：mingw32-make -C 3024244427-3-1/src
#
# 其他常用目标：
# 1. build：只编译主程序，不主动清理
# 2. test：编译并运行核心类型与工具函数测试
# 3. run：重新编译后运行主程序
#
# 后续添加 reader、algorithm、runner 的 .cpp 文件后，只需要扩展 SOURCES 即可。

# 明确指定 mingw32-make 使用 Windows cmd.exe 执行命令。
# 这样无论用户从 PowerShell 还是 cmd 调用，都不会依赖 Git Bash 中的 rm/sh 等 Unix 工具。
SHELL := cmd.exe
.SHELLFLAGS := /C

CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra

TARGET := ../experiments/knapsack_experiment.exe
TEST_TARGET := ../experiments/core_utils_test.exe

# Windows 下给 cmd 使用的路径。编译时 g++ 可以接受 /，但 cmd 的 del/运行命令用 \ 更稳定。
TARGET_WIN := ..\experiments\knapsack_experiment.exe
TEST_TARGET_WIN := ..\experiments\core_utils_test.exe

SOURCES := main.cpp
TEST_SOURCES := tests/core_utils_test.cpp

.PHONY: all build test clean run

# 默认目标：先 clean 再 build，避免旧可执行文件影响验证。
all: clean build

# 只编译主程序，不主动清理；供 test/run 或后续脚本按需复用。
build: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

test: $(TEST_TARGET)
	$(TEST_TARGET_WIN)

$(TEST_TARGET): $(TEST_SOURCES) core/knapsack_types.h core/experiment_types.h core/constants.h utils/string_utils.h utils/csv_utils.h utils/timer.h
	$(CXX) $(CXXFLAGS) $(TEST_SOURCES) -o $(TEST_TARGET)

run: all
	$(TARGET_WIN)

clean:
	if exist $(TARGET_WIN) del /F /Q $(TARGET_WIN)
	if exist $(TEST_TARGET_WIN) del /F /Q $(TEST_TARGET_WIN)
