#ifndef RESULT_WRITER_H
#define RESULT_WRITER_H

/**
 * @file result_writer.h
 * @brief 声明实验结果 CSV 写出工具。
 */

#include "../core/experiment_types.h"

#include <string>
#include <vector>

struct ExperimentRecord;

/** results.csv 的固定表头，必须与实验设计方案中的字段顺序一致。 */
extern const std::string RESULT_CSV_HEADER;

/** 按实验规则计算误差百分比；不可计算时返回 NA。 */
std::string formatErrorPercent(long long optimum, const SolveResult& result);

/** 写主结果文件 experiments/results.csv。 */
void writeResultsCsv(const std::string& output_path, const std::vector<ExperimentRecord>& records);

/** 写回溯超时案例文件，只输出 status 为 TIMEOUT 的记录。 */
void writeTimeoutCasesCsv(const std::string& output_path, const std::vector<ExperimentRecord>& records);

/** 写一份简短运行日志，记录本次输出规模和文件位置。 */
void writeRunLog(const std::string& output_path,
                 const std::vector<ExperimentRecord>& records,
                 const std::string& results_path);

#endif // RESULT_WRITER_H
