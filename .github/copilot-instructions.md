# C++ Coding Guidelines

## Language Standard

* Use C++20.
* Prefer standard library solutions.
* Do not introduce third-party dependencies unless explicitly requested.
* Keep code portable across Windows and Linux.

## Naming Convention

### Functions

* Static member functions use camelCase, starting with an lowercase letter (e.g. `getName() const`).
* Instance member functions use camelCase, starting with a lowercase letter (e.g. `setName()`, `loadMesh()`).
* Free (global) functions use camelCase, starting with an lowercase letter (e.g. `getAppConfig()`).

### Property Access (Getters / Setters)

* Use Qt-style accessors:
  * Getters use the plain property name without a `get` prefix (e.g. `name()`, `width()`).
  * Boolean getters use an `is` / `has` prefix (e.g. `isEnabled()`, `hasDefault()`).
  * Setters use a `set` prefix (e.g. `setName(...)`, `setEnabled(bool)`).
* Do NOT use Java-style `getXXX()` / `setXXX()` getters.
* Exception: fluent builder APIs are exempt from this rule. A setter that returns the
  object itself for chaining uses the property name without a `set` prefix and returns
  a reference (e.g. `ConfigItem& description(const String&)`, `ConfigItem& range(int, int)`).


### Fields

Fields use `snake_case`:

* `private` and `protected` fields get a trailing underscore `_` (e.g. `file_path_`, `val3_`).
* `public` fields do NOT get a trailing underscore (e.g. `val1`).
* Class static fields additionally get an `s_` prefix (e.g. `s_count`, `s_val2`, `s_val5_`).
* Under PImpl:
  * The `impl` member does NOT get a trailing underscore (`Impl* const impl;` or `std::unique_ptr<Impl> impl;`).
  * Fields of the `Impl` object do NOT get a trailing underscore.
* The Qt Designer generated UI member `Ui::XXX ui;` does NOT get a trailing underscore, regardless of access level (even when private).
* Local variables use `snake_case` without a trailing underscore.

### Class Layout

For simple classes, group members with access control specifiers (`public:` /
`protected:` / `private:`) and lay them out in blocks in the following order:

1. Type declarations (`enum`, `using`, `friend`).
2. Constructors / destructor.
3. Methods.
4. Fields.

The block names below (类型声明区块 / 构造函数区块 / 方法区块 / 字段区块) are
labels used ONLY to describe what each block holds. They are NOT comments and
MUST NOT be inserted into the code: write the layout with plain access-specifier
blocks and no block-label comments.

Example:

```cpp
class Cls
{
  public:
    enum Type {};
    using Id = std::uint64_t;
    friend class Xxx;

  public:
    Cls();
  protected:
    Cls(Val v);

  public:
    void method1(int param);
  protected:
    void method2();
  private:
    void method3();

  public:
    int val1{ 1 };
    inline static int s_val2{ 1 };
  protected:
    int val3_{ 1 };
  private:
    int val4_{ 1 };
    inline static int s_val5_{ 1 };

    struct Impl;
    Impl* const impl;  // or std::unique_ptr<Impl> impl;
};
```

## Header Include Order

### .hpp files

Group includes in this order, separated by blank lines:

1. The project `xxxGlobal.hpp` first (if any).
2. C++ standard library headers.
3. Third-party library headers.
4. Local library headers.
5. Same-directory headers (quoted).

Example:

```cpp
#include <string>
#include <vector>

#include <Eigen/xxx>

#include <MYLib/xxx>

#include "local.h"
```

### .cpp files

The first include is the header corresponding to this `.cpp` file; the remaining includes follow the same grouping as `.hpp` files.

### Exceptions

Headers with special requirements (e.g. `GL.h` which must be included first) may be handled specially.


## Comments and Documentation

* Public interfaces must use Doxygen-style comments.
* Do not create section comments.
* Use `/** @brief ... */` format with `@`-prefixed tags.
* Any function with a return value must document it with `@return`.
* Prefer multi-line comments; do not condense them into a single line.
* Keep each sentence on one line: wrap to a new line after a sentence completes, so a full sentence occupies one line. A line should generally not exceed 160 characters (you may break earlier to keep a sentence on its own line).

Example:

```cpp
/**
 * @brief Loads mesh data from a file.
 *
 * @param file_path Path to the mesh file.
 * @return true if loading succeeds.
 */
bool loadMesh(const std::filesystem::path& file_path);
```

* Comments should explain design intent, constraints, and non-obvious logic.
* Do not add comments that only restate the code.

## Code Quality

* Prefer RAII.
* Avoid raw owning pointers.
* Use `std::span` for non-owning contiguous data access.
* Use `constexpr` where appropriate.
* Use `noexcept` when correctness allows.
* Consider thread safety when shared state is involved.

## API Design

* Do not change existing public APIs unless explicitly requested.
* Preserve existing interfaces and behavior.
* Avoid unnecessary abstractions.
* Prefer simple, maintainable designs.

## File Modification Safety

Before editing any file:

1. Check the latest file contents from the workspace.
2. Do not rely on previously generated code if the file may have changed.
3. Preserve user modifications.
4. Never silently overwrite user changes.
5. If generated changes conflict with user edits, explain the conflict before replacing code.

## Code Generation Rules

When generating code:

* Provide complete compilable implementations.
* Do not provide pseudo-code unless explicitly requested.
* Include required headers.
* Match the existing project style.

## Review Rules

When reviewing code, check:

* Correctness.
* Undefined behavior.
* Memory safety.
* Exception safety.
* Performance issues.
* Thread safety.
* API consistency.
* Naming and style compliance.
