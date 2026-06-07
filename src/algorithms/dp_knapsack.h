#ifndef DP_KNAPSACK_H
#define DP_KNAPSACK_H

/**
 * @file dp_knapsack.h
 * @brief 声明 0/1 背包问题的一维滚动数组动态规划算法。
 *
 * 动态规划是本实验中的精确求解算法。它利用“前 i 个物品、容量不超过 c 时的最大价值”
 * 这一状态转移思想获得最优解，并通过容量倒序遍历把二维 DP 压缩为一维数组。
 */

#include "../core/experiment_types.h"
#include "../core/knapsack_types.h"

/**
 * @brief 使用一维滚动数组 DP 求解一个 0/1 背包实例。
 *
 * 时间复杂度：O(nC)，其中 n 为物品数量，C 为背包容量。
 * 空间复杂度：O(C)，使用滚动数组保存每个容量下的当前最优价值。
 *
 * @param instance 统一背包实例。
 * @return SolveResult，包含最优价值、容量阈值状态、运行时间和内存估算等字段。
 */
SolveResult solveDp(const KnapsackInstance& instance);

#endif // DP_KNAPSACK_H
