#ifndef BACKTRACKING_KNAPSACK_H
#define BACKTRACKING_KNAPSACK_H

/**
 * @file backtracking_knapsack.h
 * @brief 声明 0/1 背包问题的带上界剪枝回溯算法。
 *
 * 回溯法是本实验中的精确搜索算法。它枚举每个物品“选”或“不选”两种分支，
 * 并使用分数背包上界提前剪掉不可能超过当前最优值的搜索子树。
 */

#include "../core/experiment_types.h"
#include "../core/knapsack_types.h"

/**
 * @brief 使用带价值密度上界剪枝的 DFS 回溯法求解一个 0/1 背包实例。
 *
 * 最坏时间复杂度：O(2^n)，剪枝效果取决于数据分布和当前最好解质量。
 * 空间复杂度：O(n)，主要来自排序后的物品数组和递归深度。
 *
 * @param instance 统一背包实例。
 * @return SolveResult，包含最好价值、搜索节点数、剪枝次数、超时状态等字段。
 */
SolveResult solveBacktracking(const KnapsackInstance& instance);

#endif // BACKTRACKING_KNAPSACK_H
