#ifndef STRING_UTILS_H
#define STRING_UTILS_H

/**
 * @file string_utils.h
 * @brief 字符串处理辅助函数。
 *
 * 数据集配置文件和原始数据文件中经常会出现行尾换行、Windows 回车符、字段前后空格等。
 * 如果不统一清理，后续比较 source、group、enabled 等字段时容易出现隐藏错误。
 * 因此这里提供少量简单、可复用的字符串工具。
 */

#include <algorithm>
#include <cctype>
#include <string>

/**
 * @brief 删除字符串首尾空白字符。
 *
 * 这里会处理空格、制表符、换行符和 Windows 文本中常见的 '\r'。
 */
inline std::string trim(const std::string& text) {
    std::size_t begin = 0;
    while (begin < text.size() && std::isspace(static_cast<unsigned char>(text[begin]))) {
        ++begin;
    }

    std::size_t end = text.size();
    while (end > begin && std::isspace(static_cast<unsigned char>(text[end - 1]))) {
        --end;
    }

    return text.substr(begin, end - begin);
}

/**
 * @brief 判断 text 是否以 prefix 开头。
 *
 * 后续可用于根据数据集名称识别 knapPI 类型，或者做简单命令行参数判断。
 */
inline bool startsWith(const std::string& text, const std::string& prefix) {
    if (prefix.size() > text.size()) {
        return false;
    }
    return std::equal(prefix.begin(), prefix.end(), text.begin());
}

/**
 * @brief 转为小写字符串。
 *
 * 用于 enabled 字段、命令行参数等不希望区分大小写的场景。
 */
inline std::string toLower(std::string text) {
    std::transform(text.begin(), text.end(), text.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return text;
}

#endif // STRING_UTILS_H
