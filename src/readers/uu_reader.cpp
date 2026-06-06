#include "data_reader.h"

#include <cmath>
#include <fstream>
#include <stdexcept>
#include <string>

namespace {
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

/** 判断 UU 实例是否需要小数放大。当前正式数据中 f5 是小数实例。 */
bool needsDecimalScaling(const DatasetConfig& config) {
    return config.name == "f5_l-d_kp_15_375" || config.note.find("scaled by 10000") != std::string::npos;
}

/** 将 double 转为 long long；小数实例按 scale 放大，整数实例 scale 为 1。 */
long long scaledInteger(double value, long long scale) {
    return static_cast<long long>(std::llround(value * static_cast<double>(scale)));
}
}

KnapsackInstance readUuInstance(const DatasetConfig& config, const std::string& project_root) {
    // UU 原始格式：第一行 n capacity，后续每行 value weight。
    // 低维 f5 数据包含小数，而 DP 数组下标必须是整数容量，所以本实验统一放大 10000 倍。
    const std::string path = joinPath(project_root, config.path);
    std::ifstream input(path);
    if (!input) {
        throw std::runtime_error("cannot open UU instance: " + path);
    }

    int n = 0;
    double capacity_raw = 0.0;
    input >> n >> capacity_raw;
    if (!input) {
        throw std::runtime_error("invalid UU header: " + path);
    }

    const long long scale = needsDecimalScaling(config) ? 10000LL : 1LL;

    KnapsackInstance instance;
    instance.source = config.source;
    instance.group = config.group;
    instance.name = config.name;
    instance.capacity = scaledInteger(capacity_raw, scale);
    instance.optimum = config.optimum;

    for (int i = 0; i < n; ++i) {
        double value_raw = 0.0;
        double weight_raw = 0.0;
        input >> value_raw >> weight_raw;
        if (!input) {
            throw std::runtime_error("invalid UU item line in: " + path);
        }

        Item item;
        item.id = i + 1;
        item.value = scaledInteger(value_raw, scale);
        item.weight = scaledInteger(weight_raw, scale);
        instance.items.push_back(item);
    }

    return instance;
}
