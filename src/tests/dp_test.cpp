#include "../algorithms/dp_knapsack.h"
#include "../core/constants.h"
#include "../readers/data_reader.h"

#include <cassert>
#include <string>
#include <vector>

namespace {
DatasetConfig findConfig(const std::vector<DatasetConfig>& configs, const std::string& source, const std::string& name) {
    for (const DatasetConfig& config : configs) {
        if (config.source == source && config.name == name) {
            return config;
        }
    }
    assert(false && "required dataset config not found");
    return DatasetConfig{};
}
}

int main() {
    const std::string project_root = "..";
    const std::vector<DatasetConfig> configs = readDatasetConfigs(project_root + "/data/datasets.csv");

    // 人工小实例：验证 0/1 DP 能选出全局最优，而不是简单按单个高价值物品选择。
    KnapsackInstance manual;
    manual.source = "TEST";
    manual.group = "manual";
    manual.name = "dp_manual";
    manual.capacity = 10;
    manual.optimum = 14;
    manual.items = {
        Item{1, 6, 6},
        Item{2, 5, 4},
        Item{3, 10, 7},
        Item{4, 4, 3}
    };
    SolveResult manual_result = solveDp(manual);
    assert(manual_result.algorithm == "DP");
    assert(manual_result.status == STATUS_OK);
    assert(manual_result.value == 14);
    assert(manual_result.total_weight <= manual.capacity);
    assert(manual_result.runtime_ms_avg >= 0.0);
    assert(manual_result.memory_kb > 0);
    assert(manual_result.repeat_times == 1);
    assert(manual_result.nodes == 0);
    assert(manual_result.pruned == 0);

    // FSU p01：DP 是精确算法，应得到参考解最优值 309。
    DatasetConfig fsu_config = findConfig(configs, "FSU", "p01");
    KnapsackInstance fsu = loadInstance(fsu_config, project_root);
    SolveResult fsu_result = solveDp(fsu);
    assert(fsu_result.status == STATUS_OK);
    assert(fsu_result.value == 309);
    assert(fsu_result.value == fsu.optimum);
    assert(fsu_result.total_weight <= fsu.capacity);

    // UU f1：标准整数实例，已知最优值 295。
    DatasetConfig uu_config = findConfig(configs, "UU", "f1_l-d_kp_10_269");
    KnapsackInstance uu = loadInstance(uu_config, project_root);
    SolveResult uu_result = solveDp(uu);
    assert(uu_result.status == STATUS_OK);
    assert(uu_result.value == 295);
    assert(uu_result.value == uu.optimum);
    assert(uu_result.total_weight <= uu.capacity);

    // UU f8：容量较大的低维实例，验证一维滚动数组在 C=10000 时仍能得到最优值 9767。
    DatasetConfig f8_config = findConfig(configs, "UU", "f8_l-d_kp_23_10000");
    KnapsackInstance f8 = loadInstance(f8_config, project_root);
    SolveResult f8_result = solveDp(f8);
    assert(f8_result.status == STATUS_OK);
    assert(f8_result.value == 9767);
    assert(f8_result.value == f8.optimum);
    assert(f8_result.total_weight <= f8.capacity);

    // 容量阈值：DP 的复杂度与 capacity 直接相关，超过阈值时应主动跳过而不是盲目分配数组。
    DatasetConfig jj_large_config = findConfig(configs, "JJ", "n_400_c_100000000_g_10_f_0.1_eps_0.0001_s_100");
    KnapsackInstance jj_large = loadInstance(jj_large_config, project_root);
    SolveResult skipped = solveDp(jj_large);
    assert(skipped.algorithm == "DP");
    assert(skipped.status == STATUS_SKIPPED_BY_CAPACITY);
    assert(skipped.value == 0);
    assert(skipped.total_weight == 0);
    assert(skipped.selected_count == 0);

    return 0;
}
