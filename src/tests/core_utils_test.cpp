#include "../core/knapsack_types.h"
#include "../core/experiment_types.h"
#include "../core/constants.h"
#include "../utils/string_utils.h"
#include "../utils/csv_utils.h"
#include "../utils/timer.h"

#include <cassert>
#include <string>
#include <vector>

int main() {
    // 验证统一物品结构可以保存 0/1 背包实例中的基本字段。
    Item item{1, 55, 95};
    assert(item.id == 1);
    assert(item.value == 55);
    assert(item.weight == 95);

    // 验证不同数据源最终都能被统一表示为 KnapsackInstance。
    KnapsackInstance instance;
    instance.source = "UU";
    instance.group = "uu_low";
    instance.name = "f1_l-d_kp_10_269";
    instance.capacity = 269;
    instance.optimum = 295;
    instance.items.push_back(item);
    assert(instance.n() == 1);

    // 验证数据集配置结构与 datasets.csv 字段一致。
    DatasetConfig config;
    config.source = "FSU";
    config.group = "fsu_correctness";
    config.name = "p01";
    config.path = "data/FSU/p01";
    config.enabled = true;
    assert(config.enabled);

    // 验证实验状态和阈值常量可被后续 runner 统一使用。
    assert(DP_CAPACITY_LIMIT == 10000000LL);
    assert(BACKTRACKING_TIME_LIMIT_MS == 10000);
    assert(STATUS_OK == std::string("OK"));
    assert(STATUS_SKIPPED_BY_CAPACITY == std::string("SKIPPED_BY_CAPACITY"));

    // 验证字符串工具能够清理 CSV 和配置文件读取时常见的空白字符。
    assert(trim("  knapsack \r\n") == "knapsack");
    assert(startsWith("knapPI_1_100", "knapPI"));

    // 验证 CSV 工具能处理带逗号和双引号的字段。
    std::vector<std::string> fields = splitCsvLine("UU,uu_low,\"a,b\",\"quote \"\"x\"\"\"");
    assert(fields.size() == 4);
    assert(fields[2] == "a,b");
    assert(fields[3] == "quote \"x\"");
    assert(escapeCsv("a,b") == "\"a,b\"");

    // 验证计时器接口可用，具体耗时只要求非负。
    Timer timer;
    assert(timer.elapsedMs() >= 0.0);

    return 0;
}
