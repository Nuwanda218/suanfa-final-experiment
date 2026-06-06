# 0/1 背包结课实验 Makefile
#
# 当前 Makefile 先支持两个目标：
# 1. all：编译主程序骨架，输出到 ../experiments/knapsack_experiment
# 2. test：编译并运行核心类型与工具函数测试
#
# 后续添加 reader、algorithm、runner 的 .cpp 文件后，只需要扩展 SOURCES 即可。

CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra

TARGET := ../experiments/knapsack_experiment
TEST_TARGET := ../experiments/core_utils_test

SOURCES := main.cpp
TEST_SOURCES := tests/core_utils_test.cpp

.PHONY: all test clean run

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

test: $(TEST_TARGET)
	$(TEST_TARGET)

$(TEST_TARGET): $(TEST_SOURCES) core/knapsack_types.h core/experiment_types.h core/constants.h utils/string_utils.h utils/csv_utils.h utils/timer.h
	$(CXX) $(CXXFLAGS) $(TEST_SOURCES) -o $(TEST_TARGET)

run: all
	$(TARGET)

clean:
	rm -f $(TARGET) $(TEST_TARGET)
