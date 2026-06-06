#include "data_reader.h"

#include "../utils/csv_utils.h"
#include "../utils/string_utils.h"

#include <cmath>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
/**
 * @brief 拼接项目根目录和相对路径。
 *
 * datasets.csv 中的 path 都相对于项目根目录 3024244427-3-1，而测试和程序通常从 src
 * 目录运行，因此需要把 project_root 和相对 path 拼接起来。
 */
std::string joinPath(const std::string& root, const std::string& relative) {
    if (root.empty() || root == ".") {
        return relative;
    }
    const char last = root[root.size() - 1];
    if (last == '/' || last == '\\') {
        return root + relative;
    }
    return root + "/" + relative;
}

/** 把字符串转成 bool。datasets.csv 中 enabled 使用 1/0，也兼容 true/false。 */
bool parseEnabled(const std::string& text) {
    const std::string value = toLower(trim(text));
    return value == "1" || value == "true" || value == "yes" || value == "y";
}

/** 读取一个文本文件中的所有数值，并四舍五入为 long long。 */
std::vector<long long> readNumberList(const std::string& path) {
    std::ifstream input(path);
    if (!input) {
        throw std::runtime_error("cannot open number list file: " + path);
    }

    std::vector<long long> numbers;
    double value = 0.0;
    while (input >> value) {
        numbers.push_back(static_cast<long long>(std::llround(value)));
    }
    return numbers;
}

/** 读取单个整数文件，例如 FSU 的容量文件。 */
long long readSingleNumber(const std::string& path) {
    std::vector<long long> numbers = readNumberList(path);
    if (numbers.empty()) {
        throw std::runtime_error("empty number file: " + path);
    }
    return numbers[0];
}
}

std::vector<DatasetConfig> readDatasetConfigs(const std::string& csv_path) {
    std::ifstream input(csv_path);
    if (!input) {
        throw std::runtime_error("cannot open datasets csv: " + csv_path);
    }

    std::vector<DatasetConfig> configs;
    std::string line;

    // 第一行是表头。当前字段顺序由 data/datasets.csv 固定，后续按位置读取。
    if (!std::getline(input, line)) {
        return configs;
    }

    while (std::getline(input, line)) {
        line = trim(line);
        if (line.empty()) {
            continue;
        }

        const std::vector<std::string> fields = splitCsvLine(line);
        if (fields.size() < 9) {
            throw std::runtime_error("invalid datasets csv line: " + line);
        }

        DatasetConfig config;
        config.source = trim(fields[0]);
        config.group = trim(fields[1]);
        config.name = trim(fields[2]);
        config.path = trim(fields[3]);
        config.optimum = std::stoll(trim(fields[4]));
        config.n = std::stoi(trim(fields[5]));
        config.capacity = std::stoll(trim(fields[6]));
        config.enabled = parseEnabled(fields[7]);
        config.note = trim(fields[8]);
        configs.push_back(config);
    }

    return configs;
}

KnapsackInstance loadInstance(const DatasetConfig& config, const std::string& project_root) {
    if (config.source == "FSU") {
        return readFsuInstance(config, project_root);
    }
    if (config.source == "UU") {
        return readUuInstance(config, project_root);
    }
    if (config.source == "JJ") {
        return readJjInstance(config, project_root);
    }
    throw std::runtime_error("unknown dataset source: " + config.source);
}

KnapsackInstance readFsuInstance(const DatasetConfig& config, const std::string& project_root) {
    // FSU 的 path 是不带后缀的前缀，例如 data/FSU/p01。
    // 真实文件由该前缀加 _c.txt、_w.txt、_p.txt、_s.txt 组成。
    const std::string prefix = joinPath(project_root, config.path);
    const long long capacity = readSingleNumber(prefix + "_c.txt");
    const std::vector<long long> weights = readNumberList(prefix + "_w.txt");
    const std::vector<long long> values = readNumberList(prefix + "_p.txt");
    const std::vector<long long> selected = readNumberList(prefix + "_s.txt");

    if (weights.size() != values.size() || values.size() != selected.size()) {
        throw std::runtime_error("FSU value/weight/solution length mismatch: " + config.name);
    }

    KnapsackInstance instance;
    instance.source = config.source;
    instance.group = config.group;
    instance.name = config.name;
    instance.capacity = capacity;
    instance.optimum = 0;

    for (std::size_t i = 0; i < values.size(); ++i) {
        Item item;
        item.id = static_cast<int>(i) + 1;
        item.value = values[i];
        item.weight = weights[i];
        instance.items.push_back(item);

        // FSU 的 pXX_s.txt 是参考最优选择方案，因此可直接用它计算 optimum。
        if (selected[i] != 0) {
            instance.optimum += values[i];
        }
    }

    return instance;
}
