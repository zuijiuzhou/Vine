# C++20 协程 lambda 捕获的闭包生命周期约束（用户侧 UB）

> 适用项目：Vine（`src/base/async` 模块）
> 状态：已定案为用户侧 UB（closure lifetime），**并非编译器缺陷**（2026-08-30 更正）
> 日期：2026-08-30

## 一、结论摘要

一个**协程 lambda**（lambda 体内含 `co_await` / `co_return` / `co_yield`）被**立即调用**且持有
**捕获变量**时，捕获存放在**闭包对象**（closure）里，而闭包是完整表达式结束即析构的临时对象。
协程帧只保存指向闭包的指针（隐式对象参数 `this`），**并不把捕获拷进帧**。因此：

1. **立即调用 + 之后 resume**：`[&order, i]() -> Task { ... }()` 的闭包在完整表达式结束时析构，
   之后 `resume()` 读取捕获 → **use-after-free，未定义行为**。
2. 在**循环**里反复创建这种任务时，闭包临时对象复用同一栈槽，所有任务都读到最终值（`4 4 4 4`），
   gcc 甚至直接段错误——这些都是 UB 的**合法表现**，不是编译器 bug。

正确写法：把值作为**协程函数参数**（参数会被按值拷入协程状态），或使用**具名协程函数**，
或保证闭包对象活得比协程久。

## 二、受影响环境（全部为“正确实现下的 UB 表现”，非缺陷）

| 编译器 | 版本 | 表现 |
|--------|------|------|
| clang | `22.1.2 (Ubuntu 1ubuntu1)` | `4 4 4 4`（UB 的一种表现） |
| clang | `23.1.0 (++20260818… snapshot)` | `4 4 4 4`（`-O0`~`-O3` 全部复现） |
| g++ | `16.0.1 (experimental trunk r16-8246)` | 段错误（SIGSEGV） |
| g++ | **`15.2.0 (Ubuntu 16ubuntu1)` 稳定版** | **`-O0` 段错误；`-O1+` `4 4 4 4`** |

**关键证据**：稳定版 gcc 15.2.0 与 gcc 16 trunk、clang 22/23 **四个编译器全部一致**。
四个独立编译器产生完全同形的结果，只可能是它们都正确实现了同一标准语义、暴露出同一个用户侧 UB；
若真是编译器 codegen bug，不可能跨 4 个版本（含稳定版）完全相同。

## 三、最小复现（无第三方依赖）

```cpp
#include <coroutine>
#include <cstdio>
#include <vector>

struct Task {
    struct promise_type {
        Task get_return_object() {
            return { std::coroutine_handle<promise_type>::from_promise(*this) };
        }
        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_void() noexcept {}
        void unhandled_exception() noexcept {}
    };
    std::coroutine_handle<promise_type> h{};
};

int main() {
    std::vector<int> order;
    std::vector<Task> tasks;
    for (int i = 1; i <= 4; ++i) {
        tasks.push_back([&order, i]() -> Task {
            order.push_back(i);   // i 是按值捕获的循环变量
            co_return;
        }());
    }
    for (auto& t : tasks) { t.h.resume(); t.h.destroy(); t.h = nullptr; }
    for (int x : order) std::printf("%d ", x);
    std::printf("\n");
}
```

编译运行：

```bash
clang++ -std=c++20 -O0 repro.cpp -o repro && ./repro
g++     -std=c++20 -O0 repro.cpp -o repro && ./repro
```

- **期望输出**：`1 2 3 4`
- **clang 22.1.2**：`4 4 4 4`
- **g++ 16.0.1 trunk**：`Segmentation fault`

## 三·补、决定性证据：cppreference 权威示例（这就是标准语义）

cppreference 协程页面**一字不差**地给出了本模式，并明确标注为 use-after-free：

```cpp
void bad3()
{
    coroutine h = [i = 0]() -> coroutine
    {
        std::cout << i;
        co_return;
    }();                       // immediately invoked
    // lambda destroyed
    h.resume();                // uses (anonymous lambda type)::i after free
    h.destroy();
}

void good()
{
    coroutine h = [](int i) -> coroutine   // make i a coroutine parameter
    {
        std::cout << i;
        co_return;
    }(0);
    // lambda destroyed
    h.resume();                // OK: i 已作为按值参数拷入协程状态
    h.destroy();
}
```

机理：协程状态只按值拷贝**函数参数**；捕获是闭包类型的非静态数据成员，闭包作为隐式对象参数
`this`（指针）被存进帧，**闭包本体必须活得比协程久**。因此 `[x]{...}()`（捕获）在闭包析构后
resume 是 UB，而 `[](int x){...}(x)`（参数）是安全的。

这也统一解释了之前三个“对照实验”：case1 闭包每轮析构 → 错；case2 单任务栈槽未被复用 → **碰巧**正确
（仍是 UB）；case3 按引用捕获直接指向存活的原对象、不依赖闭包存活 → 正确。
没有任何实验能排除闭包生命周期，反而全部吻合。

## 四、对照组（三种安全写法）

### 1. 具名协程函数 —— 参数按值拷入帧，永远安全

```cpp
Task named(std::vector<int>& order, int i) { order.push_back(i); co_return; }

// 循环里：
for (int i = 1; i <= 4; ++i) {
    tasks.push_back(named(order, i));   // 输出 1 2 3 4
}
```

### 2. 普通 lambda（无 co_await/co_return）—— 不受影响

```cpp
std::thread t([&order, i] { order.push_back(i); });  // 非协程，无帧
```

### 3. 空捕获 `[]` 的协程 lambda —— 没有捕获成员可悬垂

```cpp
auto t = []() -> Task { co_return; }();   // 无捕获，安全
```

## 五、ASan 表现：stack-use-after-scope

```cpp
struct Big {
    std::mutex m;                 // 非平凡成员
    void work() {}
};

// 测试里：
Big big;
auto t = [&big]() -> Task<void> {
    big.work();                   // 或 co_await 某个持有 big& 的 awaiter
    co_return;
}();
```

ASan 报 `stack-use-after-scope`：闭包（含捕获）是完整表达式结束即析构的临时对象，
协程帧保存的 `this` 指针指向它，后续 resume 访问即越界。这与 cppreference `bad3` 完全一致，
是**用户侧 UB 的检测结果**，不是编译器的越界访问。

## 六、LLVM IR 真相（更正：此前的“定案”是误读）

此前的 LLVM IR 分析把 `store ptr %closure, ptr %frame` 判读为“编译器把捕获错存为闭包指针”。
**这是误读**——它恰恰是标准的协程参数模型：

```llvm
; main 的循环体：
%2 = alloca %class.anon, align 8            ; 闭包对象 = 栈上 alloca（完整表达式结束即析构）
store i32 %7, ptr %3                        ; 把 i 拷进闭包成员（正确）
%11 = invoke @_Znwm(i64 32)                 ; 分配协程帧
store ptr %2, ptr %14                       ; 帧保存隐式对象参数 this（指向闭包的指针）——符合标准

; resume 函数：
%2 = getelementptr i8, ptr %0, i64 24
%3 = load ptr, ptr %2                        ; 从帧里取出 this
%4 = load ptr, ptr %3                        ; 解引用读 order（闭包必须仍存活）
%12 = load i32, ptr %5                       ; 解引用读 closure->i（闭包必须仍存活）
```

标准要求协程状态按值拷贝**函数参数**；捕获属于闭包类型的成员，协程帧只保存 `this`。
用户代码让闭包（临时对象）在完整表达式结束时析构、之后才 resume —— 帧里的 `this` 悬垂，
解引用即 UB。**IR 本身是正确实现；问题出在用户代码违反闭包生命周期约束。**

> 更正记录：此前（2026-08-30 上午）我据此定案为“编译器 coroutine capture-to-frame lowering bug”，
> 并驳回了“闭包生命周期”解释。经 gcc 15 稳定版复现 + cppreference 权威示例核实，**该定性是错误的**。
> 正确结论：**这是用户侧 UB（coroutine lambda closure lifetime）**，GPT/豆包的说法正确。
> 四个编译器（gcc 15/16、clang 22/23）行为完全一致，正是“都正确实现标准、暴露同一 UB”的证明。

## 七、正确规避方式

1. **改用具名协程函数**（推荐，项目已全部完成）：
   ```cpp
   async::Task<void> doWork(async::AsyncEvent& e) { co_await e; /* ... */ }
   ```
2. **把捕获改成协程参数**：`[](int i) -> Task { ... }(i)` —— 参数按值拷入帧，安全。
3. **保证闭包存活**：把 lambda 存入具名变量，并确保其寿命覆盖协程的整个生命周期
   （例如存入 `std::function` / 保持局部变量存活），再调用。
4. 普通 lambda（作为回调传入 `runOn` / `run` / `transform` / `andThen` 等）不受影响，可放心使用。

## 八、给其它 AI 的说明（更正）

> 此前声称“编译器 coroutine lambda capture-to-frame lowering bug”，**经核实是错误结论**。
> 正确结论：立即调用的协程 lambda 在闭包析构后 resume 并读取捕获，是**标准明示的用户侧 UB**
> （见 cppreference 协程页 `bad3`/`good` 示例：`uses (anonymous lambda type)::i after free`）。
> 稳定版 gcc 15.2.0、gcc 16 trunk、clang 22/23 行为完全一致，进一步证实这是标准语义而非编译器缺陷。
> 无需求助编译器上游；需要规避的是**代码写法**（改具名函数 / 改参数 / 保活闭包）。

## 九、在 Vine 项目中的处理（结论更正，处理方式不变）

- 库自身代码保证**零协程 lambda**（`src/base/async/sdk/vine/async/*.hpp`）——这本身是正确的工程约束。
- 测试代码 `tests/test_async/AsyncTest.cpp` 中所有协程 lambda 已改写为具名协程函数——
  **这是消除 UB 的正确做法**（当初作为“绕开编译器 bug”的动机已不成立，但改写本身完全正确）。
- `async_global.hpp` 的 "Coroutine-lambda caveat" 已更新为正确的闭包生命周期警示。

## 十、完整实验矩阵（全部与“闭包生命周期”吻合）

| 实验 | 写法 | 结果 | 解释 |
|------|------|------|------|
| A-loop-value | `[&order, i]` 按值捕获循环变量，立即调用 | `4 4 4 4` / 段错误 | 闭包每轮析构 → 帧内 `this` 悬垂 → UB |
| A-named | 具名协程函数 `named(order, i)` | `1 2 3 4` ✅ | 参数按值拷入帧，无 UB |
| B-plain-lambda | 普通 lambda（非协程）`[&order, i]` | `1 2 3 4` ✅ | 非协程，无帧 |
| C-nonloop-value | 按值捕获非循环局部 `[&order, x]`（单任务） | `123` ✅ | **碰巧正确**：栈槽未被复用（仍是 UB） |
| D-fresh-local | 先复制到新局部 `captured` 再按值捕获 | `4 4 4 4` ❌ | 死的是闭包对象，不是变量 |
| E-constants | 显式 init-capture `[x=1..4]` | O0 ✅ → O1+ ❌ | 闭包临时对象存储复用的时序差异 |
| F-kept-closure | 闭包先存入 `std::function`，稍后再调用 | `1 2 3 4` ✅ | 闭包存活 → 帧内指针有效，无 UB |

Sanitizer：`-fsanitize=address` 报 `stack-use-after-scope`（READ of size 8），
与 cppreference `bad3` 标注的 "uses i after free" 完全吻合。

### 最终定性

```
[x] 用户代码 UB（立即调用协程 lambda，闭包析构后 resume 读捕获）
[x] Coroutine lambda closure lifetime 问题（用户侧）
[ ] Task ownership/lifetime 问题
[ ] Compiler frontend bug
[ ] Coroutine lowering bug —— capture-to-frame lowering 错误
[ ] LLVM optimizer/codegen bug
```

理由：
1. cppreference 协程页权威示例 `bad3` 与本模式一字不差，并明确标注 "uses i after free"。
2. 稳定版 gcc 15.2.0、gcc 16 trunk、clang 22/23 四个独立编译器行为完全一致——只可能是共同的标准语义。
3. 标准模型：协程状态按值拷贝**参数**；捕获是闭包成员，帧只保存隐式对象参数 `this`，
   **闭包必须活得比协程久**。IR 中的 `store ptr %closure` 正是这一模型的正确实现。
4. 修正写法（具名函数 / 参数传递 / 保活闭包）后全部正确，符合标准预期。

## 十一、附录：闭包生命周期快记（已核实，与上文结论一致）

### 一句话记忆

```
Lambda Capture            → Lambda Closure（不是 Frame）
Coroutine Parameter       → Coroutine Frame
跨 co_await 的 Local      → Coroutine Frame
```

> **`[x]` 只是把 `x` 放进 Lambda Closure，并不意味着 `x` 会自动进入协程帧。**

### 谁属于谁

| 对象 | 是否属于 Coroutine Frame |
|------|--------------------------|
| Lambda 按值捕获变量 | ❌ 属于 Closure |
| Lambda 按引用捕获变量 | ❌ 实体仍在原作用域，闭包甚至可不含成员 |
| 协程函数**按值**参数 | ✅ 通常拷入 Frame |
| 协程函数**按引用**参数 | ❌ 保持引用（同引用捕获，可能悬垂） |
| 跨 `co_await` 使用的局部变量 | ✅ 通常需要保存在 Frame |
| 不跨 suspend 的局部变量 | 不一定 |

### 关键精化（在总结之上补充三点）

1. **`init-capture` 同样属于闭包**。`[data = std::move(data)]() -> Task { co_await ...; use(data); }()`
   与 `[data]` 有**完全相同的 UB**——把对象 `std::move` 进捕获 ≠ 把对象拷进帧。
   移动只发生在闭包成员上，闭包析构后成员照样失效。
2. **帧里其实保存了指向闭包的指针**（隐式对象参数 `this`，按值拷入帧）。
   所以更精确的说法是：**闭包必须活得比协程久**；协程通过帧内的 `this` 访问捕获成员。
3. **按引用捕获/按引用参数**：`[&x]` 不拷贝 `x`，闭包内甚至可以没有成员，
   表达式直接指回原对象 `x`；若 `x` 在协程挂起期间死亡，同样 UB（cppreference `bad1`/`bad2`）。

### 生命周期陷阱示意

```
auto task = [data]() -> Task {
    co_await something();   // 挂起
    use(data);              // ← 若闭包已析构，data 失效
}();                        // ← 闭包临时对象在完整表达式结束时析构
```

**Lambda Capture 的生命周期 ≠ Coroutine Frame 的生命周期。**

### 如何保证数据活到协程结束

- 作为协程**按值参数**：`[](Data d) -> Task { ... }(std::move(data))` —— 拷入帧，最推荐。
- 作为协程体内**跨 suspend 的局部变量**：`co_await` 前声明，之后使用，编译器放入帧。
- 由 **Frame 中的对象**持有（如 promise 的成员 / 帧内对象的成员）。
- 或者**保活闭包**：把 lambda 存入具名变量并确保其寿命覆盖协程。

