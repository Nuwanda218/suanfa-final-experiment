#ifndef TIMER_H
#define TIMER_H

/**
 * @file timer.h
 * @brief 高精度计时工具。
 *
 * 本实验需要比较 DP、回溯法和贪心算法在不同数据规模上的运行时间。
 * 为了避免每个算法重复编写 chrono 代码，这里把计时逻辑封装为一个简单 Timer 类。
 */

#include <chrono>

/**
 * @brief 简单的毫秒级计时器。
 *
 * 使用 std::chrono::high_resolution_clock 获取时间点。算法开始前构造或 reset，
 * 算法结束后调用 elapsedMs()，即可得到毫秒级运行时间。
 */
class Timer {
public:
    Timer() {
        reset();
    }

    /** 重置起始时间。 */
    void reset() {
        start_time_ = Clock::now();
    }

    /** 返回从上次 reset 到当前时刻经过的毫秒数。 */
    double elapsedMs() const {
        const auto now = Clock::now();
        const std::chrono::duration<double, std::milli> elapsed = now - start_time_;
        return elapsed.count();
    }

private:
    using Clock = std::chrono::high_resolution_clock;

    /** 计时起点。 */
    Clock::time_point start_time_;
};

#endif // TIMER_H
