#ifndef EXPERIMENT_TYPES_H
#define EXPERIMENT_TYPES_H

/**
 * @file experiment_types.h
 * @brief 定义实验配置和算法运行结果结构。
 *
 * knapsack_types.h 描述“问题是什么”，本文件描述“实验如何运行、结果如何记录”。
 * 这些结构会被 reader、runner、result_writer 等模块共同使用，最终对应到
 * data/datasets.csv 和 experiments/results.csv 中的字段。
 */

#include <string>

/**
 * @brief data/datasets.csv 中一行数据集配置。
 *
 * 使用配置文件而不是在代码里写死数据集列表，可以让实验数据选择更加清晰。
 * 后续如果增加或禁用某个实例，只需要修改 CSV，不需要改算法代码。
 */
struct DatasetConfig {
    /** 数据源：FSU / UU / JJ。runner 会据此选择不同 reader。 */
    std::string source;

    /** 实验分组：例如 fsu_correctness、uu_low、uu_large_uncorrelated、jj_hard。 */
    std::string group;

    /** 数据集名称：例如 p01、f1_l-d_kp_10_269。 */
    std::string name;

    /**
     * 数据路径，相对于 3024244427-3-1/ 项目根目录。
     * FSU 使用不带后缀的前缀路径，例如 data/FSU/p01；
     * UU 使用单个文件路径；JJ 使用实例目录路径。
     */
    std::string path;

    /** 已知最优值，未知时为 -1。用于正确性验证和误差计算。 */
    long long optimum = -1;

    /** 配置文件中记录的物品数量，用于读取后校验。 */
    int n = 0;

    /** 配置文件中记录的背包容量，用于读取后校验和实验分组分析。 */
    long long capacity = 0;

    /** 是否启用该数据集。false 表示保留在配置中但暂不参与正式实验。 */
    bool enabled = true;

    /** 备注字段，用于说明小数缩放、困难实例、大容量边界等实验信息。 */
    std::string note;
};

/**
 * @brief 单个算法在单个数据集上的运行结果。
 *
 * 该结构最终会被写入 experiments/results.csv。字段设计覆盖正确性、解质量、
 * 时间开销、空间估算以及回溯法的搜索统计，便于报告中进行多维度对比。
 */
struct SolveResult {
    /** 算法名称：DP / Backtracking / Greedy。 */
    std::string algorithm;

    /** 运行状态：OK、TIMEOUT、SKIPPED_BY_CAPACITY 等。 */
    std::string status;

    /** 算法得到的总价值。精确算法正常完成时应等于 optimum。 */
    long long value = 0;

    /** 选中物品总重量。用于验证结果没有超过 capacity。 */
    long long total_weight = 0;

    /** 选中物品数量。用于辅助分析解的结构。 */
    int selected_count = 0;

    /** 平均运行时间，单位毫秒。多次重复运行时取平均。 */
    double runtime_ms_avg = 0.0;

    /** 内存估算值，单位 KB。DP 的估算值主要体现容量 C 对空间的影响。 */
    long long memory_kb = 0;

    /** 重复运行次数。小规模可多次运行取平均，回溯和 JJ 通常只运行一次。 */
    int repeat_times = 1;

    /** 回溯法访问的搜索树节点数。非回溯算法保持 0。 */
    long long nodes = 0;

    /** 回溯法剪枝次数。非回溯算法保持 0。 */
    long long pruned = 0;

    /** 剪枝率 pruned / nodes。非回溯算法保持 0。 */
    double prune_rate = 0.0;
};

#endif // EXPERIMENT_TYPES_H
