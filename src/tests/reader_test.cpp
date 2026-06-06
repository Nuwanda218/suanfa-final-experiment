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

    // datasets.csv 是后续批量实验的入口。这里先验证配置读取数量和典型字段。
    std::vector<DatasetConfig> configs = readDatasetConfigs(project_root + "/data/datasets.csv");
    assert(configs.size() == 25);
    assert(configs[0].source == "FSU");
    assert(configs[0].name == "p01");
    assert(configs[0].enabled);

    // FSU p01：四文件格式，最优值应由 p01_s.txt 参考解计算得到 309。
    DatasetConfig fsu_config = findConfig(configs, "FSU", "p01");
    KnapsackInstance fsu = loadInstance(fsu_config, project_root);
    assert(fsu.source == "FSU");
    assert(fsu.group == "fsu_correctness");
    assert(fsu.name == "p01");
    assert(fsu.capacity == 165);
    assert(fsu.n() == 10);
    assert(fsu.optimum == 309);
    assert(fsu.items[0].id == 1);
    assert(fsu.items[0].value == 92);
    assert(fsu.items[0].weight == 23);

    // UU f1：标准 n capacity + value weight 格式，整数数据不需要缩放。
    DatasetConfig uu_config = findConfig(configs, "UU", "f1_l-d_kp_10_269");
    KnapsackInstance uu = loadInstance(uu_config, project_root);
    assert(uu.source == "UU");
    assert(uu.group == "uu_low");
    assert(uu.name == "f1_l-d_kp_10_269");
    assert(uu.capacity == 269);
    assert(uu.n() == 10);
    assert(uu.optimum == 295);
    assert(uu.items[0].id == 1);
    assert(uu.items[0].value == 55);
    assert(uu.items[0].weight == 95);

    // UU f5：原始文件包含小数。reader 应统一放大 10000 倍，便于后续整数 DP 处理。
    DatasetConfig f5_config = findConfig(configs, "UU", "f5_l-d_kp_15_375");
    KnapsackInstance f5 = loadInstance(f5_config, project_root);
    assert(f5.capacity == 3750000);
    assert(f5.n() == 15);
    assert(f5.optimum == 4810694);
    assert(f5.items[0].value == 1251);
    assert(f5.items[0].weight == 563585);

    // JJ：困难实例格式为 n + 多行 id profit weight + capacity。
    DatasetConfig jj_config = findConfig(configs, "JJ", "n_400_c_1000000_g_10_f_0.1_eps_0.0001_s_100");
    KnapsackInstance jj = loadInstance(jj_config, project_root);
    assert(jj.source == "JJ");
    assert(jj.group == "jj_hard");
    assert(jj.capacity == 1000000);
    assert(jj.n() == 400);
    assert(jj.optimum == 1004190);
    assert(jj.items[0].id == 0);
    assert(jj.items[0].value == 500111);
    assert(jj.items[0].weight == 500179);

    return 0;
}
