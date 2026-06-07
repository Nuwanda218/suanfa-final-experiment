#include "experiment_runner.h"

#include "result_writer.h"

#include "../algorithms/backtracking_knapsack.h"
#include "../algorithms/dp_knapsack.h"
#include "../algorithms/greedy_knapsack.h"
#include "../core/constants.h"
#include "../readers/data_reader.h"

#include <functional>
#include <exception>
#include <string>
#include <vector>

namespace {
using Solver = std::function<SolveResult(const KnapsackInstance&)>;

SolveResult runRepeated(const KnapsackInstance& instance, const Solver& solver, int repeat_times) {
    SolveResult last_result;
    double runtime_total = 0.0;

    for (int i = 0; i < repeat_times; ++i) {
        last_result = solver(instance);
        runtime_total += last_result.runtime_ms_avg;
    }

    last_result.repeat_times = repeat_times;
    last_result.runtime_ms_avg = repeat_times > 0 ? runtime_total / repeat_times : 0.0;
    return last_result;
}

void appendRecord(std::vector<ExperimentRecord>& records,
                  const DatasetConfig& config,
                  const SolveResult& result) {
    ExperimentRecord record;
    record.config = config;
    record.result = result;
    record.error_percent = formatErrorPercent(config.optimum, result);
    records.push_back(record);
}

void appendParseErrorRecords(std::vector<ExperimentRecord>& records, const DatasetConfig& config) {
    const std::vector<std::string> algorithms = {"DP", "Backtracking", "Greedy"};
    for (const std::string& algorithm : algorithms) {
        SolveResult result;
        result.algorithm = algorithm;
        result.status = STATUS_PARSE_ERROR;
        result.repeat_times = 0;
        appendRecord(records, config, result);
    }
}
}

int repeatTimesForGroup(const std::string& group) {
    if (group == "fsu_correctness" || group == "fsu_capacity_pressure" || group == "uu_low") {
        return REPEAT_SMALL;
    }
    if (group.find("uu_large") == 0) {
        return REPEAT_LARGE;
    }
    return REPEAT_HARD;
}

std::vector<ExperimentRecord> runExperiments(const std::vector<DatasetConfig>& configs,
                                             const std::string& project_root) {
    std::vector<ExperimentRecord> records;

    for (const DatasetConfig& config : configs) {
        if (!config.enabled) {
            continue;
        }

        KnapsackInstance instance;
        try {
            instance = loadInstance(config, project_root);
        } catch (const std::exception&) {
            appendParseErrorRecords(records, config);
            continue;
        }

        const int repeat_times = repeatTimesForGroup(config.group);

        appendRecord(records, config, runRepeated(instance, solveDp, repeat_times));

        // 回溯法可能指数级增长。按实施计划无论分组如何都只运行 1 次，并依赖算法内部超时返回。
        appendRecord(records, config, runRepeated(instance, solveBacktracking, 1));

        appendRecord(records, config, runRepeated(instance, solveGreedy, repeat_times));
    }

    return records;
}

std::vector<ExperimentRecord> runExperimentsFromCsv(const std::string& csv_path,
                                                    const std::string& project_root) {
    return runExperiments(readDatasetConfigs(csv_path), project_root);
}
