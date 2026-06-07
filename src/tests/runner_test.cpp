#include "../core/constants.h"
#include "../readers/data_reader.h"
#include "../runner/experiment_runner.h"
#include "../runner/result_writer.h"

#include <cassert>
#include <cstdio>
#include <fstream>
#include <sstream>
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

const ExperimentRecord& findRecord(const std::vector<ExperimentRecord>& records, const std::string& algorithm) {
    for (const ExperimentRecord& record : records) {
        if (record.result.algorithm == algorithm) {
            return record;
        }
    }
    assert(false && "required algorithm result not found");
    return records.front();
}
}

int main() {
    const std::string project_root = "..";
    const std::vector<DatasetConfig> configs = readDatasetConfigs(project_root + "/data/datasets.csv");

    DatasetConfig enabled = findConfig(configs, "FSU", "p02");
    DatasetConfig disabled = findConfig(configs, "FSU", "p03");
    disabled.enabled = false;

    DatasetConfig bad_config;
    bad_config.source = "BAD";
    bad_config.group = "parse_error";
    bad_config.name = "bad_source";
    bad_config.path = "missing";
    bad_config.optimum = -1;
    bad_config.n = 0;
    bad_config.capacity = 0;
    bad_config.enabled = true;

    std::vector<ExperimentRecord> records = runExperiments({enabled, disabled, bad_config}, project_root);
    assert(records.size() == 6);

    const ExperimentRecord& dp = findRecord(records, "DP");
    assert(dp.config.name == "p02");
    assert(dp.result.status == STATUS_OK);
    assert(dp.result.value == enabled.optimum);
    assert(dp.result.repeat_times == REPEAT_SMALL);
    assert(dp.error_percent == "0.0000");

    const ExperimentRecord& backtracking = findRecord(records, "Backtracking");
    assert(backtracking.result.status == STATUS_OK);
    assert(backtracking.result.value == enabled.optimum);
    assert(backtracking.result.repeat_times == 1);
    assert(backtracking.result.nodes > 0);

    const ExperimentRecord& greedy = findRecord(records, "Greedy");
    assert(greedy.result.status == STATUS_OK);
    assert(greedy.result.total_weight <= enabled.capacity);
    assert(greedy.result.repeat_times == REPEAT_SMALL);

    int parse_error_records = 0;
    for (const ExperimentRecord& record : records) {
        if (record.config.name == "bad_source") {
            assert(record.result.status == STATUS_PARSE_ERROR);
            assert(record.error_percent == "NA");
            ++parse_error_records;
        }
    }
    assert(parse_error_records == 3);

    SolveResult skipped;
    skipped.algorithm = "DP";
    skipped.status = STATUS_SKIPPED_BY_CAPACITY;
    skipped.value = 0;
    assert(formatErrorPercent(100, skipped) == "NA");
    assert(formatErrorPercent(-1, dp.result) == "NA");

    const std::string result_path = project_root + "/experiments/runner_test_results.csv";
    writeResultsCsv(result_path, records);

    std::ifstream input(result_path);
    assert(input.is_open());

    std::string header;
    std::getline(input, header);
    assert(header == RESULT_CSV_HEADER);

    int data_lines = 0;
    std::string line;
    while (std::getline(input, line)) {
        if (!line.empty()) {
            ++data_lines;
        }
    }
    assert(data_lines == 6);
    input.close();

    std::remove(result_path.c_str());

    return 0;
}
