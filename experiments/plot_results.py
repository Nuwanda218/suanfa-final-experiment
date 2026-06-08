#!/usr/bin/env python3
"""
为 0/1 背包结课实验生成三轮迭代图表、图表说明、报告初稿和 PPT 方案。

输入：experiments/results.csv
输出：
- reports/figures/iteration1/*.png
- reports/figures/iteration2/*.png
- reports/figures/final/*.png
- reports/项目完成情况检查.md
- reports/图表优化说明.md
- reports/图表说明.md
- reports/结果分析草稿.md
- reports/实验报告初稿.md
- reports/PPT方案.md
"""

from __future__ import annotations

import csv
import re
from collections import Counter, defaultdict
from pathlib import Path
from typing import Dict, Iterable, List, Optional, Sequence

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

ROOT = Path(__file__).resolve().parents[1]
RESULTS_CSV = ROOT / "experiments" / "results.csv"
REPORT_DIR = ROOT / "reports"
FIGURE_DIR = REPORT_DIR / "figures"
ITER1 = FIGURE_DIR / "iteration1"
ITER2 = FIGURE_DIR / "iteration2"
FINAL = FIGURE_DIR / "final"

ALGORITHMS = ["DP", "Backtracking", "Greedy"]
COLORS = {
    "DP": "#4C78A8",
    "Backtracking": "#F58518",
    "Greedy": "#54A24B",
    "OK": "#54A24B",
    "TIMEOUT": "#E45756",
    "SKIPPED_BY_CAPACITY": "#9D9DA1",
}
NUMERIC_KEYS = [
    "n", "capacity", "optimum", "value", "total_weight", "selected_count",
    "runtime_ms_avg", "memory_kb", "repeat_times", "nodes", "pruned", "prune_rate",
]


def parse_number(text: str):
    if text == "NA" or text == "":
        return None
    return float(text) if "." in text else int(text)


def read_results() -> List[Dict[str, object]]:
    with RESULTS_CSV.open("r", encoding="utf-8", newline="") as f:
        rows: List[Dict[str, object]] = list(csv.DictReader(f))
    for row in rows:
        for key in NUMERIC_KEYS:
            row[key] = parse_number(str(row[key]))
        row["error_value"] = parse_number(str(row["error_percent"]))
    return rows


def average(values: Iterable[float]) -> float:
    values = list(values)
    return sum(values) / len(values) if values else 0.0


def fmt(value: float) -> str:
    return f"{value:.4f}"


def safe_runtime(value: object) -> float:
    # 对数坐标不能显示 0。这里仅用于显示，原始 results.csv 不变。
    try:
        return max(float(value or 0), 0.001)
    except (TypeError, ValueError):
        return 0.001


def format_capacity(capacity: int) -> str:
    if capacity >= 10_000_000_000:
        return "1e10"
    if capacity >= 100_000_000:
        return "1e8"
    if capacity >= 1_000_000:
        return "1e6"
    if capacity >= 10_000:
        return f"{capacity // 1000}k"
    return str(capacity)


def short_label(row_or_name) -> str:
    dataset = str(row_or_name.get("dataset") if isinstance(row_or_name, dict) else row_or_name)
    capacity = int(row_or_name.get("capacity", 0) or 0) if isinstance(row_or_name, dict) else 0
    if re.fullmatch(r"p\d+", dataset):
        return f"FSU-{dataset}"
    if dataset.startswith("f"):
        m = re.match(r"f(\d+)_", dataset)
        return f"UU-f{m.group(1)}" if m else dataset
    m = re.match(r"knapPI_(\d+)_(\d+)_", dataset)
    if m:
        return f"PI{m.group(1)}-n{m.group(2)}"
    m = re.match(r"n_(\d+)_c_(\d+)_", dataset)
    if m:
        return f"JJ-n{m.group(1)}-C{format_capacity(int(m.group(2)))}"
    return dataset if not capacity else f"{dataset}-C{format_capacity(capacity)}"


def unique_datasets(rows: Iterable[Dict[str, object]]) -> List[str]:
    return sorted({str(r["dataset"]) for r in rows})


def by_dataset_algorithm(rows: Iterable[Dict[str, object]]) -> Dict[tuple, Dict[str, object]]:
    return {(str(r["dataset"]), str(r["algorithm"])): r for r in rows}


def grouped_values(rows: List[Dict[str, object]], labels: Sequence[str], algorithms: Sequence[str], field: str, runtime=False):
    lookup = by_dataset_algorithm(rows)
    result = {algorithm: [] for algorithm in algorithms}
    for label in labels:
        for algorithm in algorithms:
            row = lookup.get((label, algorithm))
            value = row[field] if row else 0
            result[algorithm].append(safe_runtime(value) if runtime else (value or 0))
    return result


def apply_style(title: str, ylabel: str, log_y: bool = False):
    plt.title(title, fontsize=13, weight="bold")
    plt.ylabel(ylabel)
    plt.grid(axis="y", linestyle="--", alpha=0.35)
    if log_y:
        plt.yscale("log")


def save_fig(directory: Path, filename: str) -> Path:
    directory.mkdir(parents=True, exist_ok=True)
    path = directory / filename
    plt.tight_layout()
    plt.savefig(path, dpi=220, bbox_inches="tight")
    plt.close()
    return path


def status_counts(rows: Iterable[Dict[str, object]], algorithm: str) -> Counter:
    return Counter(str(r["status"]) for r in rows if r["algorithm"] == algorithm)


def summarize(rows: List[Dict[str, object]]) -> Dict[str, object]:
    fsu = [r for r in rows if r["group"] == "fsu_correctness"]
    uu_low = [r for r in rows if r["group"] == "uu_low"]
    uu_large = [r for r in rows if r["source"] == "UU" and str(r["group"]).startswith("uu_large")]
    jj = [r for r in rows if r["source"] == "JJ"]
    fsu_g = [float(r["error_value"] or 0) for r in fsu if r["algorithm"] == "Greedy" and r["error_value"] is not None]
    uu_g = [float(r["error_value"] or 0) for r in uu_low if r["algorithm"] == "Greedy" and r["error_value"] is not None]
    jj_g = [float(r["error_value"] or 0) for r in jj if r["algorithm"] == "Greedy" and r["error_value"] is not None]
    return {
        "records": len(rows),
        "datasets": len({r["dataset"] for r in rows}),
        "fsu": len({r["dataset"] for r in rows if r["source"] == "FSU"}),
        "uu": len({r["dataset"] for r in rows if r["source"] == "UU"}),
        "jj": len({r["dataset"] for r in rows if r["source"] == "JJ"}),
        "fsu_dp_exact": sum(1 for r in fsu if r["algorithm"] == "DP" and r["value"] == r["optimum"]),
        "fsu_bt_exact": sum(1 for r in fsu if r["algorithm"] == "Backtracking" and r["value"] == r["optimum"]),
        "fsu_g_avg": average(fsu_g),
        "fsu_g_max": max(fsu_g) if fsu_g else 0.0,
        "uu_g_avg": average(uu_g),
        "uu_g_max": max(uu_g) if uu_g else 0.0,
        "jj_g_avg": average(jj_g),
        "jj_g_max": max(jj_g) if jj_g else 0.0,
        "uu_large_count": len({r["dataset"] for r in uu_large}),
        "jj_dp_ok": status_counts(jj, "DP")["OK"],
        "jj_dp_skip": status_counts(jj, "DP")["SKIPPED_BY_CAPACITY"],
        "jj_bt_timeout": status_counts(jj, "Backtracking")["TIMEOUT"],
        "jj_g_ok": status_counts(jj, "Greedy")["OK"],
    }


# -------- iteration 1: improve readability while retaining original chart ideas --------

def iter1_fsu(rows):
    data = [r for r in rows if r["group"] == "fsu_correctness"]
    labels = unique_datasets(data)
    values = grouped_values(data, labels, ALGORITHMS, "value")
    x = list(range(len(labels)))
    width = 0.25
    plt.figure(figsize=(10, 4.8))
    for i, alg in enumerate(ALGORITHMS):
        plt.bar([v + (i - 1) * width for v in x], values[alg], width=width, label=alg, color=COLORS[alg])
    plt.xticks(x, [short_label(v) for v in labels])
    apply_style("Iteration 1 - FSU solution values", "Value")
    plt.legend()
    return save_fig(ITER1, "iter1_fsu_correctness_value.png")


def iter1_uu_low_time(rows):
    data = [r for r in rows if r["group"] == "uu_low"]
    labels = unique_datasets(data)
    values = grouped_values(data, labels, ALGORITHMS, "runtime_ms_avg", runtime=True)
    x = list(range(len(labels)))
    width = 0.25
    plt.figure(figsize=(10, 4.8))
    for i, alg in enumerate(ALGORITHMS):
        plt.bar([v + (i - 1) * width for v in x], values[alg], width=width, label=alg, color=COLORS[alg])
    plt.xticks(x, [short_label(v) for v in labels])
    apply_style("Iteration 1 - UU low runtime", "Runtime (ms, log)", log_y=True)
    plt.legend()
    return save_fig(ITER1, "iter1_uu_low_time.png")


def iter1_greedy_error(rows):
    data = [r for r in rows if r["algorithm"] == "Greedy" and r["error_value"] is not None]
    data.sort(key=lambda r: (str(r["source"]), str(r["group"]), int(r["n"] or 0), int(r["capacity"] or 0)))
    labels = [short_label(r) for r in data]
    values = [float(r["error_value"] or 0) for r in data]
    colors = ["#4C78A8" if r["source"] == "FSU" else "#54A24B" if r["source"] == "UU" else "#F58518" for r in data]
    plt.figure(figsize=(14, 5.2))
    plt.bar(range(len(labels)), values, color=colors)
    plt.xticks(range(len(labels)), labels, rotation=55, ha="right", fontsize=8)
    apply_style("Iteration 1 - Greedy relative error", "Error (%)")
    return save_fig(ITER1, "iter1_greedy_error.png")


def iter1_uu_large_time(rows):
    data = [r for r in rows if r["source"] == "UU" and str(r["group"]).startswith("uu_large") and r["algorithm"] in {"DP", "Greedy"}]
    labels = unique_datasets(data)
    values = grouped_values(data, labels, ["DP", "Greedy"], "runtime_ms_avg", runtime=True)
    x = list(range(len(labels)))
    width = 0.35
    plt.figure(figsize=(10, 4.8))
    for i, alg in enumerate(["DP", "Greedy"]):
        plt.bar([v + (i - 0.5) * width for v in x], values[alg], width=width, label=alg, color=COLORS[alg])
    plt.xticks(x, [short_label(v) for v in labels], rotation=25, ha="right")
    apply_style("Iteration 1 - UU large runtime", "Runtime (ms, log)", log_y=True)
    plt.legend()
    return save_fig(ITER1, "iter1_uu_large_time.png")


def iter1_backtracking(rows):
    data = [r for r in rows if r["algorithm"] == "Backtracking"]
    data.sort(key=lambda r: (str(r["source"]), str(r["group"]), int(r["n"] or 0), int(r["capacity"] or 0)))
    labels = [short_label(r) for r in data]
    nodes = [max(int(r["nodes"] or 0), 1) for r in data]
    pruned = [max(int(r["pruned"] or 0), 1) for r in data]
    x = list(range(len(labels)))
    width = 0.35
    plt.figure(figsize=(15, 5.8))
    plt.bar([v - width / 2 for v in x], nodes, width=width, label="Nodes", color="#4C78A8")
    plt.bar([v + width / 2 for v in x], pruned, width=width, label="Pruned", color="#F58518")
    plt.xticks(x, labels, rotation=65, ha="right", fontsize=8)
    apply_style("Iteration 1 - Backtracking nodes/pruned", "Count (log)", log_y=True)
    plt.legend()
    return save_fig(ITER1, "iter1_backtracking_nodes_pruned.png")


def iter1_jj_status(rows):
    data = [r for r in rows if r["source"] == "JJ"]
    statuses = ["OK", "TIMEOUT", "SKIPPED_BY_CAPACITY"]
    counts = defaultdict(Counter)
    for r in data:
        counts[str(r["algorithm"])][str(r["status"])] += 1
    x = list(range(len(ALGORITHMS)))
    bottom = [0] * len(ALGORITHMS)
    plt.figure(figsize=(8, 4.8))
    for status in statuses:
        vals = [counts[a][status] for a in ALGORITHMS]
        plt.bar(x, vals, bottom=bottom, label=status, color=COLORS.get(status, "#999"))
        bottom = [bottom[i] + vals[i] for i in range(len(vals))]
    plt.xticks(x, ALGORITHMS)
    apply_style("Iteration 1 - JJ status summary", "Case count")
    plt.legend()
    return save_fig(ITER1, "iter1_jj_status_summary.png")


# -------- iteration 2: reorganize around conclusions --------

def iter2_fsu_greedy_error(rows):
    data = [r for r in rows if r["group"] == "fsu_correctness" and r["algorithm"] == "Greedy"]
    data.sort(key=lambda r: str(r["dataset"]))
    labels = [short_label(r) for r in data]
    values = [float(r["error_value"] or 0) for r in data]
    plt.figure(figsize=(9, 4.6))
    bars = plt.bar(labels, values, color=COLORS["Greedy"])
    for bar, value in zip(bars, values):
        if value > 0:
            plt.text(bar.get_x() + bar.get_width() / 2, value, f"{value:.1f}%", ha="center", va="bottom", fontsize=8)
    apply_style("Iteration 2 - FSU greedy error focus", "Error (%)")
    return save_fig(ITER2, "iter2_fsu_greedy_error_focus.png")


def iter2_uu_scale_trend(rows):
    names = ["knapPI_1_100_1000_1", "knapPI_1_500_1000_1", "knapPI_1_1000_1000_1"]
    data = [r for r in rows if r["dataset"] in names and r["algorithm"] in ALGORITHMS]
    lookup = by_dataset_algorithm(data)
    labels = ["n=100", "n=500", "n=1000"]
    plt.figure(figsize=(8.5, 4.8))
    for alg in ALGORITHMS:
        values = [safe_runtime(lookup[(name, alg)]["runtime_ms_avg"]) for name in names]
        plt.plot(labels, values, marker="o", linewidth=2, label=alg, color=COLORS[alg])
    apply_style("Iteration 2 - UU scale trend", "Runtime (ms, log)", log_y=True)
    plt.legend()
    return save_fig(ITER2, "iter2_uu_scale_trend.png")


def iter2_greedy_error_by_source(rows):
    sources = ["FSU", "UU", "JJ"]
    avg_vals, max_vals = [], []
    for source in sources:
        vals = [float(r["error_value"] or 0) for r in rows if r["source"] == source and r["algorithm"] == "Greedy" and r["error_value"] is not None]
        avg_vals.append(average(vals))
        max_vals.append(max(vals) if vals else 0)
    x = list(range(len(sources)))
    width = 0.35
    plt.figure(figsize=(8, 4.8))
    plt.bar([v - width / 2 for v in x], avg_vals, width=width, label="Average", color="#4C78A8")
    plt.bar([v + width / 2 for v in x], max_vals, width=width, label="Max", color="#E45756")
    plt.xticks(x, sources)
    apply_style("Iteration 2 - Greedy error by source", "Error (%)")
    plt.legend()
    return save_fig(ITER2, "iter2_greedy_error_by_source.png")


def iter2_backtracking_timeout(rows):
    data = [r for r in rows if r["algorithm"] == "Backtracking" and r["source"] in {"UU", "JJ"}]
    data.sort(key=lambda r: (str(r["source"]), str(r["group"]), int(r["n"] or 0), int(r["capacity"] or 0)))
    labels = [short_label(r) for r in data]
    values = [max(int(r["nodes"] or 0), 1) for r in data]
    colors = [COLORS["TIMEOUT"] if r["status"] == "TIMEOUT" else COLORS["Backtracking"] for r in data]
    plt.figure(figsize=(14, 5.2))
    plt.bar(range(len(labels)), values, color=colors)
    plt.xticks(range(len(labels)), labels, rotation=60, ha="right", fontsize=8)
    apply_style("Iteration 2 - Backtracking timeout highlighted", "Nodes (log)", log_y=True)
    plt.legend([plt.Rectangle((0, 0), 1, 1, color=COLORS["Backtracking"]), plt.Rectangle((0, 0), 1, 1, color=COLORS["TIMEOUT"])], ["OK", "TIMEOUT"])
    return save_fig(ITER2, "iter2_backtracking_timeout_nodes.png")


def jj_matrix(rows, directory: Path, filename: str, title: str) -> Path:
    data = [r for r in rows if r["source"] == "JJ"]
    statuses = ["OK", "TIMEOUT", "SKIPPED_BY_CAPACITY"]
    matrix = [[status_counts(data, alg)[s] for s in statuses] for alg in ALGORITHMS]
    plt.figure(figsize=(7, 4.8))
    image = plt.imshow(matrix, cmap="YlOrRd")
    plt.xticks(range(len(statuses)), ["OK", "TIMEOUT", "SKIP"])
    plt.yticks(range(len(ALGORITHMS)), ALGORITHMS)
    for i, row in enumerate(matrix):
        for j, value in enumerate(row):
            plt.text(j, i, str(value), ha="center", va="center", fontsize=12, weight="bold")
    plt.colorbar(image, fraction=0.046, pad=0.04)
    plt.title(title, fontsize=13, weight="bold")
    return save_fig(directory, filename)


# -------- final report-ready figures --------

def final_fsu(rows, s):
    data = [r for r in rows if r["group"] == "fsu_correctness" and r["algorithm"] == "Greedy"]
    data.sort(key=lambda r: str(r["dataset"]))
    labels = [short_label(r) for r in data]
    values = [float(r["error_value"] or 0) for r in data]
    plt.figure(figsize=(9, 4.6))
    bars = plt.bar(labels, values, color=COLORS["Greedy"])
    for bar, value in zip(bars, values):
        plt.text(bar.get_x() + bar.get_width() / 2, value, f"{value:.1f}%", ha="center", va="bottom", fontsize=8)
    plt.text(0.02, 0.95, f"DP exact: {s['fsu_dp_exact']}/7\nBacktracking exact: {s['fsu_bt_exact']}/7", transform=plt.gca().transAxes, va="top", bbox=dict(facecolor="white", alpha=0.85, edgecolor="#CCCCCC"))
    apply_style("Fig.1 FSU correctness and greedy error", "Greedy error (%)")
    return save_fig(FINAL, "fig1_fsu_correctness_error.png")


def final_uu_low(rows):
    data = [r for r in rows if r["group"] == "uu_low"]
    labels = unique_datasets(data)
    values = grouped_values(data, labels, ALGORITHMS, "runtime_ms_avg", runtime=True)
    x = list(range(len(labels)))
    width = 0.25
    plt.figure(figsize=(9.5, 4.8))
    for i, alg in enumerate(ALGORITHMS):
        plt.bar([v + (i - 1) * width for v in x], values[alg], width=width, label=alg, color=COLORS[alg])
    plt.xticks(x, [short_label(v) for v in labels])
    apply_style("Fig.2 UU low-dimensional runtime", "Runtime (ms, log)", log_y=True)
    plt.legend()
    return save_fig(FINAL, "fig2_uu_low_runtime.png")


def final_uu_large(rows):
    names = ["knapPI_1_100_1000_1", "knapPI_1_500_1000_1", "knapPI_1_1000_1000_1"]
    data = [r for r in rows if r["dataset"] in names and r["algorithm"] in {"DP", "Greedy"}]
    lookup = by_dataset_algorithm(data)
    labels = ["n=100", "n=500", "n=1000"]
    plt.figure(figsize=(8.5, 4.8))
    for alg in ["DP", "Greedy"]:
        values = [safe_runtime(lookup[(name, alg)]["runtime_ms_avg"]) for name in names]
        plt.plot(labels, values, marker="o", linewidth=2.4, label=alg, color=COLORS[alg])
    apply_style("Fig.3 UU large-scale runtime trend", "Runtime (ms, log)", log_y=True)
    plt.legend()
    return save_fig(FINAL, "fig3_uu_large_runtime.png")


def final_greedy_error(rows):
    data = [r for r in rows if r["algorithm"] == "Greedy" and r["error_value"] is not None]
    data.sort(key=lambda r: (str(r["source"]), str(r["group"]), int(r["n"] or 0), int(r["capacity"] or 0)))
    labels = [short_label(r) for r in data]
    values = [float(r["error_value"] or 0) for r in data]
    colors = ["#4C78A8" if r["source"] == "FSU" else "#54A24B" if r["source"] == "UU" else "#F58518" for r in data]
    plt.figure(figsize=(13, 5.2))
    plt.bar(range(len(labels)), values, color=colors)
    plt.xticks(range(len(labels)), labels, rotation=55, ha="right", fontsize=8)
    apply_style("Fig.4 Greedy relative error by dataset", "Error (%)")
    plt.legend([plt.Rectangle((0,0),1,1,color="#4C78A8"), plt.Rectangle((0,0),1,1,color="#54A24B"), plt.Rectangle((0,0),1,1,color="#F58518")], ["FSU", "UU", "JJ"])
    return save_fig(FINAL, "fig4_greedy_error.png")


def final_backtracking(rows):
    data = [r for r in rows if r["algorithm"] == "Backtracking" and (r["source"] == "JJ" or r["group"] == "uu_low")]
    data.sort(key=lambda r: (str(r["source"]), str(r["group"]), int(r["n"] or 0), int(r["capacity"] or 0)))
    labels = [short_label(r) for r in data]
    values = [max(int(r["nodes"] or 0), 1) for r in data]
    colors = [COLORS["TIMEOUT"] if r["status"] == "TIMEOUT" else COLORS["Backtracking"] for r in data]
    plt.figure(figsize=(12, 5.2))
    plt.bar(range(len(labels)), values, color=colors)
    plt.xticks(range(len(labels)), labels, rotation=55, ha="right", fontsize=8)
    apply_style("Fig.5 Backtracking search nodes", "Nodes (log)", log_y=True)
    plt.legend([plt.Rectangle((0,0),1,1,color=COLORS["Backtracking"]), plt.Rectangle((0,0),1,1,color=COLORS["TIMEOUT"])], ["OK", "TIMEOUT"])
    return save_fig(FINAL, "fig5_backtracking_search.png")


# -------- documents --------

def write_project_status(s):
    text = f"""# 项目完成情况检查

## 1. 当前完成状态

当前项目已经完成从数据整理、算法实现、实验运行到初步结果绘图的主要链路。

| 模块 | 当前状态 | 说明 |
|---|---|---|
| 数据集 | 已完成 | `data/datasets.csv` 中包含 FSU、UU、JJ 三类数据源。 |
| 数据读取 | 已完成 | 已实现 FSU、UU、JJ reader。 |
| 贪心算法 | 已完成 | 已实现价值密度贪心算法。 |
| 动态规划 | 已完成 | 已实现一维滚动数组 DP 和容量阈值规则。 |
| 回溯法 | 已完成 | 已实现上界剪枝和超时控制。 |
| 实验运行器 | 已完成 | 主程序支持 `--check-data`、`--run-all`、`--run-group`、`--run-one`。 |
| 结果输出 | 已完成 | 已生成 `results.csv`、`timeout_cases.csv`、`run_log.txt`。 |
| 图表绘制 | 本次优化 | 已升级为三轮迭代式图表生成。 |
| 报告/PPT | 本次生成初稿 | 已生成实验报告初稿和 PPT 方案。 |

## 2. 当前实验结果规模

- 数据集实例数：{s['datasets']} 个。
- 结果记录数：{s['records']} 条。
- FSU：{s['fsu']} 个实例。
- UU：{s['uu']} 个实例。
- JJ：{s['jj']} 个实例。

## 3. 后续完善建议

1. 将 `实验报告初稿.md` 整理为正式 PDF。
2. 根据 `PPT方案.md` 制作正式 `.pptx`。
3. 录制 5-10 分钟讲解视频。
4. 更新 README 中的运行方式、数据说明和结果说明。
5. 最终提交前再次运行测试和绘图脚本。
"""
    path = REPORT_DIR / "项目完成情况检查.md"
    path.write_text(text, encoding="utf-8")
    return path


def write_optimization_notes(s):
    text = f"""# 图表效果不佳原因与三轮优化说明

## 1. 原图效果不佳的主要原因

### 1.1 运行时间存在 0 值，log 坐标显示失真

原始结果中，小规模实例的部分算法运行时间为 `0.0000 ms`。这是因为实例规模较小，运行时间低于计时精度。原脚本直接使用对数坐标绘制时间图，导致 0 值无法正常显示，图中差异被压缩或隐藏。

### 1.2 数据集名称过长，横轴标签拥挤

JJ 数据集名称包含 n、capacity、g、f、eps、seed 等多个参数，直接显示完整名称会严重挤压图表主体。报告和 PPT 中缩小后几乎无法阅读。

### 1.3 多数据源混在同一张图中，结论不聚焦

FSU、UU、JJ 的实验角色不同：FSU 用于正确性验证，UU 用于标准规模性能比较，JJ 用于困难实例边界观察。原图将多类数据直接混合，容易造成信息堆叠。

### 1.4 部分图表没有直接回答研究问题

例如 FSU 正确性图直接比较三算法 value，但 DP 和回溯法都等于最优值，柱状图高度接近，不能突出“是否命中最优”和“贪心误差”两个核心结论。

### 1.5 回溯图没有突出 TIMEOUT 现象

回溯法的重要结论是：小规模可行，困难实例上可能超时。原图只展示 nodes/pruned，未突出状态差异，因此对算法边界的表达不够强。

## 2. 第一轮迭代：修复可读性

第一轮保留原图思路，但进行基础修复：短标签、运行时间显示下限、统一颜色、调整图像尺寸和标签角度。输出目录：`reports/figures/iteration1/`。

## 3. 第二轮迭代：按实验结论重组

第二轮让每张图回答一个具体问题：FSU 图突出贪心误差，UU 图突出规模增长，回溯图突出 TIMEOUT，JJ 图使用状态矩阵展示算法边界。输出目录：`reports/figures/iteration2/`。

## 4. 第三轮迭代：报告/PPT 最终版

第三轮保留 6 张最终图表，统一命名为 `fig1` 到 `fig6`，可直接放入报告和 PPT。输出目录：`reports/figures/final/`。

## 5. 优化后的核心结论

- FSU 中 DP 命中 {s['fsu_dp_exact']}/7，回溯法命中 {s['fsu_bt_exact']}/7。
- FSU 中贪心平均误差为 {fmt(s['fsu_g_avg'])}%，最大误差为 {fmt(s['fsu_g_max'])}%。
- UU 低维组贪心平均误差为 {fmt(s['uu_g_avg'])}%。
- JJ 中 DP 有 {s['jj_dp_skip']} 个大容量实例被跳过。
- JJ 中回溯法 TIMEOUT={s['jj_bt_timeout']}，体现指数搜索边界。
"""
    path = REPORT_DIR / "图表优化说明.md"
    path.write_text(text, encoding="utf-8")
    return path


def write_figure_guide(final_paths):
    rows = [
        ("fig1_fsu_correctness_error.png", "报告 4.1 正确性验证", "说明 DP 和回溯法命中最优值，同时展示贪心误差。"),
        ("fig2_uu_low_runtime.png", "报告 4.2 UU 低维性能分析", "展示小规模标准实例中三种算法运行时间对比。"),
        ("fig3_uu_large_runtime.png", "报告 4.2 UU 大规模性能分析", "展示 n=100、500、1000 时 DP 与 Greedy 的时间趋势。"),
        ("fig4_greedy_error.png", "报告 4.3 贪心误差分析", "展示贪心在不同数据源上的相对误差。"),
        ("fig5_backtracking_search.png", "报告 4.4 回溯搜索分析", "展示回溯搜索节点数，并用红色突出 TIMEOUT。"),
        ("fig6_jj_boundary.png", "报告 4.5 困难实例边界分析", "展示 JJ 中三种算法的状态分布。"),
    ]
    lines = ["# 图表说明", "", "| 图表 | 建议位置 | 说明 |", "|---|---|---|"]
    for name, section, desc in rows:
        lines.append(f"| `reports/figures/final/{name}` | {section} | {desc} |")
    lines += ["", "## 最终图表清单", ""]
    lines += [f"- `{p.relative_to(ROOT).as_posix()}`" for p in final_paths]
    path = REPORT_DIR / "图表说明.md"
    path.write_text("\n".join(lines), encoding="utf-8")
    return path


def write_analysis(s, final_paths):
    lines = [
        "# 实验结果分析草稿", "", "## 1. 输出文件", "",
        f"- 主结果文件：`experiments/results.csv`，共 {s['records']} 条记录。",
        "- 超时记录文件：`experiments/timeout_cases.csv`。",
        "- 图表输出目录：`reports/figures/`。", "", "## 2. 关键现象", "",
        f"- FSU 正确性组中，DP 精确命中 {s['fsu_dp_exact']}/7，回溯法精确命中 {s['fsu_bt_exact']}/7。",
        f"- FSU 正确性组中，贪心平均误差为 {fmt(s['fsu_g_avg'])}%，最大误差为 {fmt(s['fsu_g_max'])}%。",
        f"- UU 低维组中，贪心平均误差为 {fmt(s['uu_g_avg'])}%，最大误差为 {fmt(s['uu_g_max'])}%。",
        f"- UU 大规模组共 {s['uu_large_count']} 个数据集，重点比较 DP 与 Greedy 的运行时间。",
        f"- JJ 数据中，DP 状态统计：OK={s['jj_dp_ok']}，SKIPPED_BY_CAPACITY={s['jj_dp_skip']}。",
        f"- JJ 数据中，回溯法 TIMEOUT={s['jj_bt_timeout']}，体现指数搜索在困难实例上的边界。",
        "", "## 3. 最终图表清单", "",
    ]
    lines += [f"- `{p.relative_to(ROOT).as_posix()}`" for p in final_paths]
    lines += ["", "## 4. 可写入报告的结论", "",
        "1. DP 和回溯法在小规模 FSU、UU 低维实例中能够达到已知最优值，可作为正确性基准。",
        "2. 贪心算法始终运行成功且不超容量，但在部分实例上存在非零误差。",
        "3. DP 的主要瓶颈来自容量维度，大容量 JJ 实例被标记为 `SKIPPED_BY_CAPACITY`。",
        "4. 回溯法在部分 JJ 困难实例上出现 `TIMEOUT`，体现指数复杂度边界。",
        "5. 贪心算法适合大规模或实时近似场景，DP 适合容量可控且要求最优的场景，回溯法适合小规模精确搜索和剪枝效果展示。", ""]
    path = REPORT_DIR / "结果分析草稿.md"
    path.write_text("\n".join(lines), encoding="utf-8")
    return path


def write_report_draft(s):
    text = f"""# 多数据源下0/1背包问题三种求解算法的性能比较

## 摘要

本实验针对经典 0/1 背包问题，选取 FSU、UU、JJ 三类数据源，分别实现动态规划、回溯法和贪心算法，并从正确性、运行时间、空间开销、近似误差和困难实例适用边界等角度进行对比分析。实验结果表明，动态规划和回溯法在小规模实例上能够稳定获得最优解，其中 FSU 正确性组中二者均命中全部 7 个参考最优值；贪心算法运行速度最快，但在部分实例上存在相对误差；动态规划受容量维度影响明显，在大容量 JJ 实例上需要主动跳过；回溯法虽然可通过上界剪枝减少搜索，但在部分困难实例上仍会超时。综合来看，三种算法分别适用于容量可控的精确求解、小规模精确搜索和大规模快速近似场景。

## 1. 实验目的与问题定义

0/1 背包问题是组合优化中的经典问题。给定 n 个物品，每个物品具有价值 `v_i` 和重量 `w_i`，背包容量为 `C`，要求选择若干物品放入背包，使得总重量不超过容量，同时总价值最大。

```text
max Σ v_i x_i
s.t. Σ w_i x_i <= C
x_i ∈ {{0, 1}}
```

本实验目标不是单独实现某一种算法，而是在同一批数据上比较动态规划、回溯法和贪心算法。通过对比三者的结果质量、运行时间、内存估算、超时情况和适用边界，总结不同算法在解决同一问题时的优缺点。

## 2. 算法设计

### 2.1 动态规划算法

动态规划算法使用一维滚动数组 `dp[c]`，表示容量不超过 `c` 时能够获得的最大价值。对每个物品，从大到小枚举容量并更新状态：

```text
dp[c] = max(dp[c], dp[c - weight] + value)
```

容量必须倒序遍历，因为每个物品只能选择一次。该算法时间复杂度为 `O(nC)`，空间复杂度为 `O(C)`。由于 JJ 中存在超大容量实例，程序设置 `DP_CAPACITY_LIMIT = 10,000,000`，超过阈值时记录为 `SKIPPED_BY_CAPACITY`。

### 2.2 回溯法

回溯法通过深度优先搜索枚举物品选或不选的决策。为了减少搜索量，程序先按价值密度排序，并使用分数背包思想计算乐观上界。如果当前价值加剩余上界仍不超过当前最优值，则剪枝。回溯法记录搜索节点数 `nodes`、剪枝次数 `pruned` 和剪枝率 `prune_rate`。该算法最坏时间复杂度为 `O(2^n)`，空间复杂度为 `O(n)`，因此程序设置 10 秒超时限制。

### 2.3 贪心算法

贪心算法按照价值密度 `value / weight` 从高到低排序，依次尝试放入物品，能放则放，不能放则跳过。其时间复杂度主要来自排序，为 `O(n log n)`，空间复杂度为 `O(n)`。贪心算法速度快，但对于 0/1 背包不能保证全局最优，因此实验中重点统计其相对误差。

### 2.4 复杂度对比

| 算法 | 时间复杂度 | 空间复杂度 | 是否保证最优 | 主要瓶颈 |
|---|---:|---:|---|---|
| 动态规划 | O(nC) | O(C) | 是 | 容量 C 较大时内存和时间压力明显 |
| 回溯法 | O(2^n) | O(n) | 正常完成时是 | 物品数量增加时搜索树膨胀 |
| 贪心算法 | O(n log n) | O(n) | 否 | 可能产生近似误差 |

## 3. 实验数据与程序实现

实验使用三类数据源。FSU 主要用于正确性验证，UU 用于标准规模性能对比，JJ 用于困难实例下算法边界分析。当前 `datasets.csv` 中共包含 {s['datasets']} 个实例，其中 FSU {s['fsu']} 个、UU {s['uu']} 个、JJ {s['jj']} 个。三类数据源格式不同，程序通过 reader 模块统一转换为 `KnapsackInstance`，再交给三种算法求解。

程序结构采用分层设计：`core/` 存放核心类型和常量，`readers/` 负责数据读取，`algorithms/` 实现三种算法，`runner/` 负责批量运行和结果输出，`utils/` 提供计时、CSV 和字符串工具。实验结果统一写入 `experiments/results.csv`。

## 4. 实验结果与分析

### 4.1 FSU 正确性验证

FSU 正确性组中，DP 精确命中 {s['fsu_dp_exact']}/7，回溯法精确命中 {s['fsu_bt_exact']}/7，说明数据读取、状态转移和回溯搜索逻辑能够得到参考最优值。贪心算法在 FSU 上平均误差为 {fmt(s['fsu_g_avg'])}%，最大误差为 {fmt(s['fsu_g_max'])}%，说明虽然贪心速度快，但其选择策略并不总能得到全局最优。该部分可引用 `reports/figures/final/fig1_fsu_correctness_error.png`。

### 4.2 UU 标准规模性能分析

UU 低维组中，贪心算法平均误差为 {fmt(s['uu_g_avg'])}%，最大误差为 {fmt(s['uu_g_max'])}%。这说明在部分标准实例中，价值密度贪心能够得到接近最优的结果。UU 大规模组中，DP 和 Greedy 均能完成运行，但 DP 的运行时间随规模和容量增长而增加，Greedy 始终保持较低开销。该部分可引用 `fig2_uu_low_runtime.png` 和 `fig3_uu_large_runtime.png`。

### 4.3 贪心误差分析

贪心算法在全部数据源上都能快速给出可行解，但误差分布并不相同。FSU 中最大误差达到 {fmt(s['fsu_g_max'])}%，而 JJ 中平均误差为 {fmt(s['jj_g_avg'])}%，最大误差为 {fmt(s['jj_g_max'])}%。这表明贪心算法的效果与数据分布有关，不能简单认为速度快就一定足够准确。该部分可引用 `fig4_greedy_error.png`。

### 4.4 回溯法剪枝与超时分析

回溯法在小规模实例中能够得到最优解，并通过上界剪枝减少搜索节点。但在 JJ 困难实例中，回溯法出现 {s['jj_bt_timeout']} 个 TIMEOUT，说明即使加入剪枝，指数级搜索在困难实例上仍可能无法在限定时间内完成。该部分可引用 `fig5_backtracking_search.png`。

### 4.5 JJ 困难实例边界分析

JJ 数据源用于观察困难实例下的算法边界。实验中 DP 在 {s['jj_dp_ok']} 个 JJ 实例上正常完成，在 {s['jj_dp_skip']} 个大容量实例上因超过容量阈值被标记为 `SKIPPED_BY_CAPACITY`。贪心算法在 JJ 实例上均能运行完成，体现出良好的可扩展性。该部分可引用 `fig6_jj_boundary.png`。

## 5. 总结

本实验通过 FSU、UU、JJ 三类数据源比较了动态规划、回溯法和贪心算法在 0/1 背包问题上的表现。动态规划适合容量可控且要求最优解的场景；回溯法适合小规模精确搜索和剪枝效果展示，但在困难实例上可能超时；贪心算法速度最快，适合大规模近似求解，但需要接受一定误差。整体来看，三种算法体现了“最优性、时间开销、空间开销、可扩展性”之间的典型权衡。

实验不足包括：内存使用采用估算方式，未统计真实峰值内存；回溯法只设置固定超时时间，未进一步比较不同剪枝策略；图表和报告仍可根据正式排版要求继续精简。后续可增加更多数据规模、加入分支限界或近似算法，并对不同算法的适用边界进行更细致分析。

## 参考资料

1. Cormen T. H. 等，《算法导论》，动态规划与贪心算法章节。
2. Kellerer H., Pferschy U., Pisinger D. Knapsack Problems. Springer, 2004.
3. Jooken J., Leyman P., De Causmaecker P. A new class of hard problem instances for the 0-1 knapsack problem. European Journal of Operational Research, 2022.
4. UU 0/1 Knapsack benchmark instances.
"""
    path = REPORT_DIR / "实验报告初稿.md"
    path.write_text(text, encoding="utf-8")
    return path


def write_ppt_plan(s):
    slides = [
        ("1", "标题页", "多数据源下0/1背包问题三种求解算法的性能比较", "题目、姓名、学号、课程名称"),
        ("2", "问题定义", "0/1 背包问题", "公式与约束条件"),
        ("3", "实验设计", "三算法 × 三数据源", "FSU/UU/JJ 分层表"),
        ("4", "程序结构", "统一读取、统一求解、统一输出", "程序数据流示意"),
        ("5", "算法思想", "DP、回溯法、贪心算法", "复杂度对比表"),
        ("6", "FSU 正确性验证", f"DP 命中 {s['fsu_dp_exact']}/7，回溯命中 {s['fsu_bt_exact']}/7", "fig1_fsu_correctness_error.png"),
        ("7", "UU 性能对比", "标准规模下的时间表现", "fig2_uu_low_runtime.png / fig3_uu_large_runtime.png"),
        ("8", "贪心误差分析", f"FSU 最大误差 {fmt(s['fsu_g_max'])}%", "fig4_greedy_error.png"),
        ("9", "回溯与 JJ 边界", f"DP 跳过 {s['jj_dp_skip']} 个，回溯超时 {s['jj_bt_timeout']} 个", "fig5_backtracking_search.png / fig6_jj_boundary.png"),
        ("10", "总结", "三种算法的适用场景", "总结表"),
    ]
    lines = ["# PPT 方案", "", "建议 PPT 控制在 10 页左右，讲解时间控制在 5-10 分钟。", "", "| 页码 | 页面标题 | 主要内容 | 建议图表/素材 |", "|---|---|---|---|"]
    for no, title, content, fig in slides:
        lines.append(f"| {no} | {title} | {content} | {fig} |")
    lines += ["", "## 讲解节奏建议", "", "| 时间 | 内容 |", "|---|---|", "| 0:00-0:40 | 标题和实验目标 |", "| 0:40-1:30 | 0/1 背包问题定义 |", "| 1:30-2:40 | 三种算法思想和复杂度 |", "| 2:40-3:30 | 数据集和程序结构 |", "| 3:30-6:30 | 图表结果分析 |", "| 6:30-7:40 | 算法适用性总结 |", "| 7:40-8:00 | 不足与改进方向 |", "", "## 制作建议", "", "1. 每页不要放太多文字，优先放图和结论句。", "2. 图表使用 `reports/figures/final/` 中的最终版。", "3. 对超时和跳过不要解释成程序失败，而要解释为算法适用边界。", "4. 视频讲解时不需要逐行讲代码，重点讲实验设计、结果和结论。", ""]
    path = REPORT_DIR / "PPT方案.md"
    path.write_text("\n".join(lines), encoding="utf-8")
    return path


def generate_all():
    rows = read_results()
    s = summarize(rows)
    iter1 = [iter1_fsu(rows), iter1_uu_low_time(rows), iter1_greedy_error(rows), iter1_uu_large_time(rows), iter1_backtracking(rows), iter1_jj_status(rows)]
    iter2 = [iter2_fsu_greedy_error(rows), iter2_uu_scale_trend(rows), iter2_greedy_error_by_source(rows), iter2_backtracking_timeout(rows), jj_matrix(rows, ITER2, "iter2_jj_boundary_matrix.png", "Iteration 2 - JJ algorithm boundary")]
    final = [final_fsu(rows, s), final_uu_low(rows), final_uu_large(rows), final_greedy_error(rows), final_backtracking(rows), jj_matrix(rows, FINAL, "fig6_jj_boundary.png", "Fig.6 JJ hard-instance boundary")]
    docs = [write_project_status(s), write_optimization_notes(s), write_figure_guide(final), write_analysis(s, final), write_report_draft(s), write_ppt_plan(s)]
    print(f"rows={len(rows)}")
    print(f"iteration1_figures={len(iter1)}")
    print(f"iteration2_figures={len(iter2)}")
    print(f"final_figures={len(final)}")
    print(f"documents={len(docs)}")
    for p in iter1 + iter2 + final + docs:
        print(p.relative_to(ROOT).as_posix())


if __name__ == "__main__":
    generate_all()
