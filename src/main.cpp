/**
 * @file main.cpp
 * @brief 0/1 背包三算法对比实验程序入口。
 *
 * 当前 Task 3 只建立可编译的项目骨架：核心类型、工具函数、Makefile 和一个简单入口。
 * 后续 Task 会继续补充数据读取、三种算法、批量实验运行器和 CSV 结果输出。
 */

#include "core/constants.h"
#include "core/experiment_types.h"
#include "core/knapsack_types.h"
#include "utils/timer.h"

#include <iostream>

int main() {
    Timer timer;

    // 这里先构造一个最小实例，用于验证核心结构可以被主程序正常包含和使用。
    // 真正的数据读取会在后续 reader 模块中实现。
    KnapsackInstance demo;
    demo.source = "DEMO";
    demo.group = "skeleton";
    demo.name = "empty_instance";
    demo.capacity = 0;
    demo.optimum = -1;

    std::cout << "0/1 Knapsack Experiment Skeleton" << '\n';
    std::cout << "status=" << STATUS_OK << '\n';
    std::cout << "dp_capacity_limit=" << DP_CAPACITY_LIMIT << '\n';
    std::cout << "demo_items=" << demo.n() << '\n';
    std::cout << "startup_ms=" << timer.elapsedMs() << '\n';

    return 0;
}
