#ifndef GREEDY_KNAPSACK_H
#define GREEDY_KNAPSACK_H

/**
 * @file greedy_knapsack.h
 * @brief 声明 0/1 背包问题的价值密度贪心算法。
 *
 * 贪心算法是本实验中的快速近似算法。它按照 value / weight 的价值密度从高到低排序，
 * 然后依次尝试装入物品。该方法时间复杂度低，适合大规模实例，但对于 0/1 背包问题
 * 不能保证得到全局最优解，因此后续需要与 DP 或已知 optimum 比较误差。
 */

#include "../core/experiment_types.h"
#include "../core/knapsack_types.h"

/**
 * @brief 使用价值密度贪心策略求解一个 0/1 背包实例。
 *
 * 时间复杂度：O(n log n)，主要来自排序。
 * 空间复杂度：O(n)，需要复制并排序物品数组，避免改变原始实例顺序。
 *
 * @param instance 统一背包实例。
 * @return SolveResult，包含价值、总重量、选中数量、运行时间和状态等字段。
 */
SolveResult solveGreedy(const KnapsackInstance& instance);

#endif // GREEDY_KNAPSACK_H
