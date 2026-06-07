# 0/1 背包结课实验 Makefile
#
# 默认直接执行 make 或 mingw32-make 时，会先清理旧的可执行文件，再重新编译主程序。
# 如果系统安装的是 GNU make，可以执行：make -C 3024244427-3-1/src
# 当前 Windows/MinGW 环境中可用的是 mingw32-make，因此可执行：mingw32-make -C 3024244427-3-1/src
#
# 其他常用目标：
# 1. build：只编译主程序，不主动清理
# 2. test：编译并运行当前已有测试
# 3. run：重新编译后运行主程序
#
# 后续添加 runner 的 .cpp 文件后，只需要扩展 SOURCES 或对应测试源即可。

# 明确指定 mingw32-make 使用 Windows cmd.exe 执行命令。
# 这样无论用户从 PowerShell 还是 cmd 调用，都不会依赖 Git Bash 中的 rm/sh 等 Unix 工具。
SHELL := cmd.exe
.SHELLFLAGS := /C

CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra
# 静态链接 MinGW 的 C++/GCC 运行库，避免 PowerShell PATH 中 Git 自带 libstdc++-6.dll 抢先加载导致崩溃。
LDFLAGS := -static-libstdc++ -static-libgcc

TARGET := ../experiments/knapsack_experiment.exe
CORE_TEST_TARGET := ../experiments/core_utils_test.exe
READER_TEST_TARGET := ../experiments/reader_test.exe
GREEDY_TEST_TARGET := ../experiments/greedy_test.exe
DP_TEST_TARGET := ../experiments/dp_test.exe
BACKTRACKING_TEST_TARGET := ../experiments/backtracking_test.exe
RUNNER_TEST_TARGET := ../experiments/runner_test.exe

# Windows 下给 cmd 使用的路径。编译时 g++ 可以接受 /，但 cmd 的 del/运行命令用 \ 更稳定。
TARGET_WIN := ..\experiments\knapsack_experiment.exe
CORE_TEST_TARGET_WIN := ..\experiments\core_utils_test.exe
READER_TEST_TARGET_WIN := ..\experiments\reader_test.exe
GREEDY_TEST_TARGET_WIN := ..\experiments\greedy_test.exe
DP_TEST_TARGET_WIN := ..\experiments\dp_test.exe
BACKTRACKING_TEST_TARGET_WIN := ..\experiments\backtracking_test.exe
RUNNER_TEST_TARGET_WIN := ..\experiments\runner_test.exe

READER_SOURCES := readers/fsu_reader.cpp readers/uu_reader.cpp readers/jj_reader.cpp
GREEDY_SOURCES := algorithms/greedy_knapsack.cpp
DP_SOURCES := algorithms/dp_knapsack.cpp
BACKTRACKING_SOURCES := algorithms/backtracking_knapsack.cpp
RUNNER_SOURCES := runner/experiment_runner.cpp runner/result_writer.cpp
SOURCES := main.cpp $(RUNNER_SOURCES) $(GREEDY_SOURCES) $(DP_SOURCES) $(BACKTRACKING_SOURCES) $(READER_SOURCES)
CORE_TEST_SOURCES := tests/core_utils_test.cpp
READER_TEST_SOURCES := tests/reader_test.cpp $(READER_SOURCES)
GREEDY_TEST_SOURCES := tests/greedy_test.cpp $(GREEDY_SOURCES) $(READER_SOURCES)
DP_TEST_SOURCES := tests/dp_test.cpp $(DP_SOURCES) $(READER_SOURCES)
BACKTRACKING_TEST_SOURCES := tests/backtracking_test.cpp $(BACKTRACKING_SOURCES) $(READER_SOURCES)
RUNNER_TEST_SOURCES := tests/runner_test.cpp $(RUNNER_SOURCES) $(GREEDY_SOURCES) $(DP_SOURCES) $(BACKTRACKING_SOURCES) $(READER_SOURCES)

.PHONY: all build test clean run core-test reader-test greedy-test dp-test backtracking-test runner-test

# 默认目标：先 clean 再 build，避免旧可执行文件影响验证。
all: clean build

# 只编译主程序，不主动清理；供 test/run 或后续脚本按需复用。
build: $(TARGET)

$(TARGET): $(SOURCES) runner/experiment_runner.h runner/result_writer.h readers/data_reader.h core/knapsack_types.h core/experiment_types.h core/constants.h utils/timer.h utils/csv_utils.h
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET) $(LDFLAGS)

# test 会运行当前已有的全部测试。后续增加算法测试时继续挂到这里。
test: core-test reader-test greedy-test dp-test backtracking-test runner-test

core-test: $(CORE_TEST_TARGET)
	$(CORE_TEST_TARGET_WIN)

reader-test: $(READER_TEST_TARGET)
	$(READER_TEST_TARGET_WIN)

greedy-test: $(GREEDY_TEST_TARGET)
	$(GREEDY_TEST_TARGET_WIN)

dp-test: $(DP_TEST_TARGET)
	$(DP_TEST_TARGET_WIN)

backtracking-test: $(BACKTRACKING_TEST_TARGET)
	$(BACKTRACKING_TEST_TARGET_WIN)

runner-test: $(RUNNER_TEST_TARGET)
	$(RUNNER_TEST_TARGET_WIN)

$(CORE_TEST_TARGET): $(CORE_TEST_SOURCES) core/knapsack_types.h core/experiment_types.h core/constants.h utils/string_utils.h utils/csv_utils.h utils/timer.h
	$(CXX) $(CXXFLAGS) $(CORE_TEST_SOURCES) -o $(CORE_TEST_TARGET) $(LDFLAGS)

$(READER_TEST_TARGET): $(READER_TEST_SOURCES) readers/data_reader.h core/knapsack_types.h core/experiment_types.h utils/string_utils.h utils/csv_utils.h
	$(CXX) $(CXXFLAGS) $(READER_TEST_SOURCES) -o $(READER_TEST_TARGET) $(LDFLAGS)

$(GREEDY_TEST_TARGET): $(GREEDY_TEST_SOURCES) algorithms/greedy_knapsack.h readers/data_reader.h core/knapsack_types.h core/experiment_types.h core/constants.h utils/timer.h
	$(CXX) $(CXXFLAGS) $(GREEDY_TEST_SOURCES) -o $(GREEDY_TEST_TARGET) $(LDFLAGS)

$(DP_TEST_TARGET): $(DP_TEST_SOURCES) algorithms/dp_knapsack.h readers/data_reader.h core/knapsack_types.h core/experiment_types.h core/constants.h utils/timer.h
	$(CXX) $(CXXFLAGS) $(DP_TEST_SOURCES) -o $(DP_TEST_TARGET) $(LDFLAGS)

$(BACKTRACKING_TEST_TARGET): $(BACKTRACKING_TEST_SOURCES) algorithms/backtracking_knapsack.h readers/data_reader.h core/knapsack_types.h core/experiment_types.h core/constants.h utils/timer.h
	$(CXX) $(CXXFLAGS) $(BACKTRACKING_TEST_SOURCES) -o $(BACKTRACKING_TEST_TARGET) $(LDFLAGS)

$(RUNNER_TEST_TARGET): $(RUNNER_TEST_SOURCES) runner/experiment_runner.h runner/result_writer.h readers/data_reader.h core/knapsack_types.h core/experiment_types.h core/constants.h utils/timer.h
	$(CXX) $(CXXFLAGS) $(RUNNER_TEST_SOURCES) -o $(RUNNER_TEST_TARGET) $(LDFLAGS)

run: all
	$(TARGET_WIN)

clean:
	if exist $(TARGET_WIN) del /F /Q $(TARGET_WIN)
	if exist $(CORE_TEST_TARGET_WIN) del /F /Q $(CORE_TEST_TARGET_WIN)
	if exist $(READER_TEST_TARGET_WIN) del /F /Q $(READER_TEST_TARGET_WIN)
	if exist $(GREEDY_TEST_TARGET_WIN) del /F /Q $(GREEDY_TEST_TARGET_WIN)
	if exist $(DP_TEST_TARGET_WIN) del /F /Q $(DP_TEST_TARGET_WIN)
	if exist $(BACKTRACKING_TEST_TARGET_WIN) del /F /Q $(BACKTRACKING_TEST_TARGET_WIN)
	if exist $(RUNNER_TEST_TARGET_WIN) del /F /Q $(RUNNER_TEST_TARGET_WIN)
