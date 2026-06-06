#ifndef CONSTANTS_H
#define CONSTANTS_H

/**
 * @file constants.h
 * @brief 保存实验运行规则中需要统一使用的常量。
 *
 * 本文件中的阈值不是 0/1 背包问题本身的数学定义，而是为了让结课实验可以稳定运行。
 * 例如 JJ 数据集中存在非常大的容量，如果不加限制直接运行 O(nC) 的 DP，程序可能长时间
 * 无响应或占用过多内存。因此 runner 会根据这些常量主动跳过不适合运行的组合，并把跳过
 * 原因写入结果文件。
 */

#include <string>

/** DP 容量阈值。capacity 超过该值时，DP 记录为 SKIPPED_BY_CAPACITY。 */
constexpr long long DP_CAPACITY_LIMIT = 10000000LL;

/** 回溯法时间限制，单位毫秒。超过该时间后返回当前最好结果并记录 TIMEOUT。 */
constexpr int BACKTRACKING_TIME_LIMIT_MS = 10000;

/** FSU 和 UU 低维组重复次数，用于减少计时波动。 */
constexpr int REPEAT_SMALL = 5;

/** UU 大规模组重复次数。规模较大时减少重复次数，避免实验耗时过长。 */
constexpr int REPEAT_LARGE = 3;

/** JJ 困难实例重复次数。困难实例通常只运行一次。 */
constexpr int REPEAT_HARD = 1;

/** 正常完成。 */
const std::string STATUS_OK = "OK";

/** 回溯法超过时间限制。 */
const std::string STATUS_TIMEOUT = "TIMEOUT";

/** DP 因容量过大被主动跳过。 */
const std::string STATUS_SKIPPED_BY_CAPACITY = "SKIPPED_BY_CAPACITY";

/** 内存不足或主动判定内存压力过高。 */
const std::string STATUS_MEMORY_LIMIT = "MEMORY_LIMIT";

/** 数据读取或格式解析失败。 */
const std::string STATUS_PARSE_ERROR = "PARSE_ERROR";

/** 数据集没有可用最优值，算法仍可运行但不能计算误差。 */
const std::string STATUS_NO_OPTIMUM = "NO_OPTIMUM";

#endif // CONSTANTS_H
