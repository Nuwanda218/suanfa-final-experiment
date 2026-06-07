#include "dp_knapsack.h"

#include "../core/constants.h"
#include "../utils/timer.h"

#include <cstddef>
#include <vector>

SolveResult solveDp(const KnapsackInstance& instance) {
    SolveResult result;
    result.algorithm = "DP";
    result.repeat_times = 1;
    result.nodes = 0;
    result.pruned = 0;
    result.prune_rate = 0.0;

    if (instance.capacity < 0 || instance.capacity > DP_CAPACITY_LIMIT) {
        result.status = STATUS_SKIPPED_BY_CAPACITY;
        return result;
    }

    Timer timer;

    const std::size_t capacity = static_cast<std::size_t>(instance.capacity);

    // dp[c] 表示“在容量不超过 c 的限制下，已经扫描过的物品能得到的最大价值”。
    // selected_counts[c] 与 dp[c] 同步更新，用于记录该最优值对应的选中物品数量。
    std::vector<long long> dp(capacity + 1, 0);
    std::vector<int> selected_counts(capacity + 1, 0);

    for (const Item& item : instance.items) {
        if (item.weight < 0 || item.weight > instance.capacity) {
            continue;
        }

        const std::size_t weight = static_cast<std::size_t>(item.weight);

        // 容量必须倒序遍历。这样 dp[c - weight] 仍然来自“未选择当前物品”的上一层状态，
        // 保证每个物品最多使用一次，符合 0/1 背包而不是完全背包。
        for (std::size_t c = capacity + 1; c-- > weight;) {
            const long long candidate_value = dp[c - weight] + item.value;
            const int candidate_count = selected_counts[c - weight] + 1;

            if (candidate_value > dp[c]) {
                dp[c] = candidate_value;
                selected_counts[c] = candidate_count;
            }
        }
    }

    const long long best_value = dp[capacity];
    std::size_t best_weight = 0;
    while (best_weight <= capacity && dp[best_weight] < best_value) {
        ++best_weight;
    }

    result.status = STATUS_OK;
    result.value = best_value;
    result.total_weight = static_cast<long long>(best_weight);
    result.selected_count = selected_counts[best_weight];
    result.runtime_ms_avg = timer.elapsedMs();

    // DP 额外内存主要来自 value 数组和选中数量数组。这里按实际辅助数组大小估算 KB。
    const std::size_t bytes = dp.size() * sizeof(long long) + selected_counts.size() * sizeof(int);
    result.memory_kb = static_cast<long long>((bytes + 1023) / 1024);

    return result;
}
