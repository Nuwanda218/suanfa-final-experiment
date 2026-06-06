#ifndef DATA_READER_H
#define DATA_READER_H

/**
 * @file data_reader.h
 * @brief 声明数据集配置读取和三类背包数据源读取接口。
 *
 * 本实验正式使用 FSU、UU、JJ 三个数据源。它们原始格式不同：
 * - FSU：一个实例由容量、重量、价值、参考选择方案四个文件组成；
 * - UU：一个实例是单文件，第一行为 n 和 capacity，后续为 value weight；
 * - JJ：一个实例目录下的 test.in 第一行为 n，中间为 id profit weight，最后为 capacity。
 *
 * 为了让算法模块不关心这些差异，reader 会把所有数据源转换为统一的
 * KnapsackInstance 结构。
 */

#include "../core/experiment_types.h"
#include "../core/knapsack_types.h"

#include <string>
#include <vector>

/**
 * @brief 读取 data/datasets.csv，返回所有数据集配置。
 *
 * @param csv_path datasets.csv 路径，可以是相对路径或绝对路径。
 * @return CSV 中的配置列表，不在这里过滤 enabled，过滤逻辑留给 runner。
 */
std::vector<DatasetConfig> readDatasetConfigs(const std::string& csv_path);

/**
 * @brief 根据 DatasetConfig 的 source 字段分发到对应 reader。
 *
 * @param config datasets.csv 中的一行配置。
 * @param project_root 项目根目录。测试中从 src 运行，因此通常传入 ".."。
 * @return 统一后的背包实例。
 */
KnapsackInstance loadInstance(const DatasetConfig& config, const std::string& project_root = ".");

/** 读取 FSU 四文件格式实例。 */
KnapsackInstance readFsuInstance(const DatasetConfig& config, const std::string& project_root = ".");

/** 读取 UU 单文件格式实例。 */
KnapsackInstance readUuInstance(const DatasetConfig& config, const std::string& project_root = ".");

/** 读取 JJ test.in 格式实例。 */
KnapsackInstance readJjInstance(const DatasetConfig& config, const std::string& project_root = ".");

#endif // DATA_READER_H
