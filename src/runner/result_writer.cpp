#include "result_writer.h"

#include "experiment_runner.h"

#include "../core/constants.h"
#include "../utils/csv_utils.h"

#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

const std::string RESULT_CSV_HEADER =
    "source,group,dataset,n,capacity,algorithm,status,optimum,value,total_weight,selected_count,"
    "error_percent,runtime_ms_avg,memory_kb,repeat_times,nodes,pruned,prune_rate";

namespace {
void ensureOpen(const std::ofstream& output, const std::string& output_path) {
    if (!output.is_open()) {
        throw std::runtime_error("failed to open output file: " + output_path);
    }
}

std::string formatDouble(double value, int precision) {
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(precision) << value;
    return stream.str();
}

std::string recordToCsvLine(const ExperimentRecord& record) {
    const DatasetConfig& config = record.config;
    const SolveResult& result = record.result;

    std::vector<std::string> fields = {
        config.source,
        config.group,
        config.name,
        std::to_string(config.n),
        std::to_string(config.capacity),
        result.algorithm,
        result.status,
        std::to_string(config.optimum),
        std::to_string(result.value),
        std::to_string(result.total_weight),
        std::to_string(result.selected_count),
        record.error_percent,
        formatDouble(result.runtime_ms_avg, 4),
        std::to_string(result.memory_kb),
        std::to_string(result.repeat_times),
        std::to_string(result.nodes),
        std::to_string(result.pruned),
        formatDouble(result.prune_rate, 4)
    };

    std::ostringstream line;
    for (std::size_t i = 0; i < fields.size(); ++i) {
        if (i > 0) {
            line << ',';
        }
        line << escapeCsv(fields[i]);
    }
    return line.str();
}
}

std::string formatErrorPercent(long long optimum, const SolveResult& result) {
    if (optimum <= 0 || result.status != STATUS_OK) {
        return "NA";
    }

    const double error = (static_cast<double>(optimum - result.value) / static_cast<double>(optimum)) * 100.0;
    return formatDouble(error, 4);
}

void writeResultsCsv(const std::string& output_path, const std::vector<ExperimentRecord>& records) {
    std::ofstream output(output_path);
    ensureOpen(output, output_path);

    output << RESULT_CSV_HEADER << '\n';
    for (const ExperimentRecord& record : records) {
        output << recordToCsvLine(record) << '\n';
    }
}

void writeTimeoutCasesCsv(const std::string& output_path, const std::vector<ExperimentRecord>& records) {
    std::ofstream output(output_path);
    ensureOpen(output, output_path);

    output << RESULT_CSV_HEADER << '\n';
    for (const ExperimentRecord& record : records) {
        if (record.result.status == STATUS_TIMEOUT) {
            output << recordToCsvLine(record) << '\n';
        }
    }
}

void writeRunLog(const std::string& output_path,
                 const std::vector<ExperimentRecord>& records,
                 const std::string& results_path) {
    std::ofstream output(output_path);
    ensureOpen(output, output_path);

    output << "records=" << records.size() << '\n';
    output << "results=" << results_path << '\n';
}
