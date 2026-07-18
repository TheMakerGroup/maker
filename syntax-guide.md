# Syntax Guide for Task Configuration File

>[!IMPORTANT]
> Please strictly abide by the guide set forth below.  
> Non‑compliance may cause **UNDEFINED BEHAVIOR** – ranging from maker crashes to **data corruption** at worst.

---

## 1. Overview
This document specifies the syntax and execution rules for a YAML‑based task configuration file.  
The format supports **declarative task definitions** with explicit dependencies, file‑based incremental checks, and sequential shell command execution.  
It is designed to automate common development workflows (building, cleaning, running, testing, etc.) while providing clear dependency management.

---

## 2. Basic File Structure
The configuration follows standard YAML syntax.  
All task definitions **must** be nested under the single top‑level key `tasks`.

Each task can be defined in **two styles**, each serving a distinct purpose:

- **New (structured) style** – a mapping with predefined fields. **Intended for file‑based real targets.**
- **Legacy (list) style** – a simple list of shell commands. **Intended for phony targets** (actions that always run).

```yaml
tasks:
  task_name_1:          # structured style (real target)
    depend: []
    input:  []
    output: []
    cmd:
      - step 1
      - step 2

  task_name_2:          # legacy style (phony target)
    - step 3
    - step 4
```

---

## 3. Core Syntax Rules

### 3.1 Task Definition
- Each task is declared as a key directly under the `tasks` mapping.
- Task names are custom identifiers (alphanumeric characters and underscores are recommended; avoid spaces).
- The value of a task can be:
  - A **mapping** (structured style) with the fields described below.
  - A **list** (legacy style) – each list item is a shell command; the maker automatically expands this to the structured form with empty `depend`, `input`, `output` and the list as `cmd`.

### 3.2 Structured Task Fields (Real Targets)
When using the mapping form, the following fields are recognised:

| Field      | Type          | Required? | Description |
|------------|---------------|-----------|-------------|
| `depend`   | list of task names | No (default `[]`) | Names of other tasks that must **complete successfully** before this task’s own commands run. The dependencies are executed in the order listed. |
| `input`    | list of file paths | No (default `[]`) | Files that are considered as input sources. Used for change‑detection (e.g., to decide whether the task needs to rerun). |
| `output`   | list of file paths | No (default `[]`) | Files that are produced by the task. Used for change‑detection and incremental builds. |
| `cmd`      | list of shell command strings | **Yes** (must be present in structured style) | The actual shell commands to execute, in sequential order. |

**Example (structured):**
```yaml
build:
  depend: []
  input:  [main.c, utils.c]
  output: [final]
  cmd:
    - gcc -Wall -Wextra -O2 -o final main.c utils.c
```

### 3.3 Legacy (List) Style – **Phony Targets**
For simplicity and backward compatibility, a task may be defined as a plain list of shell commands.  
The maker automatically interprets this as:

```yaml
task_name:
  depend: []
  input:  []
  output: []
  cmd:
    - <first command>
    - <second command>
    - ...
```

> **[!IMPORTANT] Design Intent – Phony Targets**  
> The legacy list style is **specifically intended to serve as phony targets** (like `.PHONY` in Makefiles).  
> Since it has no `input` or `output` files defined, it **will always execute** whenever it is invoked (via `depend` or `tasks.` reference), without any incremental skipping logic.  
> This makes it ideal for **action‑oriented** tasks such as `clean`, `run`, `test`, `deploy`, or `format`, where you always want the commands to run unconditionally.

**Example:**
```yaml
run:                     # Phony target – always runs
  - echo Running the program...
  - ./final

clean:                   # Phony target – always runs
  - echo Cleaning up...
  - rm -f *.o final
```

> **[!TIP] When to use which?**
> - Use **structured style** for tasks that produce files (e.g., `build`, `compile`, `package`) – they can leverage `input`/`output` for future incremental optimisations.
> - Use **legacy (list) style** for tasks that perform actions (e.g., `clean`, `run`, `test`, `lint`) – they are phony by design and always run fresh.

### 3.4 Shell Command Steps (inside `cmd`)
- Each item in the `cmd` list is a shell command string.
- Commands execute in the system shell in the order they appear.
- All valid shell syntax (flags, arguments, pipelines, redirections, echo, file operations) is supported.
- Lines starting with `#` **inside a command string** are treated as comments by the shell, not by the YAML parser (they remain part of the string).

**Example:**
```yaml
cmd:
  - gcc -Wall -Wextra -O2 -o final main.c   # This comment is part of the shell command
```

### 3.5 Task Dependencies (`depend` field)
- The `depend` field contains a list of task names (as strings) that must be executed **before** the current task’s own `cmd`.
- Dependencies are resolved **in order** – each named task runs completely (including its own dependencies) before proceeding to the next dependency.
- The `tasks.` prefix is **not** used inside `depend` – simply provide the task name as a string.
- If a dependency task is not defined, the maker exits with an error.
- Dependencies can mix both **real targets** (structured) and **phony targets** (legacy list) freely.

**Example:**
```yaml
default:
  depend: [build, test]   # runs build (real) then test (phony)
  cmd:
    - echo "All done!"
```

### 3.6 Task References inside `cmd` (Legacy Compatibility)
- For backward compatibility, you **may** still write `tasks.<task_name>` inside a `cmd` list.
- When such a reference is encountered, the referenced task runs **immediately** (including its own dependencies) at that point in the command sequence, then execution continues.
- However, **prefer using the `depend` field** for explicit dependency ordering – it makes the dependency graph clearer and enables future optimisation (e.g., parallel execution, change detection).
- Mixing both styles is allowed but not recommended for clarity.

**Example:**
```yaml
default:
  cmd:
    - tasks.build   # runs build (if not already run via depend)
    - tasks.run
```

### 3.7 Special `default` Task
- `default` is a reserved task name.
- When the maker is invoked **without** specifying a target task, it automatically executes the `default` task.
- `default` follows all the same syntax rules as any other task (structured or legacy). Often defined as a phony target (list style) or with `depend` to orchestrate the main workflow.

**Example:**
```yaml
default:
  depend: [build, run]
  cmd:
    - echo "Workflow completed."
```

---

## 4. Full Example (Structured Real Targets + Legacy Phony Targets)
```yaml
tasks:
  # ---- Real Targets (structured) ----
  build:
    depend: []
    input:  [main.c, utils.c]
    output: [final]
    cmd:
      - gcc -Wall -Wextra -O2 -o final main.c utils.c

  test:
    depend: [build]
    input:  [tests/*.c]
    output: [test.log]
    cmd:
      - echo "Running tests..."
      - ./run_tests > test.log

  # ---- Phony Targets (legacy list - always run) ----
  clean:
    - echo Cleaning up...
    - rm -f final *.o test.log

  run:
    - echo Running the program...
    - ./final

  lint:
    - echo Linting source code...
    - cppcheck --enable=all main.c utils.c

  # ---- Orchestration ----
  default:
    depend: [build, test]   # build and test run first
    cmd:
      - echo "Default workflow completed."
```

---

## 5. Execution Behaviour

1. **Dependency resolution** – When a task is invoked, its `depend` list is processed first. Each dependency task is executed completely (with its own dependencies) in the listed order. Cyclic dependencies are not allowed and will cause the maker to abort.

2. **Sequential command execution** – After all dependencies have finished, the commands in the `cmd` list are executed one after another, in order.

3. **Phony vs. Real semantics** – 
   - Tasks defined in **legacy list style** (with no `input`/`output`) are treated as **phony**. They are **never considered up‑to‑date** and will always execute when requested.
   - Tasks defined in **structured style** may use `input`/`output` for future incremental build optimisations (currently informational, but reserved for that purpose).

4. **In‑place expansion of `tasks.`** – If a command contains `tasks.X`, that reference is expanded in‑place at the moment of execution – the whole task X runs immediately, then execution resumes. This is a legacy feature; use `depend` for static dependencies.

5. **Error handling** – If any shell command exits with a non‑zero status, execution normally aborts (subject to the runner’s configuration). Dependency tasks that fail will also abort the current task.

6. **Working directory** – All commands execute in the directory where the configuration file resides.

---

## 6. Notes & Best Practices

- Use **consistent indentation** (spaces preferred over tabs) to avoid YAML parsing errors.
- **Avoid spaces in task names** – use underscores or hyphens instead. Unquoted task names with spaces cause YAML syntax errors.
- **Choose the right style for the right purpose**:
  - **Structured style** → for **file‑producing tasks** (e.g., `build`, `compile`). Define `input`/`output` to enable future incremental builds.
  - **Legacy (list) style** → for **phony/action tasks** (e.g., `clean`, `run`, `test`, `deploy`). They are guaranteed to always run, matching the classic `.PHONY` behaviour.
- **Use `depend` over `tasks.`** – it provides a cleaner dependency graph and is more maintainable, especially when mixing real and phony targets.
- **Keep tasks small and single‑purpose** – decompose complex workflows into small tasks and chain them via `depend`.
- **Add comments** to explain non‑obvious commands or the purpose of a task (YAML comments start with `#` outside strings).
- **Do not mix `depend` with `tasks.` references to the same task** – this can cause duplicate execution unless intended. Rely on `depend` to avoid repetition.

---

>[!CAUTION]
> In YAML, keys with spaces (like `task 1`) that are **not** enclosed in quotes will trigger parsing errors. Always quote such names or, better, avoid spaces entirely.

---

**End of Guide** – any deviation from the rules described above may lead to unpredictable results.  
When in doubt, refer to the full example or consult the maker’s documentation.