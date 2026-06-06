#ifndef CSV_UTILS_H
#define CSV_UTILS_H

/**
 * @file csv_utils.h
 * @brief CSV 读取和写出时使用的辅助函数。
 *
 * 本实验的 data/datasets.csv 和 experiments/results.csv 都使用 CSV 格式。
 * 虽然大多数字段较简单，但 note 字段可能包含逗号或引号，因此仍然需要基本的
 * CSV 转义和解析逻辑，避免结果文件在表格软件中打开时错列。
 */

#include <string>
#include <vector>

/**
 * @brief 拆分一行 CSV。
 *
 * 支持双引号包裹字段，以及 CSV 标准中的双引号转义："" 表示字段内部的一个 "。
 * 该函数足够覆盖本实验配置文件和结果文件的需求，但不负责处理跨行字段。
 */
inline std::vector<std::string> splitCsvLine(const std::string& line) {
    std::vector<std::string> fields;
    std::string current;
    bool in_quotes = false;

    for (std::size_t i = 0; i < line.size(); ++i) {
        char ch = line[i];

        if (ch == '"') {
            if (in_quotes && i + 1 < line.size() && line[i + 1] == '"') {
                current.push_back('"');
                ++i;
            } else {
                in_quotes = !in_quotes;
            }
        } else if (ch == ',' && !in_quotes) {
            fields.push_back(current);
            current.clear();
        } else {
            current.push_back(ch);
        }
    }

    fields.push_back(current);
    return fields;
}

/**
 * @brief 写 CSV 字段时进行必要转义。
 *
 * 如果字段中包含逗号、双引号、换行符或回车符，则用双引号包裹；字段内部的双引号
 * 按 CSV 规则写成两个双引号。这样生成的 results.csv 可以被 Excel/WPS 正确识别。
 */
inline std::string escapeCsv(const std::string& field) {
    bool need_quotes = false;
    for (char ch : field) {
        if (ch == ',' || ch == '"' || ch == '\n' || ch == '\r') {
            need_quotes = true;
            break;
        }
    }

    if (!need_quotes) {
        return field;
    }

    std::string escaped = "\"";
    for (char ch : field) {
        if (ch == '"') {
            escaped += "\"\"";
        } else {
            escaped.push_back(ch);
        }
    }
    escaped.push_back('"');
    return escaped;
}

#endif // CSV_UTILS_H
