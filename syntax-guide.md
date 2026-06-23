# Syntax Guide for Task Configuration File

>[!IMPORTANT]
> Please strictly abside by the guide set forth below.
>Or might causing **UNDEFINED BEHAVIOR**.
>It may result in maker crashes at the best and **data corruption** at worst.

## 1. Overview
This document specifies the syntax and execution rules for a YAML-based task configuration file. This format is designed to automate common development workflows (building, cleaning, running programs, etc.) through declarative task definitions, supporting sequential shell command execution and cross-task dependency referencing.

## 2. Basic File Structure
The configuration follows standard YAML syntax. All task definitions must be nested under the single top-level key `tasks`.

```yaml
tasks:
  task_name_1:
    - step 1
    - step 2
  task_name_2:
    - step 3
```

## 3. Core Syntax Rules

### 3.1 Task Definition
- Each task is declared as a key directly under the `tasks` mapping object.
- Task names are custom identifiers (alphanumeric characters and underscores are recommended).
- The value of every task must be a **YAML list** (sequence). Each item in the list represents one execution step, and steps run strictly sequentially from top to bottom.

### 3.2 Shell Command Steps
A step can be a plain shell command string:
- Commands are executed in the system shell in the order they appear.
- All valid shell syntax (flags, arguments, echo statements, file operations) is supported inside the command string.
- Lines starting with `#` are treated as comments and ignored by the parser.

Example:
```yaml
build:
  - gcc -Wall -Wextra -O2 -o final main.c # Compile source code
```

### 3.3 Task Reference Steps
To reuse logic from another task, use the reference format `tasks.<task_name>`:
- When the executor encounters a task reference, it runs all steps of the referenced task completely, then resumes executing the remaining steps of the current task.
- This mechanism is used to define task dependencies and composite workflows.

Example:
```yaml
default:
  - tasks.build  # Run the entire "build" task first
  - tasks.run    # Then run the entire "run" task
```

>[!NOTE]
>If reference task is invalid or not defined, maker will exit to keep data save.

### 3.4 Special `default` Task
- `default` is a reserved special task.
- When the task runner is launched without specifying a target task, it automatically executes the `default` task.
- The `default` task follows the same syntax rules as regular tasks, and typically consists of references to other functional tasks.

## 4. Full Example Breakdown
Below is the complete sample configuration:

```yaml
tasks:
  build:
    - gcc -Wall -Wextra -O2 -o final main.c #Build command

  clean:
    - echo Cleaning up...
    - rm -f final

  run:
    - echo Running the program...
    - ./final
  
  default:
    - tasks.build
    - tasks.run
```

## 5. Execution Behavior
1. **Sequential execution**: Steps within a task run one after another in written order.
2. **In-place expansion**: A `tasks.X` reference is replaced in-place by the full step list of task X during execution.
3. **Conventional error handling**: If any shell command exits with a non-zero status code, execution will normally abort (subject to the runner's configuration).
4. **Working directory**: All commands execute in the directory where the configuration file resides.

## 6. Notes & Best Practices
- Use consistent indentation.
- Not using spaces in task names to prevent parsing ambiguity.
- For complex workflows, decompose logic into small, single-purpose tasks and combine them via references.
- Use comments to document non-obvious commands or task purposes.

>[!CAUTION]
> In YAML, keys with spaces like `task 1` that are not enclosed in quotes will trigger parsing errors.