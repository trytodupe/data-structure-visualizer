# VisualizerWindow 重构方案 (Refactoring Plan)

## 当前问题 (Current Issues)
- **文件大小**: VisualizerWindow.h 有 510 行代码
- **职责过多**: 一个类负责了数组和栈的可视化、操作控制、可视化控制等多个职责
- **可维护性**: 添加新数据结构时需要修改大文件
- **代码重复**: 数组和栈的渲染逻辑有相似模式

## 重构方案 (Refactoring Proposal)

### 方案一：按数据结构类型分离 (Split by Data Structure Type)

#### 优点 (Advantages)
- 职责清晰，每个类负责一种数据结构
- 添加新数据结构时互不影响
- 每个文件大小适中 (150-200行)

#### 缺点 (Disadvantages)
- 可能有一些代码重复
- 需要一个协调类来管理

#### 文件结构 (File Structure)
```
src/visual/
├── VisualizerWindow.h           (~100 lines) - 主窗口，协调各个可视化器
├── ArrayVisualizer.h            (~200 lines) - 数组可视化和操作
├── StackVisualizer.h            (~200 lines) - 栈可视化和操作
└── VisualizationController.h    (~100 lines) - 可视化控制逻辑 (Start/Step)
```

#### ArrayVisualizer.h 职责
- 渲染数组的视觉表示（彩色方块）
- 显示索引标签和箭头指示器
- 处理数组操作下拉菜单和输入
- 检测修改的元素并高亮显示
- 管理数组相关的状态（insertIndex, deleteIndex等）

#### StackVisualizer.h 职责
- 渲染栈的视觉表示（垂直堆叠方块）
- 显示 TOP 标签
- 处理栈操作下拉菜单和输入
- 检测修改的元素并高亮显示
- 管理栈相关的状态（pushValue等）

#### VisualizationController.h 职责
- 管理操作暂存 (stagedOperation, stagedDataStructure)
- 控制可视化步骤 (currentAtomicStep, isVisualizing)
- 渲染 Start/Step/Cancel 按钮
- 执行原子操作步进
- 完成后添加到操作管理器

#### VisualizerWindow.h 职责
- 管理主窗口和标签页
- 协调 ArrayVisualizer 和 StackVisualizer
- 持有 VisualizationController
- 处理窗口的打开/关闭状态

---

### 方案二：按功能层分离 (Split by Functional Layer)

#### 优点 (Advantages)
- 按功能划分更符合软件架构原则
- 渲染逻辑和控制逻辑完全分离
- 更容易测试和维护

#### 缺点 (Disadvantages)
- 文件数量较多
- 类之间的依赖关系更复杂

#### 文件结构 (File Structure)
```
src/visual/
├── VisualizerWindow.h           (~80 lines)  - 主窗口框架
├── DataStructureRenderer.h      (抽象基类)   - 渲染器接口
├── ArrayRenderer.h              (~150 lines) - 数组渲染器
├── StackRenderer.h              (~150 lines) - 栈渲染器
├── OperationPanel.h             (~100 lines) - 操作选择面板（下拉菜单和输入）
└── VisualizationController.h    (~100 lines) - 可视化步进控制
```

#### DataStructureRenderer.h (抽象接口)
```cpp
class DataStructureRenderer {
public:
    virtual void render(DataStructure& ds, const std::set<size_t>& highlightIndices) = 0;
    virtual void renderOperationIndicators() = 0;
};
```

#### ArrayRenderer.h 职责
- 纯渲染：绘制数组方块、索引、箭头
- 接收高亮索引集合，应用颜色
- 不包含业务逻辑

#### OperationPanel.h 职责
- 渲染操作下拉菜单
- 渲染输入字段
- 渲染执行按钮
- 返回用户选择的操作

---

### 方案三：混合方案 (Hybrid Approach) - **推荐**

#### 优点 (Advantages)
- 平衡了简洁性和可维护性
- 文件数量适中（4个文件）
- 职责清晰但不过度拆分

#### 缺点 (Disadvantages)
- 仍有少量代码重复

#### 文件结构 (File Structure)
```
src/visual/
├── VisualizerWindow.h              (~100 lines) - 主窗口和协调
├── ArrayVisualizerPanel.h          (~200 lines) - 数组完整面板
├── StackVisualizerPanel.h          (~200 lines) - 栈完整面板
└── VisualizationController.h       (~100 lines) - 步进控制器
```

#### 详细职责分配

**VisualizerWindow.h** (主窗口)
- 管理窗口状态 (isOpen, windowScale)
- 渲染标签页框架
- 持有 VisualizationController 实例
- 调用各个面板的 render 方法
```cpp
class VisualizerWindow {
    ArrayVisualizerPanel arrayPanel;
    StackVisualizerPanel stackPanel;
    VisualizationController vizController;

    void render() {
        // Tab bar
        if (BeginTabItem("Array")) {
            vizController.renderControls();
            arrayPanel.render(arrayDS, vizController);
        }
        if (BeginTabItem("Stack")) {
            vizController.renderControls();
            stackPanel.render(stackDS, vizController);
        }
    }
};
```

**ArrayVisualizerPanel.h** (数组面板)
- 数组可视化渲染（方块、索引、箭头）
- 数组操作选择（下拉菜单）
- 输入字段（insertIndex, insertValue, deleteIndex）
- 检测当前原子操作修改的索引
- 调用 vizController 暂存操作
```cpp
class ArrayVisualizerPanel {
    int insertIndex, insertValue, deleteIndex;
    int selectedOperation;

    void render(ArrayStructure& ds, VisualizationController& controller);
    void renderVisual(ArrayStructure& ds, const std::set<size_t>& highlightIndices);
    void renderOperationControls(ArrayStructure& ds, VisualizationController& controller);
    std::set<size_t> getModifiedIndices(VisualizationController& controller);
};
```

**StackVisualizerPanel.h** (栈面板)
- 栈可视化渲染（垂直方块、TOP标签）
- 栈操作选择（下拉菜单）
- 输入字段（pushValue）
- 检测当前原子操作（Push/Pop）
- 调用 vizController 暂存操作
```cpp
class StackVisualizerPanel {
    int pushValue;
    int selectedOperation;

    void render(StackStructure& ds, VisualizationController& controller);
    void renderVisual(StackStructure& ds, bool highlightTop);
    void renderOperationControls(StackStructure& ds, VisualizationController& controller);
    bool isTopBeingModified(VisualizationController& controller);
};
```

**VisualizationController.h** (可视化控制器)
- 管理暂存的操作 (stagedOperation, stagedDataStructure)
- 管理步进状态 (currentAtomicStep, isVisualizing)
- 渲染 Start/Step/Cancel 按钮
- 执行单个原子操作
- 完成后添加到 OperationManager
```cpp
class VisualizationController {
    std::unique_ptr<UserOperation> stagedOperation;
    DataStructure* stagedDataStructure;
    size_t currentAtomicStep;
    bool isVisualizing;

    void stageOperation(std::unique_ptr<UserOperation> op, DataStructure* ds);
    void renderControls(OperationManager& opManager);
    void executeNextStep();

    bool isInVisualization() const { return isVisualizing; }
    Operation* getCurrentAtomicOperation();
};
```

---

## 推荐方案：方案三 (Recommendation: Hybrid Approach)

### 理由 (Rationale)
1. **平衡性好**: 4个文件，每个100-200行，大小适中
2. **职责清晰**: 每个类有明确的职责
3. **易于扩展**: 添加新数据结构只需新建一个 Panel 类
4. **代码复用**: VisualizationController 被所有面板共享
5. **降低耦合**: 各个面板独立，互不影响
6. **保持一致性**: 与 HistoryWindow 的设计风格一致

### 重构步骤 (Refactoring Steps)
1. 创建 VisualizationController.h - 提取可视化控制逻辑
2. 创建 ArrayVisualizerPanel.h - 提取数组相关代码
3. 创建 StackVisualizerPanel.h - 提取栈相关代码
4. 重构 VisualizerWindow.h - 简化为协调者角色
5. 编译测试 - 确保功能不变
6. 代码审查 - 检查是否有改进空间

### 代码行数估算 (Estimated Line Count)
- VisualizerWindow.h: ~100 lines (减少 80%)
- ArrayVisualizerPanel.h: ~200 lines (新)
- StackVisualizerPanel.h: ~200 lines (新)
- VisualizationController.h: ~100 lines (新)
- **总计**: ~600 lines (比当前 510 lines 略多，但更易维护)

### 依赖关系图 (Dependency Graph)
```
VisualizerWindow
    ├── ArrayVisualizerPanel ──→ VisualizationController
    ├── StackVisualizerPanel ──→ VisualizationController
    └── VisualizationController ──→ OperationManager
```

### 未来扩展示例 (Future Extension Example)
添加队列数据结构：
```cpp
// 只需新建一个文件
src/visual/QueueVisualizerPanel.h

// 在 VisualizerWindow 中添加
QueueVisualizerPanel queuePanel;
if (BeginTabItem("Queue")) {
    vizController.renderControls();
    queuePanel.render(queueDS, vizController);
}
```

---

## 总结 (Summary)

**推荐使用方案三（混合方案）**，因为它：
- ✅ 将 510 行的单一文件拆分为 4 个更小的文件
- ✅ 每个类的职责单一且明确
- ✅ 易于添加新的数据结构类型
- ✅ VisualizationController 可被所有面板复用
- ✅ 符合单一职责原则和开闭原则
- ✅ 与现有代码风格（HistoryWindow）保持一致

这个方案在简洁性、可维护性和扩展性之间取得了良好的平衡。
