#include "../algorithms/backtracking_knapsack.h"
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

    // 人工实例：最优解为 item1 + item2，价值 19、重量 10。
    // 该用例还要求回溯统计搜索节点和剪枝次数，避免退化成没有实验指标的穷举。
    KnapsackInstance manual;
    manual.source = "TEST";
    manual.group = "manual";
    manual.name = "backtracking_manual";
    manual.capacity = 10;
    manual.optimum = 19;
    manual.items = {
        Item{1, 10, 5},
        Item{2, 9, 5},
        Item{3, 8, 6},
        Item{4, 7, 6},
        Item{5, 1, 9},
        Item{6, 1, 9}
    };
    SolveResult manual_result = solveBacktracking(manual);
    assert(manual_result.algorithm == "Backtracking");
    assert(manual_result.status == STATUS_OK);
    assert(manual_result.value == 19);
    assert(manual_result.total_weight == 10);
    assert(manual_result.selected_count == 2);
    assert(manual_result.runtime_ms_avg >= 0.0);
    assert(manual_result.memory_kb > 0);
    assert(manual_result.repeat_times == 1);
    assert(manual_result.nodes > 0);
    assert(manual_result.pruned > 0);
    assert(manual_result.prune_rate > 0.0);
    assert(manual_result.prune_rate <= 1.0);

    // 边界实例：容量为 0 时，0 重量正价值物品仍然可以被选入。
    // 这能防止上界函数在 remaining == 0 时过早剪枝。
    KnapsackInstance zero_weight;
    zero_weight.source = "TEST";
    zero_weight.group = "manual";
    zero_weight.name = "zero_weight_item";
    zero_weight.capacity = 0;
    zero_weight.optimum = 5;
    zero_weight.items = {
        Item{1, 5, 0},
        Item{2, 10, 1}
    };
    SolveResult zero_weight_result = solveBacktracking(zero_weight);
    assert(zero_weight_result.status == STATUS_OK);
    assert(zero_weight_result.value == 5);
    assert(zero_weight_result.total_weight == 0);
    assert(zero_weight_result.selected_count == 1);

    // FSU p01：回溯法是精确算法，小规模实例应得到参考最优值。
    DatasetConfig fsu_config = findConfig(configs, "FSU", "p01");
    KnapsackInstance fsu = loadInstance(fsu_config, project_root);
    SolveResult fsu_result = solveBacktracking(fsu);
    assert(fsu_result.status == STATUS_OK);
    assert(fsu_result.value == 309);
    assert(fsu_result.value == fsu.optimum);
    assert(fsu_result.total_weight <= fsu.capacity);
    assert(fsu_result.nodes > 0);

    // UU f1：低维标准实例，回溯应与已知最优值一致。
    DatasetConfig uu_config = findConfig(configs, "UU", "f1_l-d_kp_10_269");
    KnapsackInstance uu = loadInstance(uu_config, project_root);
    SolveResult uu_result = solveBacktracking(uu);
    assert(uu_result.status == STATUS_OK);
    assert(uu_result.value == 295);
    assert(uu_result.value == uu.optimum);
    assert(uu_result.total_weight <= uu.capacity);
    assert(uu_result.nodes > 0);

    return 0;
}
