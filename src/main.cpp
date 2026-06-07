/**
 * @file main.cpp
 * @brief 0/1 背包三算法对比实验程序入口。
 *
 * 主程序负责把命令行参数转换为实验配置过滤规则，并调用 runner 批量执行实验。
 * 为避免误触发长时间实验，不带参数时只打印用法，不直接运行全部数据。
 */

#include "core/experiment_types.h"
#include "readers/data_reader.h"
#include "runner/experiment_runner.h"
#include "runner/result_writer.h"

#include <iostream>
#include <string>
#include <vector>

namespace {
void printUsage() {
    std::cout << "0/1 Knapsack Experiment" << '\n';
    std::cout << "usage:" << '\n';
    std::cout << "  knapsack_experiment --check-data" << '\n';
    std::cout << "  knapsack_experiment --run-all" << '\n';
    std::cout << "  knapsack_experiment --run-group <group>" << '\n';
    std::cout << "  knapsack_experiment --run-one <source> <name>" << '\n';
}

void writeExperimentOutputs(const std::vector<ExperimentRecord>& records, const std::string& project_root) {
    const std::string results_path = project_root + "/experiments/results.csv";
    const std::string timeout_path = project_root + "/experiments/timeout_cases.csv";
    const std::string log_path = project_root + "/experiments/run_log.txt";

    writeResultsCsv(results_path, records);
    writeTimeoutCasesCsv(timeout_path, records);
    writeRunLog(log_path, records, results_path);

    std::cout << "records=" << records.size() << '\n';
    std::cout << "results=" << results_path << '\n';
    std::cout << "timeouts=" << timeout_path << '\n';
    std::cout << "log=" << log_path << '\n';
}

std::vector<DatasetConfig> filterGroup(const std::vector<DatasetConfig>& configs, const std::string& group) {
    std::vector<DatasetConfig> filtered;
    for (const DatasetConfig& config : configs) {
        if (config.group == group) {
            filtered.push_back(config);
        }
    }
    return filtered;
}

std::vector<DatasetConfig> filterOne(const std::vector<DatasetConfig>& configs,
                                     const std::string& source,
                                     const std::string& name) {
    std::vector<DatasetConfig> filtered;
    for (const DatasetConfig& config : configs) {
        if (config.source == source && config.name == name) {
            filtered.push_back(config);
        }
    }
    return filtered;
}
}

int main(int argc, char* argv[]) {
    const std::string project_root = "..";
    const std::string csv_path = project_root + "/data/datasets.csv";

    if (argc <= 1) {
        printUsage();
        return 0;
    }

    const std::string command = argv[1];
    const std::vector<DatasetConfig> configs = readDatasetConfigs(csv_path);

    if (command == "--check-data") {
        int enabled_count = 0;
        for (const DatasetConfig& config : configs) {
            if (config.enabled) {
                ++enabled_count;
            }
        }
        std::cout << "datasets=" << configs.size() << '\n';
        std::cout << "enabled=" << enabled_count << '\n';
        return 0;
    }

    if (command == "--run-all") {
        writeExperimentOutputs(runExperiments(configs, project_root), project_root);
        return 0;
    }

    if (command == "--run-group" && argc >= 3) {
        writeExperimentOutputs(runExperiments(filterGroup(configs, argv[2]), project_root), project_root);
        return 0;
    }

    if (command == "--run-one" && argc >= 4) {
        writeExperimentOutputs(runExperiments(filterOne(configs, argv[2], argv[3]), project_root), project_root);
        return 0;
    }

    printUsage();
    return 1;
}
