#include "greedy_knapsack.h"

#include "../core/constants.h"
#include "../utils/timer.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {
/**
 * @brief 计算物品价值密度时使用的比较函数。
 *
 * 为了避免 value / weight 的浮点误差，这里不用 double 直接相除，而是交叉相乘比较：
 * a.value / a.weight > b.value / b.weight 等价于 a.value * b.weight > b.value * a.weight。
 * 若两个物品密度相同，则按 id 从小到大排序，使结果可重复，方便测试和报告复现。
 */
bool densityGreater(const Item& a, const Item& b) {
    const long double left = static_cast<long double>(a.value) * static_cast<long double>(b.weight);
    const long double right = static_cast<long double>(b.value) * static_cast<long double>(a.weight);
    if (left == right) {
        return a.id < b.id;
    }
    return left > right;
}
}

SolveResult solveGreedy(const KnapsackInstance& instance) {
    Timer timer;

    SolveResult result;
    result.algorithm = "Greedy";
    result.status = STATUS_OK;
    result.repeat_times = 1;

    // 复制物品后排序，避免改变 KnapsackInstance 中的原始读取顺序。
    std::vector<Item> items = instance.items;
    std::sort(items.begin(), items.end(), densityGreater);

    long long current_value = 0;
    long long current_weight = 0;
    int selected_count = 0;

    // 价值密度贪心：按密度从高到低扫描，当前物品能放入就选择，否则跳过。
    // 这里的“跳过”不会回头替换已有物品，因此速度快，但也正因为如此不能保证全局最优。
    for (const Item& item : items) {
        if (item.weight < 0) {
            continue;
        }
        if (current_weight + item.weight <= instance.capacity) {
            current_weight += item.weight;
            current_value += item.value;
            ++selected_count;
        }
    }

    result.value = current_value;
    result.total_weight = current_weight;
    result.selected_count = selected_count;
    result.runtime_ms_avg = timer.elapsedMs();

    // 贪心算法主要额外保存排序后的物品数组，这里给出近似内存估算，便于结果表中横向比较。
    result.memory_kb = static_cast<long long>((items.size() * sizeof(Item) + 1023) / 1024);

    // nodes/pruned 是回溯法专属指标，贪心算法不产生搜索树，因此保持 0。
    result.nodes = 0;
    result.pruned = 0;
    result.prune_rate = 0.0;

    return result;
}
