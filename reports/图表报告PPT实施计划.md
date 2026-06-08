# 图表优化、实验报告初稿与 PPT 方案实施计划

> 日期：2026-06-08  
> 对应任务：检查项目完成情况，优化数据图绘制，分析图表效果不佳原因，至少三轮迭代，并生成初版实验报告和 PPT 方案。

---

## 1. 目标

在已有 `experiments/results.csv` 的基础上，完成图表表达质量优化，并形成报告和 PPT 的初版材料。最终产出包括：

1. 三轮图表迭代目录：
   - `reports/figures/iteration1/`
   - `reports/figures/iteration2/`
   - `reports/figures/final/`
2. 图表问题分析与优化说明：
   - `reports/图表优化说明.md`
3. 最终图表使用说明：
   - `reports/图表说明.md`
4. 初版实验报告：
   - `reports/实验报告初稿.md`
5. PPT 方案：
   - `reports/PPT方案.md`

---

## 2. 文件职责

### 修改文件

- `experiments/plot_results.py`  
  重写为三轮迭代式图表生成脚本，负责读取 `results.csv`、生成三轮图表、输出图表说明、结果分析草稿、实验报告初稿和 PPT 方案。

### 新增/更新文件

- `reports/figures/iteration1/*.png`  
  第一轮：修复可读性问题，保留接近原始图的表达方式。

- `reports/figures/iteration2/*.png`  
  第二轮：按实验结论重新组织图表。

- `reports/figures/final/*.png`  
  第三轮：报告和 PPT 可直接使用的最终图表。

- `reports/图表优化说明.md`  
  分析原图效果不佳原因，并记录三轮迭代改进。

- `reports/图表说明.md`  
  说明每张最终图适合放在报告哪一节、表达什么结论。

- `reports/实验报告初稿.md`  
  基于当前结果数据生成完整初版报告草稿。

- `reports/PPT方案.md`  
  生成 10 页左右 PPT 页码、标题、图表和讲解要点。

---

## 3. 执行步骤

1. 读取并分析当前 `results.csv`、已有图表和 `plot_results.py`。
2. 重写 `plot_results.py`：
   - 增加短标签函数；
   - 增加安全运行时间函数，解决 log 坐标中 0 值问题；
   - 增加统一样式和颜色；
   - 生成三轮图表；
   - 生成 Markdown 文档。
3. 运行：
   - `python experiments/plot_results.py`
4. 验证：
   - `reports/figures/iteration1/`、`iteration2/`、`final/` 均存在图表；
   - `reports/图表优化说明.md`、`图表说明.md`、`实验报告初稿.md`、`PPT方案.md` 均生成；
   - 终端输出包含生成文件数量；
   - `results.csv` 不被修改。
5. 自动 commit，提交信息使用中文。
6. 停下来汇报，不自动 push。

---

## 4. 验证标准

本任务完成时应满足：

- 至少三轮图表迭代真实生成；
- final 目录中包含可直接用于报告/PPT 的图表；
- 文档中明确解释原图效果不佳的原因；
- 实验报告初稿包含摘要、实验目的、算法设计、数据与实现、结果分析、总结；
- PPT 方案包含每页标题、建议图表和讲解要点；
- 不修改 `data/datasets.csv` 和 `experiments/results.csv`。