#include "backtracking_knapsack.h"

#include "../core/constants.h"
#include "../utils/timer.h"

#include <algorithm>
#include <cstddef>
#include <functional>
#include <vector>

namespace {
struct SearchItem {
    Item item;
};

bool densityGreater(const SearchItem& a, const SearchItem& b) {
    if (a.item.weight == 0 || b.item.weight == 0) {
        if (a.item.weight == b.item.weight) {
            if (a.item.value == b.item.value) {
                return a.item.id < b.item.id;
            }
            return a.item.value > b.item.value;
        }
        return a.item.weight == 0 && a.item.value > 0;
    }

    const long double left = static_cast<long double>(a.item.value) * static_cast<long double>(b.item.weight);
    const long double right = static_cast<long double>(b.item.value) * static_cast<long double>(a.item.weight);
    if (left == right) {
        return a.item.id < b.item.id;
    }
    return left > right;
}
}

SolveResult solveBacktracking(const KnapsackInstance& instance) {
    Timer timer;

    SolveResult result;
    result.algorithm = "Backtracking";
    result.status = STATUS_OK;
    result.repeat_times = 1;

    std::vector<SearchItem> items;
    items.reserve(instance.items.size());
    for (const Item& item : instance.items) {
        if (item.weight >= 0) {
            items.push_back(SearchItem{item});
        }
    }
    std::sort(items.begin(), items.end(), densityGreater);

    long long best_value = 0;
    long long best_weight = 0;
    int best_selected_count = 0;
    long long nodes = 0;
    long long pruned = 0;
    bool timed_out = false;

    /**
     * 分数背包上界：假设剩余容量可以继续装入“物品的一部分”，得到一个理想化乐观估计。
     * 如果这个上界仍不超过当前 best_value，则真实 0/1 选择更不可能超过 best_value。
     */
    auto bound = [&](std::size_t index, long long current_weight, long long current_value) {
        long double upper = static_cast<long double>(current_value);
        long long remaining = instance.capacity - current_weight;

        for (std::size_t i = index; i < items.size(); ++i) {
            const Item& item = items[i].item;
            if (item.weight == 0) {
                if (item.value > 0) {
                    upper += static_cast<long double>(item.value);
                }
                continue;
            }

            if (remaining <= 0) {
                break;
            }

            if (item.weight <= remaining) {
                remaining -= item.weight;
                upper += static_cast<long double>(item.value);
            } else {
                upper += static_cast<long double>(item.value) *
                         (static_cast<long double>(remaining) / static_cast<long double>(item.weight));
                break;
            }
        }

        return upper;
    };

    std::function<void(std::size_t, long long, long long, int)> dfs =
        [&](std::size_t index, long long current_weight, long long current_value, int selected_count) {
            if (timer.elapsedMs() > BACKTRACKING_TIME_LIMIT_MS) {
                timed_out = true;
                return;
            }

            ++nodes;

            if (current_value > best_value) {
                best_value = current_value;
                best_weight = current_weight;
                best_selected_count = selected_count;
            }

            if (index >= items.size()) {
                return;
            }

            if (bound(index, current_weight, current_value) <= static_cast<long double>(best_value)) {
                ++pruned;
                return;
            }

            const Item& item = items[index].item;

            // “选当前物品”分支。优先走该分支可以更早得到较好的当前最优值，从而增强后续剪枝。
            if (current_weight + item.weight <= instance.capacity) {
                dfs(index + 1, current_weight + item.weight, current_value + item.value, selected_count + 1);
                if (timed_out) {
                    return;
                }
            }

            // “不选当前物品”分支。若上界已经不可能改善答案，会在下一层入口处被剪枝。
            dfs(index + 1, current_weight, current_value, selected_count);
        };

    dfs(0, 0, 0, 0);

    result.status = timed_out ? STATUS_TIMEOUT : STATUS_OK;
    result.value = best_value;
    result.total_weight = best_weight;
    result.selected_count = best_selected_count;
    result.runtime_ms_avg = timer.elapsedMs();
    result.nodes = nodes;
    result.pruned = pruned;
    result.prune_rate = nodes > 0 ? static_cast<double>(pruned) / static_cast<double>(nodes) : 0.0;

    const std::size_t bytes = items.size() * sizeof(SearchItem) + items.size() * sizeof(int);
    result.memory_kb = static_cast<long long>((bytes + 1023) / 1024);

    return result;
}
