#ifndef KNAPSACK_TYPES_H
#define KNAPSACK_TYPES_H

/**
 * @file knapsack_types.h
 * @brief 定义 0/1 背包问题本身的核心数据结构。
 *
 * 本实验同时使用 FSU、UU、JJ 三类数据源，它们的原始文件格式并不相同。
 * 为了让后续 DP、回溯法、贪心算法不关心“数据来自哪里”，reader 模块会把
 * 不同格式的数据统一转换为本文件中的 KnapsackInstance。这样三种算法只需要
 * 面向同一种输入结构，实验结果也更容易横向比较。
 */

#include <string>
#include <vector>

/**
 * @brief 背包问题中的一个物品。
 *
 * 0/1 背包中每个物品只能选择 0 次或 1 次，因此这里只保存物品的编号、价值和重量。
 * 价值和重量使用 long long，是为了兼容 JJ 数据集中较大的容量和价值。
 */
struct Item {
    /** 物品编号。FSU、UU 中可按读取顺序从 1 编号；JJ 中可保留原始 id。 */
    int id = 0;

    /** 物品价值，也就是目标函数中希望最大化的 profit/value。 */
    long long value = 0;

    /** 物品重量，也就是容量约束中消耗的 weight。 */
    long long weight = 0;
};

/**
 * @brief 统一后的 0/1 背包实例。
 *
 * 三类数据源读取后都转换为该结构。这样算法模块不需要分别处理 FSU 的四文件格式、
 * UU 的两列格式或 JJ 的 id-profit-weight 格式，只需要读取 capacity 和 items。
 */
struct KnapsackInstance {
    /** 数据源名称，例如 FSU、UU、JJ。用于实验结果分组。 */
    std::string source;

    /** 实验分组，例如 fsu_correctness、uu_low、jj_hard。 */
    std::string group;

    /** 数据集实例名称，例如 p01 或 knapPI_1_100_1000_1。 */
    std::string name;

    /** 背包容量 C。DP 的复杂度与该值直接相关，因此使用 long long 保存。 */
    long long capacity = 0;

    /** 物品列表。每个物品包含 id、value、weight 三个字段。 */
    std::vector<Item> items;

    /**
     * 已知最优值。若数据集没有可用最优值，则约定为 -1。
     * 本实验用它验证 DP、回溯法正确性，并计算贪心算法的相对误差。
     */
    long long optimum = -1;

    /**
     * @brief 返回物品数量。
     *
     * n 可以由 items.size() 得到，因此不额外存储，避免读取文件后出现 n 与数组长度不一致。
     */
    int n() const {
        return static_cast<int>(items.size());
    }
};

#endif // KNAPSACK_TYPES_H
