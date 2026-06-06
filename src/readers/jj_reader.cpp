#include "data_reader.h"

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
}

KnapsackInstance readJjInstance(const DatasetConfig& config, const std::string& project_root) {
    // JJ 每个实例是一个目录，正式读取其中的 test.in。
    // 格式：第一行 n；中间 n 行为 id profit weight；最后一行为 capacity。
    const std::string path = joinPath(project_root, config.path) + "/test.in";
    std::ifstream input(path);
    if (!input) {
        throw std::runtime_error("cannot open JJ instance: " + path);
    }

    int n = 0;
    input >> n;
    if (!input) {
        throw std::runtime_error("invalid JJ header: " + path);
    }

    KnapsackInstance instance;
    instance.source = config.source;
    instance.group = config.group;
    instance.name = config.name;
    instance.optimum = config.optimum;

    for (int i = 0; i < n; ++i) {
        Item item;
        input >> item.id >> item.value >> item.weight;
        if (!input) {
            throw std::runtime_error("invalid JJ item line in: " + path);
        }
        instance.items.push_back(item);
    }

    input >> instance.capacity;
    if (!input) {
        throw std::runtime_error("missing JJ capacity in: " + path);
    }

    return instance;
}
