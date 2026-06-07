#ifndef EXPERIMENT_RUNNER_H
#define EXPERIMENT_RUNNER_H

/**
 * @file experiment_runner.h
 * @brief 声明批量实验运行器。
 *
 * Runner 负责把 reader、三种算法和结果写出模块串起来：读取配置、过滤 enabled、
 * 加载实例、运行 DP/回溯/贪心，并为每条结果补充误差字段。
 */

#include "../core/experiment_types.h"

#include <string>
#include <vector>

/**
 * @brief 单条实验记录。
 *
 * config 保存数据集元信息，result 保存算法输出，error_percent 是按已知 optimum
 * 计算出的字符串字段。未知或不可计算时使用 NA，便于直接写入 CSV。
 */
struct ExperimentRecord {
    DatasetConfig config;
    SolveResult result;
    std::string error_percent;
};

/** 根据数据分组决定 DP/贪心算法重复次数。回溯法在 runner 中固定运行 1 次。 */
int repeatTimesForGroup(const std::string& group);

/** 对传入配置批量运行实验。disabled 配置会被跳过。 */
std::vector<ExperimentRecord> runExperiments(const std::vector<DatasetConfig>& configs,
                                             const std::string& project_root = ".");

/** 从 datasets.csv 读取配置后运行全部 enabled 实验。 */
std::vector<ExperimentRecord> runExperimentsFromCsv(const std::string& csv_path,
                                                    const std::string& project_root = ".");

#endif // EXPERIMENT_RUNNER_H
