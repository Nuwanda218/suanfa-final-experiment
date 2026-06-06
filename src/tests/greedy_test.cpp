#include "../algorithms/greedy_knapsack.h"
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

    // 人工构造一个小实例，验证贪心确实按价值密度从高到低选择。
    // item2 密度最高，会被最先选入；随后同密度物品按 id 升序尝试，能装入就继续选择。
    KnapsackInstance manual;
    manual.source = "TEST";
    manual.group = "manual";
    manual.name = "density_order";
    manual.capacity = 10;
    manual.optimum = -1;
    manual.items = {
        Item{1, 10, 10}, // 密度 1.0
        Item{2, 9, 5},   // 密度 1.8，应先选
        Item{3, 4, 4},   // 密度 1.0，剩余容量 5 时可选
        Item{4, 1, 1}    // 密度 1.0，也可选，用于检查同密度稳定排序规则
    };
    SolveResult manual_result = solveGreedy(manual);
    assert(manual_result.algorithm == "Greedy");
    assert(manual_result.status == STATUS_OK);
    assert(manual_result.value == 14);
    assert(manual_result.total_weight == 10);
    assert(manual_result.selected_count == 3);
    assert(manual_result.runtime_ms_avg >= 0.0);
    assert(manual_result.memory_kb >= 0);
    assert(manual_result.repeat_times == 1);
    assert(manual_result.nodes == 0);
    assert(manual_result.pruned == 0);

    // FSU p01：贪心结果必须不超容量。该实例中密度贪心刚好能达到参考最优值 309。
    DatasetConfig fsu_config = findConfig(configs, "FSU", "p01");
    KnapsackInstance fsu = loadInstance(fsu_config, project_root);
    SolveResult fsu_result = solveGreedy(fsu);
    assert(fsu_result.status == STATUS_OK);
    assert(fsu_result.value == 309);
    assert(fsu_result.total_weight == 165);
    assert(fsu_result.selected_count == 5);
    assert(fsu_result.total_weight <= fsu.capacity);

    // UU f1：贪心可以快速给出可行解，但不一定达到最优值。这里期望值为按密度排序后的确定结果。
    DatasetConfig uu_config = findConfig(configs, "UU", "f1_l-d_kp_10_269");
    KnapsackInstance uu = loadInstance(uu_config, project_root);
    SolveResult uu_result = solveGreedy(uu);
    assert(uu_result.status == STATUS_OK);
    assert(uu_result.value == 294);
    assert(uu_result.total_weight == 260);
    assert(uu_result.selected_count == 6);
    assert(uu_result.total_weight <= uu.capacity);
    assert(uu_result.value < uu.optimum);

    return 0;
}
